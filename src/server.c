/**
 * webserver.c -- A webserver written in C
 * Majority of code comes from https://github.com/LambdaSchool/C-Web-Server
 * 
 * Test with curl (if you don't have it, install it):
 * 
 *    curl -D - http://localhost:3490/
 *    curl -D - http://localhost:3490/d20
 *    curl -D - http://localhost:3490/date
 * 
 * You can also test the above URLs in your browser! They should work!
 * 
 * Posting Data:
 * 
 *    curl -D - -X POST -H 'Content-Type: text/plain' -d 'Hello, sample data!' http://localhost:3490/save
 * 
 * (Posting data is harder to test from a browser.)
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <time.h>
#include <sys/file.h>
#include <fcntl.h>
#include "net.h"
#include "file.h"
#include "mime.h"
#include "cache.h"
#include "server.h"

#define SERVER_FILES "./serverfiles"
#define SERVER_ROOT "./serverroot"
#define HTTP_200 "HTTP/1.1 200 OK"
#define HTTP_404 "HTTP/1.1 404 NOT FOUND"

/**
 * Send an HTTP response
 *
 * header:       "HTTP/1.1 404 NOT FOUND" or "HTTP/1.1 200 OK", etc.
 * content_type: "text/plain", etc.
 * body:         the data to send.
 * 
 * Return the value from the send() function.
 */
int send_response(int fd, char *header, char *content_type, void *body, int content_length) {
    const int max_response_size = 65536;
    char response[max_response_size];
    // Build HTTP response and store it in response
    char type_header[512];
    sprintf(type_header, "Content-Type: %s", content_type);

    char length_header[256];
    sprintf(length_header, "Content-Length: %d", content_length);
    // printf("foo\n");
    char *p_body = (char*)body;
    // printf("%s\n", p_body);
    int response_length = 
        sprintf(response, "%s\n%s\n%s\n\n%s\n", header, type_header, length_header, p_body);
    printf("Response: \n%s\n", response);
    // Send it all!
    int rv = send(fd, response, response_length, 0);

    if (rv < 0) {
        perror("send");
    }

    return rv;
}


int str_starts_with(const char *string, const char *substring)
{
   if(strncmp(string, substring, strlen(substring)) == 0) return 1;
   return 0;
}


/**
 * Send a /d20 endpoint response
 */
void get_d20(int fd) {
    // Generate a random number between 1 and 20 inclusive
    srand((unsigned int)time(NULL));
    int num = rand() % 20 + 1;
    int length = (num < 10) ? 1 : 2;
    // Use send_response() to send it back as text/plain data
    char *mime_type = MIME_TYPE_PLAIN;
    char *response_header = HTTP_200;
    char result[4];
    sprintf(result, "%d", num);

    send_response(fd, response_header, mime_type, result, length);
}


/**
 * Respond with a generic success operation
 */
void resp_success(int fd) {
    char *message = "Success. Return to the app.";
    int length = strlen(message);
    char *mime_type = MIME_TYPE_PLAIN;
    char *response_header = HTTP_200;
    send_response(fd, response_header, mime_type, message, length);
}


/**
 * Send a 404 response
 */
void resp_404(int fd) {
    char filepath[4096];
    struct file_data *filedata; 
    char *mime_type;

    snprintf(filepath, sizeof filepath, "%s/404.html", SERVER_FILES);
    filedata = file_load(filepath);

    if (filedata == NULL) {
        // send plain text
        char *message = "Not found.";
        int msglength = 10;

        send_response(fd, HTTP_404, "text/plain", message, msglength);
        return;
    }

    mime_type = mime_type_get(filepath);

    send_response(fd, HTTP_404, mime_type, filedata->data, filedata->size);

    file_free(filedata);
}


/**
 * Read and return a file from disk or cache
 */
void get_file(int fd, struct cache *cache, char *request_path) {
    char *empty = "";
    char *root = "/";
    if (strcmp(request_path, empty) == 0 || strcmp(request_path, root) == 0) {        
        get_file(fd, cache, "/index.html");
        return;
    }

    // get local path
    char filepath[4096];
    sprintf(filepath, "%s%s", SERVER_ROOT, request_path);

    struct file_data *filedata;
    filedata = file_load(filepath);
    if (filedata == NULL) {
        resp_404(fd);
        return;
    }

    char *mime_type = mime_type_get(request_path);

    send_response(fd, HTTP_200, mime_type, filedata->data, filedata->size);
    if (mime_type) {
        free(mime_type);
        mime_type = NULL;
    }
    if (filedata)
        file_free(filedata);
}


/**
 * Search for the end of the HTTP header
 * 
 * "Newlines" in HTTP can be \r\n (carriage return followed by newline) or \n
 * (newline) or \r (carriage return).
 */
char *find_start_of_body(char *header) {
    // strategy: find two newlines in a row, so
    // either \r\n\r\n, \r\r, or \n\n

    // assume consistent newline endings
    // const size_t BUF_SIZE = 4;
    // char *newline = calloc(BUF_SIZE, sizeof(*newline));
    char *ptr = header;
    while (*ptr) {
        char c = *ptr;
        // check for \r\r or \n\n
        if (c == '\r' || c == '\n') {
            char next_c = *(++ptr);
            if (next_c == c) {
                // means the next
                // character is the
                // start of the body
                ++ptr;
                break;
            }
        }
        // check for \r\n\r\n
        else if (c == '\r') {
            char next_c = *(++ptr);
            if (next_c == '\n') {
                ++ptr;
                c = *ptr;
                ++ptr;
                next_c = *ptr;
                if (c == '\r' && next_c == '\n') {
                    ++ptr;
                    break;
                }
            }
        }
        ++ptr;
    }

    return ptr;
}


/**
 * Parses a query string into an array of strings
 * as key1, value1, key2, value2, etc.
 * Stores the resulting array length in elem_count.
 * query_str should be pointing at the address of the 
 * '?' char (start of the query string).
 * Caller must return freed buffer.
 */
char** parse_query_str(char *query_str, size_t *elem_count) {
    size_t arr_size = 4;
    size_t num_elements = 0;
    char **ret_val = calloc(arr_size, sizeof(*ret_val));
    char *query_str_ptr = query_str;

    if (*query_str_ptr == '?') {
        ++query_str_ptr;
    }

    size_t curr_index = 0;

    char *inner_string;
    char *token, *subtoken;
    char *saveptr1, *saveptr2;
    token = strtok_r(query_str_ptr, "&", &saveptr1);
    while (token != NULL) {        
        curr_index = num_elements;
        // check to make sure we have enough space in the buffer
        // (+ 1 because we will add two elements)
        if (arr_size < num_elements + 1) {
            size_t old_size = arr_size;
            arr_size *= 2;
            ret_val = strarray_realloc(ret_val, arr_size, old_size, num_elements);
        }

        // set the key
        inner_string = token;
        subtoken = strtok_r(inner_string, "=", &saveptr2);
        if (subtoken == NULL) {
            fprintf(stderr, "Invalid query string key/value pair in %s\n", token);
            exit(1);
        }       

        strarray_setstring(ret_val, subtoken, curr_index);
        curr_index++;

        // set the value
        subtoken = strtok_r(NULL, "=", &saveptr2); 
        strarray_setstring(ret_val, subtoken, curr_index);

        token = strtok_r(NULL, "&", &saveptr1);
        num_elements += 2;
    } 

    if (elem_count) {
        *elem_count = num_elements;
    } 
    return ret_val;
}


Hashtable* create_ht_from_strarray(char **parsed_kv_pairs, size_t array_num_elements) {
    if (array_num_elements % 2 != 0) {
        array_num_elements--; // ignore last
    }
    size_t DEFAULT_SIZE = 32;
    Hashtable *ht = hashtable_create(DEFAULT_SIZE, NULL);
    for (size_t i = 0; i < array_num_elements - 1; i += 2) {
        hashtable_put(ht, parsed_kv_pairs[i], parsed_kv_pairs[i + 1]);
    }

    return ht;
}


Hashtable* parse_query_str_to_ht(char *query_str, size_t *num_kv_pairs) {
    size_t array_num_elements = 0;
    char **parsed_kv_pairs = parse_query_str(query_str, &array_num_elements);
    Hashtable *ht = create_ht_from_strarray(parsed_kv_pairs, array_num_elements);   
    if (num_kv_pairs){
        *num_kv_pairs = array_num_elements / 2;
    }

    // I think this line will mess shit up, as I don't think
    // that the hashtable copies data
    strarray_free(parsed_kv_pairs, array_num_elements);  
    return ht;
}


/**
 * Handle HTTP request and send response
 */
void* handle_http_request(int fd, struct cache *cache) {
    const int request_buffer_size = 65536; // 64K
    char request[request_buffer_size];

    // read request
    int bytes_recvd = recv(fd, request, request_buffer_size - 1, 0);

    if (bytes_recvd < 0) {
        perror("recv");
        return NULL;
    }

    // read the three components of the first request line
    char method [8];
    char path [65536];
    char version [10];

    sscanf(request, "%s %s %s", method, path, version);

    // if GET, handle the get endpoints
    if (strcmp(method, "GET") == 0) {
        // check if we need to parse query string
        char *qstringstart;
        if (( qstringstart = strchr(path, '?') )!= NULL) {
            char *data = calloc(strlen(qstringstart) + 1, sizeof(*data));
            qstringstart++;
            strcpy(data, qstringstart);
            resp_success(fd);
            return data;
        }
        // otherwise serve the requested file by calling get_file()
        else {
            get_file(fd, cache, path);
        }
    }

    // if POST, handle the post request
    else if (strcmp(method, "POST") == 0) {
        printf("start of body:\n");
        // find start of body
        char *start_of_body = find_start_of_body(request);
        printf("%s\n", start_of_body);
    }   
    return NULL; 
}


/**
 * Main
 * @data = any data we want to pass to other parts
 */
void* start_server(void *data) {
    void *ret_val = NULL;
    int newfd;  // listen on sock_fd, new connection on newfd
    struct sockaddr_storage their_addr; // connector's address information
    char s[INET6_ADDRSTRLEN];

    struct cache *cache = cache_create(10, 0);

    // Get a listening socket
    int listenfd = get_listener_socket(PORT);

    if (listenfd < 0) {
        fprintf(stderr, "Loopback server: fatal error getting listening socket\n");
        exit(1);
    }

    printf("Loopback server: waiting for connections on port %s...\n", PORT);

    // This is the main loop that accepts incoming connections and
    // fork()s a handler process to take care of it. The main parent
    // process then goes back to waiting for new connections.    
    while (ret_val == NULL) {
        socklen_t sin_size = sizeof their_addr;

        // Parent process will block on the accept() call until someone
        // makes a new connection:
        newfd = accept(listenfd, (struct sockaddr *)&their_addr, &sin_size);
        if (newfd == -1) {
            perror("accept");
            continue;
        }

        // Print out a message that we got the connection
        inet_ntop(their_addr.ss_family,
            get_in_addr((struct sockaddr *)&their_addr),
            s, sizeof s);
        printf("Loopback server: got connection from %s\n", s);
        
        // newfd is a new socket descriptor for the new connection.
        // listenfd is still listening for new connections.
        ret_val = handle_http_request(newfd, cache);
        close(newfd);
    }

    printf("Loopback server: quitting...\n");
    return ret_val;
}

