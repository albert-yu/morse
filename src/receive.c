#include <stdio.h>
#include <stddef.h>

#include "authenticate.h"
#include "curlyfries.h"
#include "dataconv.h"
#include "imapcmd.h"
#include "imap_request.h"
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
    if (!curl || !command) {
        return NULL;
    }
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

    // is this carriage return or newline feed?
    char *line_sep; 
    if (newline_char == RET_NEWLINE) {
        line_sep = "\n";
    } else {
        line_sep = "\r\n";
    }

    lines = tokenize_into_list(imap_output, line_sep);
        
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
size_t get_num_messages_from_str(char *select_box_response) {
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

    // clean up curl slist
    curl_slist_free_all(lines);

    // get the list of messages
    size_t total_count = 0;
    if (total_msg_count_as_str) {
        // printf("total count: %s\n", total_msg_count_as_str);
        total_count = decimal_to_size_t(total_msg_count_as_str);
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


size_t get_msg_count(CURL *curlhandle, char *box_name) {
    char *select_box_command = imapcmd_select_box(box_name);
    ImapResponse *response_box = morse_exec_imap_stateful(curlhandle, select_box_command);  
    free(select_box_command);
    if (!response_box) {
        fprintf(stderr, "Could not select box with name %s.\n", box_name);
        return 0;
    }
    if (response_box->status != 0) {
        fprintf(stderr, 
            "Response returned status with error code %d\n", 
            response_box->status);
        return 0;
    }
    size_t total_messages_in_box = get_num_messages_from_str(response_box->data->memory);
    imap_response_free(response_box);
    return total_messages_in_box;
}


/*
 * Gets the specified range of messages in a given box name.
 */
MailMessage* get_messages(CURL *curlhandle, 
                          char *box_name, 
                          size_t start, 
                          size_t length) {
    size_t total_messages_in_box;
    total_messages_in_box = get_msg_count(curlhandle, box_name);
    if (start > total_messages_in_box) {
        // start shouldn't be greater than number of messages
        fprintf(stderr, 
            "Requested starting index (%zu) is greater than the total number of messages (%zu).\n", 
            start, total_messages_in_box);
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
        mailmessage_setdefault(curr_msg);
        curr_msg->uid = extract_id_from(curr_line);
        i++; 
        item = next;
    } while (next);

    free(response_lines);
    return messages;                        
}


/*
 * Gets the subject associated with the given ID.
 * Need to re-use an existing CURL handle which has
 * already performed SELECT {box}.
 */
char* get_subject_stateful(CURL *curl, size_t msg_id) {
    char *subj = NULL;
     
    char *cmd_to_get_subject = imapcmd_id_get_subject(msg_id);
    ImapResponse *response;
    response = morse_exec_imap_stateful(curl, cmd_to_get_subject);
    if (response && response->status == 0) {
        subj = strdup(response->data->memory);
    }
    else {
        fprintf(stderr, "IMAP call was unsuccessful. Returned %d.\n", 
            response->status);
    }
    if (response) {
        imap_response_free(response);
    }

    free(cmd_to_get_subject);
    return subj; 
}

/*
 * Populates the messages' subjects.
 */
void populate_msgs_subjects(CURL *curlhandle, 
                            MailMessage *msgs,
                            size_t msg_count) {
    if (msgs) {
        for (size_t i = 0; i < msg_count; i++) {
            msgs[i].metadata = malloc(sizeof(MailMetadata));
            mailmetadata_setdefault(msgs[i].metadata);
            msgs[i].metadata->subject = get_subject_stateful(
                curlhandle, msgs[i].uid);
        }
    }
}


/*
 * Returns the lines from the command 
 * LIST "" *
 */
struct curl_slist* get_list_cmd_result(CURL *curl) {
    char *command = imapcmd_list_boxes();
    ImapResponse *resp = morse_exec_imap_stateful(curl, command);
    free(command);

    struct curl_slist *mailboxes = NULL;
    if (resp && resp->status == 0) {
        mailboxes = get_response_lines(resp->data->memory);
    }
    return mailboxes;
}


/*
 * Converts a line in the IMAP response returned by
 * get_list_cmd_result into a Mailbox, parsing out 
 * child mailboxes as well.
 * Returns NULL if an invalid string.
 * Examples:
 * LIST (\HasNoChildren) "/" "INBOX"
 * LIST (\HasChildren) "/" "Receipts"
 * LIST (\HasNoChildren) "/" "Receipts/Venmo"
 */
Mailbox* convert_str_to_mailbox(const char *str) {
    Mailbox *box = NULL;
   
    char *copied = strdup(str);
    if (!copied) {
        fprintf(stderr, "Not enough memory to copy string!\n");
        return box; // NULL at this point
    }

    char *beg = copied;

    // isolate the attributes and the name
    // the middle is separated by "/"
    const char *separator = "\"/\"";
    size_t sep_len = strlen(separator);

    // should point to the location
    // of "/" in the string
    char *sep;
    sep = strstr(beg, separator); 
    
    char *boxname = NULL;
    if (sep) {
        // get the name from the right side
        // e.g. "/" "INBOX"
        //      ^
        char *right_side = sep;

        // move past the forward slash separator
        // e.g. "/" "INBOX"
        //         ^
        right_side += sep_len;      
        
        // create a copy that we will modify
        char *rt_side_dup = strdup(right_side);

        // find the first quote
        char quote = '"';
        char *frst_quote = strchr(rt_side_dup, quote);
        if (frst_quote) {
            char *ptr = frst_quote;
            ptr++;
            char *sec_quote = strchr(ptr, quote);
            if (sec_quote) {
                // null out to terminate
                *sec_quote = '\0';

                // copy to new string
                boxname = strdup(ptr);
            }
        }

        free(rt_side_dup);
    }

    // stores the attributes
    struct curl_slist *linked_list_attrs = NULL;
    if (boxname) {
        box = mailbox_create_new(boxname);
        free(boxname);

        // find the locations 
        char *left_parens_ptr = strchr(beg, '(');
        char *right_parens_ptr = strchr(beg, ')');
        
        // are they in the correct order?
        ptrdiff_t dist = right_parens_ptr - left_parens_ptr;
        if (dist > 1) {
            // should have a valid parentheses pair, so
            // make copy of string, ignoring the left parens...
            char *begin_attr = left_parens_ptr + 1;

            // ... and the right parens (hence, dist - 1);
            char *attr_list = strndup(begin_attr, dist - 1);

            // tokenize
            char *whitespace = " ";
            linked_list_attrs = tokenize_into_list(attr_list, whitespace);
            free(attr_list);
        }
    }

    // copy the attributes over if found
    if (linked_list_attrs && box) {
        // iterate through list
        struct curl_slist *item;
        struct curl_slist *next;
        item = linked_list_attrs;
        do {
            next = item->next;
            char *property = item->data;
            // attributes begin with a backslash
            if (property && (*property == '\\')) {
                char *attr_ptr = property + 1;
                mailbox_add_attr(box, attr_ptr); 
            }
            item = next;
        } while (next);

        curl_slist_free_all(linked_list_attrs);
    }
    free(copied);
    copied = NULL; 
     
    // TODO: check if HasChildren attribute applies
    return box;
}


/*
 * Executes the IMAP command to retrieve the raw lines.
 * Then parses them into Mailbox structures.
 * Returns the pointer to the ROOT box which has the 
 * name defined by ROOT_MAILBOX_NAME in mailbox.h.
 * Returns NULL on failure.
 */
Mailbox* get_mailboxes(CURL *curl) {
    if (!curl) {
        fprintf(stderr, "NULL curl handle passed into get_mailboxes.\n");
        return NULL;
    }

    Mailbox *rootnode = NULL;
    struct curl_slist *raw_lines = get_list_cmd_result(curl);
    rootnode = mailbox_create_new_root();

    if (rootnode && raw_lines) {
        size_t child_count = 0;
        // iterate through linked list
        struct curl_slist *item;
        struct curl_slist *next;
        item = raw_lines;
        do {
            child_count++;
            next = item->next;
            // this curr_box SHOULD be heap-allocated
            Mailbox *curr_box = convert_str_to_mailbox(item->data);
            mailbox_add_child(rootnode, curr_box);
            item = next; 
        } while (next);
        
        // notify if child_count does not match 
        if (child_count != rootnode->child_count) {
            fprintf(stderr, "Expected %zu attrs, but could"
                            " only add %zu.\n",
                            child_count,
                            rootnode->child_count);
        }
    }

    return rootnode;
}


void print_mailboxes(Mailbox *root) {
    if (root) {
        for (size_t i = 0; i < root->child_count; i++) {
            printf("[%zu]:\t", i);
            Mailbox *child = (root->children)[i];
            if (child) {
                printf("name: %s\n", child->name);
                for (size_t j = 0; j < child->attr_count; j++) {
                    printf("\t%s\n", (child->attrs)[j]);
                }
            }
        }
    }
}


void list_last_n(char *box_name, size_t n) {
    CURL *curl;
    curl = get_imap_curl_google();

    size_t num_messages = get_msg_count(curl, box_name);
    printf("Total message count in %s: %zu\n", 
        box_name, num_messages);
    size_t length = (n > num_messages) ? num_messages : n;
    MailMessage *mailmessages;
    mailmessages = get_messages(curl, box_name, 
        num_messages - length + 1, length);

    populate_msgs_subjects(curl, mailmessages, length);

    if (mailmessages) {
        for (size_t i = 0; i < length; i++) {
            printf("%zu: %s\n", mailmessages[i].uid, 
                mailmessages[i].metadata->subject);
            mailmessage_free(mailmessages + i);
        }
        free(mailmessages);
    }
    curl_easy_cleanup(curl);
}
