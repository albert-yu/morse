#include <stdio.h>

#include "authenticate.h"
#include "curlyfries.h"
#include "dataconv.h"
#include "imapcmd.h"
#include "imap_request.h"
#include "imap_response.h"
#include "mailmessage.h"
#include "memstruct.h"
#include "receive.h"


/**
 * Caller must free returned buffer
 */
char* construct_url(char *base_url, char *path) {
    if (!base_url) {
        fprintf(stderr, "Null base URL supplied.\n");
        return NULL;
    }
    int path_is_nonempty = 1;
    if (!path) {
        path_is_nonempty = 0;
    }

    size_t total_len;
    
    if (path_is_nonempty) {
        total_len = strlen(path) + strlen(base_url);
    }
    else {
        total_len = strlen(base_url);
    }
    // total_len + 2 to account for forward slash
    char *buffer = calloc(total_len + 2, sizeof(*buffer));
    if (!buffer) {
        fprintf(stderr, "Failed to allocate memory.\n");
        return NULL;
    }

    if (path_is_nonempty) {
        sprintf(buffer, "%s/%s", base_url, path);
    }
    else {
        strcpy(buffer, base_url);
    }
    return buffer;
}


ImapResponse* morse_exec_imap_stateful(CURL *curl, char *command) {
    ImapResponse *response = imap_response_new();

    // set a timeout
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 60L);

    // verbose output
    // curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

    // COMMAND GOES HERE
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, command);

    // set callback
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &curl_mem_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)response->data);

    /* Perform the fetch */
    CURLcode res = curl_easy_perform(curl);

    /* Check for errors */
    if (res != CURLE_OK) {
        fprintf(stderr, "curl_easy_perform() failed: %s\n",
            curl_easy_strerror(res));
        response->status = (int)MorseStatus_CurlError;
    }

    return response;
}


int morse_exec_imap_xoauth2(const char *bearertoken, 
                            const char *imap_url,
                            const char *username,
                            const char *command,
                            MemoryStruct *mem  
                            ) {
    MorseStatusCode result = MorseStatus_OK; 
    CURL *curl;
    CURLcode res = CURLE_OK;

    if (!mem) {
        fprintf(stderr, "Passed in memory struct cannot be null.\n");
        return (int)MorseStatus_InvalidArg;
    }

    if (!mem->memory) {
        fprintf(stderr, "Passed in memory struct doesn't have memory.\n");
        return (int)MorseStatus_MemoryError;
    }

    curl = get_curl_xoauth2(bearertoken, imap_url, username);
    if (curl) {
        printf("curl init OK [IMAP]\n");

        // set a timeout
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 60L);

        // verbose output
        // curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

        // COMMAND GOES HERE
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, command);

        // set callback
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &curl_mem_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)mem);

        /* Perform the fetch */
        res = curl_easy_perform(curl);

        /* Check for errors */
        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(res));
            result = MorseStatus_CurlError;
        }

        /* Always cleanup */
        curl_easy_cleanup(curl);
    }

    return (int)result;
}


typedef enum retchar_t {
    /* this string contains no newline chars */
    RET_NONE,

    /* this string uses \n */
    RET_NEWLINE,

    /* this string uses \r\n */
    RET_CARRIAGE
} ReturnChar;


ReturnChar get_return_type(char *string) {
    ReturnChar retchar = RET_NONE;

    if (!string) {
        // this is technically true
        // in the event of a NULL ptr
        return retchar;
    }

    // check if we have a newline char
    char *ptr = strchr(string, '\n');
    if (!ptr) {
        return retchar; // NO return characters
    }
   
    retchar = RET_NEWLINE;
    char *r_ptr = strchr(string, '\r');

    if (r_ptr && (r_ptr[1] == '\n')) {
        retchar = RET_CARRIAGE;  
    }
    return retchar;
}


/*
 * Tokenizes a string into a list of strings.
 */
struct curl_slist* tokenize_into_list(const char *string, char *token) {
    struct curl_slist *parts = NULL;

    char *copy_of_str = strdup(string);
    if (!copy_of_str) {
        fprintf(stderr, "Not enough memory!\n");
        return NULL;
    }

    // save ptr
    char *ptr = NULL;
    ptr = strtok(copy_of_str, token);
    while (ptr != NULL) {
        parts = curl_slist_append(parts, ptr);
        ptr = strtok(NULL, token);
    }

    free(copy_of_str);
    return parts;
}


/*
 * Gets each line of an IMAP response.
 * Caller must free the returned list.
 */
struct curl_slist* get_response_lines(char *imap_output) {
    // using curl's built-in linked list
    struct curl_slist *lines = NULL;
    
    // determine type of return (\r\n or \n)
    ReturnChar newline_char = get_return_type(imap_output);
        
    if (newline_char == RET_NONE) {
        // just return the input as the first element
        // of the linked list
        lines = curl_slist_append(lines, imap_output);
        return lines;
    }

    // // do not modify original string 
    // char *copy_of_imap_output = strdup(imap_output); if (!copy_of_imap_output) {
    //     fprintf(stderr, "Not enough memory for IMAP output\n");
    //     return NULL;
    // }

    // save ptr
    // char *ptr = NULL;

    // is this carriage return or newline feed?
    char *line_sep; 
    if (newline_char == RET_NEWLINE) {
        line_sep = "\n";
    } else {
        line_sep = "\r\n";
    }

    lines = tokenize_into_list(imap_output, line_sep);
    // ptr = strtok(copy_of_imap_output, line_sep);
    // while (ptr != NULL) {
    //     lines = curl_slist_append(lines, ptr);
    //     ptr = strtok(NULL, line_sep);
    // }

    // free(copy_of_imap_output);
        
    return lines;
}


CURL* get_imap_curl_google() {
    char *base_url = GOOGLE_IMAPS;
    char *gmail_imap_url = construct_url(base_url, "INBOX");
    CURL *curl = get_google_curl(gmail_imap_url);
    return curl;
}


/*
 * Reads the response from a SELECT {boxname} command
 * by looking for EXISTS
 */
size_t get_num_messages_from(char *select_box_response) {
    struct curl_slist *lines = get_response_lines(select_box_response);

    // iterate through to find how many exist in the
    // given box
    struct curl_slist *next;
    struct curl_slist *item;
    item = lines;
    char *total_msg_count_as_str = NULL;
    int found = 0;
    do {
        next = item->next;
        char *content = item->data;
        struct curl_slist *words = tokenize_into_list(content, " ");
        struct curl_slist *inner_next; 
        struct curl_slist *inner_item; 
        inner_item = words; 

        do {
            inner_next = inner_item->next;
            if (inner_next) {
                // get the next word
                char *word = inner_next->data;
                if (strcmp(word, "EXISTS") == 0) {
                    // number should be in the current node 
                    total_msg_count_as_str = strdup(inner_item->data);
                    found = 1;
                    break;
                }
            }
            inner_item = inner_next;
        } while (inner_next);
                        
        if (found) {
            break;
        }              
        item = next;
    } while (next);

    // get the list of messages
    size_t total_count = 0;
    if (total_msg_count_as_str) {
        size_t total_count = decimal_to_size_t(total_msg_count_as_str);
        free(total_msg_count_as_str);
    }
    else {
        fprintf(stderr, "Could not get the number of messages in box.\n");
    }
    return total_count;
}


size_t extract_id_from(const char *line_containing_id) {
    struct curl_slist *words;
    words = tokenize_into_list(line_containing_id, " ");

    struct curl_slist *item;
    struct curl_slist *next;

    size_t id = 0; // invalid ID, as IMAP starts from 1
    item = words;
    do {
        next = item->next;

        // find first decimal string
        char *str = item->data;
        if (is_decimal_str(str)) {
            id = decimal_to_size_t(str);
        } 
        item = next;
    } while (next);

    curl_slist_free_all(words);
    return id;
}



/*
 * Gets the specified range of messages in a given box name.
 */
MailMessage* get_messages(CURL *curlhandle, 
                          char *box_name, 
                          size_t start, 
                          size_t length) {
    char *select_box_command = imapcmd_select_box(box_name);
    ImapResponse *response_box = morse_exec_imap_stateful(curlhandle, select_box_command);  
    free(select_box_command);
    if (!response_box) {
        fprintf(stderr, "Could not select box with name %s.\n", box_name);
        return NULL;
    }
    if (response_box->status != 0) {
        fprintf(stderr, 
            "Response returned status with error code %d\n", 
            response_box->status);
    }
    size_t total_messages_in_box = get_num_messages_from(response_box->data->memory);
    imap_response_free(response_box);
    if (start > total_messages_in_box) {
        // start shouldn't be greater than number of messages
        return NULL;
    }

    // truncate number of messages if requested length leads 
    // to overflow
    size_t num_messages; // number of messages to retrieve
    num_messages = (start + length) > total_messages_in_box 
                           ? (total_messages_in_box - start + 1)
                           : (length);

    // allocate array memory
    MailMessage *messages = calloc(num_messages + 1, sizeof(*messages));
    if (!messages) {
        fprintf(stderr, "Not enough memory for %zu mail messages.\n", num_messages);
        return NULL;
    }     

    char *list_message_ids_cmd = imapcmd_fetch_messages(
        start, num_messages);
    ImapResponse *message_ids_resp = morse_exec_imap_stateful(
        curlhandle, list_message_ids_cmd);
    free(list_message_ids_cmd);

    struct curl_slist *response_lines;
    response_lines = get_response_lines(
        message_ids_resp->data->memory);

    // iterate through and get the IDs
    struct curl_slist *item;
    struct curl_slist *next; 

    item = response_lines;
    size_t i = 0;
    do {
        next = item->next;
        char *curr_line = item->data;

        // populate a mail response
        MailMessage *curr_msg;
        curr_msg = messages + i;        
        curr_msg->uid = extract_id_from(curr_line);
        i++; 
        item = next;
    } while (next);

    free(response_lines);
    return messages;                        
}


void list_last_n(char *box_name, size_t n) {
    // char *command1 = imapcmd_select_box(box_name);
    CURL *curl;
    curl = get_imap_curl_google();
    // ImapResponse *response1 = morse_exec_imap_stateful(curl, command1);
    // if (response1) {
    //     if (response1->status == 0) {
    //         printf("%s\n", response1->data->memory);
    //         // get each line
    //         struct curl_slist *lines = get_response_lines(response1->data->memory);

    //         // iterate through to find how many exist in the
    //         // given box
    //         struct curl_slist *next;
    //         struct curl_slist *item;
    //         item = lines;
    //         char *total_msg_count_as_str = NULL;
    //         int found = 0;
    //         do {
    //             next = item->next;
    //             char *content = item->data;
    //             struct curl_slist *words = tokenize_into_list(content, " ");
    //             struct curl_slist *inner_next; 
    //             struct curl_slist *inner_item; 
    //             inner_item = words; 

    //             do {
    //                 inner_next = inner_item->next;
    //                 if (inner_next) {
    //                     // get the next word
    //                     char *word = inner_next->data;
    //                     // printf("%s\n", word);
    //                     if (strcmp(word, "EXISTS") == 0) {
    //                         // number should be in the current node 
    //                         total_msg_count_as_str = strdup(inner_item->data);
    //                         found = 1;
    //                         break;
    //                     }
    //                 }
    //                 inner_item = inner_next;
    //             } while (inner_next);
                                
    //             curl_slist_free(words);
    //             if (found) {
    //                 break;
    //             }              
    //             item = next;
    //         } while (next);

    //         curl_slist_free(lines);

    //         // get the list of messages
    //         size_t total_count = 0;
    //         if (total_msg_count_as_str) {
    //             size_t total_count = decimal_to_size_t(total_msg_count_as_str);
    //             // printf("total: %zu\n", total_count);
    //             free(total_msg_count_as_str);

    //             // IMAP server throws an error if 
    //             // one requests more messages than exists
    //             if (n > total_count) {
    //                 n = total_count;
    //             }
    //             char *command2 = imapcmd_list_messages(total_count, n);
    //             // printf("%s\n", command2);
    //             ImapResponse *second_resp = morse_exec_imap_stateful(curl, command2);
    //             if (second_resp) {
    //                 printf("%s\n", second_resp->data->memory);
    //                 imap_response_free(second_resp);
    //             }
    //             free(command2);
    //         }
    //     }
    //     imap_response_free(response1);
    // }

    // free(command1);
    MailMessage *mailmessages;
    mailmessages = get_messages(curl, box_name, 7000, 10);

    for (size_t i = 0; i < 10; i++) {
        printf("%zu\n", mailmessages[i].uid);
    }
    free(mailmessages);
    curl_easy_cleanup(curl);
}


int morse_list_folders() {
    char *command1 = imapcmd_select_box("[Gmail]/All Mail");
    char *command2 = imapcmd_list_messages(41916, 100);
    CURL *curl;
    curl = get_imap_curl_google();
    ImapResponse *response1 = morse_exec_imap_stateful(curl, command1);
    if (response1) {
        if (response1->status == 0) {
            printf("%s\n", response1->data->memory);
            printf("\n");
            ImapResponse *second_resp = morse_exec_imap_stateful(curl, command2);
            if (second_resp) {
                printf("%s\n", second_resp->data->memory);
                imap_response_free(second_resp);
            }
        }
        imap_response_free(response1);
    }

    free(command1);
    free(command2);
    curl_easy_cleanup(curl);
    return 0;
}

