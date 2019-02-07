#include <pthread.h>
#include "authenticate.h"
#include "httpclient.h"
#include "crypto.h"
#include "jsmn.h"

#define GOOGLE_AUTH_URL "https://accounts.google.com/o/oauth2/v2/auth"
#define GOOGLE_EXCHANGE_URL "https://www.googleapis.com/oauth2/v4/token"
#define GOOGLE_MAIL_SCOPE "https://mail.google.com"
#define GOOGLE_TOKEN_CHECK_URL "https://www.googleapis.com/oauth2/v1/tokeninfo?access_token="
#define INITIAL_BUF_SIZE 64

/**
 * Builds a query string from an array of strings.
 * Array size must be even; otherwise, the last string
 * is disregarded.
 * Caller must free returned buffer.
 * Length is stored in str_length address
 */
char* buildquerystr(char **kv_pairs, size_t arr_size, size_t *str_length) {
    // dynamically resizing array
    size_t buf_size = INITIAL_BUF_SIZE;
    char *result = calloc(buf_size, sizeof(*result));
    size_t str_size = 0;  // track the size of the string
    
    // used for modification of buffer.
    char *result_ptr = result;

    // integer division
    size_t num_kv_pairs = arr_size / 2;

    for (size_t i = 0; i < num_kv_pairs * 2; i += 2) {
        // is this the last element? if so, do not append
        // '&' at the end
        int is_last = (num_kv_pairs * 2 - 2 == i) ? 1 : 0;

        char *key = kv_pairs[i];
        char *value = kv_pairs[i + 1];
        size_t key_len = strlen(key);
        size_t value_len = strlen(value);

        // create buffer
        // "{key}={value}"
        size_t kv_buf_size = key_len + value_len + 3;
        char key_and_val [kv_buf_size];
        size_t spaces_left = buf_size - str_size;
        size_t size_to_add;
        if (is_last)
            size_to_add = sprintf(key_and_val, "%s=%s", key, value);
        else
            size_to_add = sprintf(key_and_val, "%s=%s&", key, value);

        if (size_to_add >= spaces_left) {
            // reallocate
            // update spaces left before doubling
            spaces_left += buf_size;

            // double buffer size
            buf_size *= 2;
            char *new_buf = calloc(buf_size, sizeof(*new_buf));
            strcpy(new_buf, result);
            char *old_buf = result;
            result = new_buf;
            free(old_buf);

            // point result_ptr to result's new address
            result_ptr = result;

            // printf("result_ptr: %s\n", result_ptr);

            // go to end of string
            result_ptr += str_size;
        }

        // write key/value pair to buffer
        for (size_t j = 0; j < size_to_add; j++) {
            *result_ptr = key_and_val[j];
            ++result_ptr;
        }

        // update string size
        str_size += size_to_add;     
        // printf("result: %s\n", result);   
    }

    if (str_length) {
        *str_length = str_size;
    }

    // printf("result: %s\n", result);
    return result;
}


/**
 * Get the base URL for redirect.
 * Caller must free returned buffer.
 */
char* get_loopback_uri() {
    // construct redirect URI
    const char *port = PORT;  // in server.h
    char *redirect_uri = calloc(INITIAL_BUF_SIZE, sizeof(*redirect_uri));
    sprintf(redirect_uri, "http://127.0.0.1:%s", port);
    return redirect_uri;
}


char* build_auth_url(char *state) {
    // constants defined in secrets.h
    const char *client_id = GOOGLE_CLIENT_ID;

    // construct redirect URI
    char *redirect_uri = get_loopback_uri();

    // https://developers.google.com/identity/protocols/OAuth2InstalledApp
    const size_t NUM_AUTH_PARAMS = 5;
    size_t array_size = NUM_AUTH_PARAMS * 2;
    char **auth_url_params = calloc(array_size, sizeof(*auth_url_params));

    strarray_setstring(auth_url_params, "scope", 0);
    // strarray_setstring(auth_url_params, "email%20profile", 1);
    strarray_setstring(auth_url_params, GOOGLE_MAIL_SCOPE, 1);

    strarray_setstring(auth_url_params, "response_type", 2);
    strarray_setstring(auth_url_params, "code", 3);

    strarray_setstring(auth_url_params, "state", 4);
    strarray_setstring(auth_url_params, state, 5);

    strarray_setstring(auth_url_params, "redirect_uri", 6);
    strarray_setstring(auth_url_params, redirect_uri, 7);

    strarray_setstring(auth_url_params, "client_id", 8);
    strarray_setstring(auth_url_params, client_id, 9);

    size_t qrystrlen = 0;
    char *querystring = buildquerystr(auth_url_params, array_size, &qrystrlen);
    // printf("%s\n", querystring);
    strarray_free(auth_url_params, array_size);
    free(redirect_uri);

    // append query string to end of base URL
    size_t url_buf_size = strlen(GOOGLE_AUTH_URL) + qrystrlen + 1;
    char *url_buffer = calloc(url_buf_size, sizeof(*url_buffer));
    size_t url_length = sprintf(url_buffer, "%s?%s", GOOGLE_AUTH_URL, querystring);
    return url_buffer;
}


/*
 * Builds the URL-encoded code exchange data.
 * Caller must free returned buffer.
 */
char *build_code_exchange_data(char *code) {
    // constants defined in secrets.h
    const char *client_id = GOOGLE_CLIENT_ID;
    const char *client_secret = GOOGLE_CLIENT_SECRET;
    const char *grant_type = "authorization_code";

    // the redirect_uri should point to
    // localhost:<port>
    char *local_server_url = get_loopback_uri();
    char redirect_uri [32];
    sprintf(redirect_uri, "%s", local_server_url);
    free(local_server_url);

    const size_t NUM_AUTH_PARAMS = 5;
    size_t array_size = NUM_AUTH_PARAMS * 2;
    char **auth_url_params = calloc(array_size, sizeof(*auth_url_params));

    strarray_setstring(auth_url_params, "code", 0);
    strarray_setstring(auth_url_params, code, 1);

    strarray_setstring(auth_url_params, "client_id", 2);
    strarray_setstring(auth_url_params, client_id, 3);

    strarray_setstring(auth_url_params, "client_secret", 4);
    strarray_setstring(auth_url_params, client_secret, 5);

    strarray_setstring(auth_url_params, "redirect_uri", 6);
    strarray_setstring(auth_url_params, redirect_uri, 7);

    strarray_setstring(auth_url_params, "grant_type", 8);
    strarray_setstring(auth_url_params, grant_type, 9);

    size_t qrystrlen = 0;
    char *querystring = buildquerystr(auth_url_params, array_size, &qrystrlen);
    strarray_free(auth_url_params, array_size);
    return querystring;
}


/**
 * Struct to hold the entire JSON
 */
typedef struct memory_struct_t {
    char *memory;
    size_t size;
} MemoryStruct;


void memory_struct_init(MemoryStruct *chunk) {
    chunk->memory = malloc(1);  /* will be grown as needed by the realloc*/ 
    chunk->size = 0;    /* no data at this point */ 
}


/**
 * Callback function for handling code exchange response
 * signature: size_t write_data(void *buffer, size_t size, size_t nmemb, void *userp);
 */
size_t code_exchange_callback(void *buffer, size_t size, size_t nmemb, void *userp)
{
    size_t realsize = size * nmemb;

    MemoryStruct *mem = (MemoryStruct *)userp;

    char *ptr = realloc(mem->memory, mem->size + realsize + 1);
    if (ptr == NULL) {
        /* out of memory! */ 
        fprintf(stderr, "Not enough memory (realloc returned NULL).\n");
        exit(1);
    }

    mem->memory = ptr;
    memcpy(&(mem->memory[mem->size]), buffer, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;

    return realsize;
}


/*
 * Gets a fresh set of credentials from the server
 * and writes it to the file system (encrypted).
 * Caller must free returned buffer.
 */
char* getfreshcredentials(size_t *tokenlength) {
    char *querystring = NULL; // this will contain the query string
    void *dummy = NULL; // not used

    const size_t STATE_SIZE = 16;
    char state [STATE_SIZE];  // for verification
    uint32_t rand_int = gen_random_int();
    sprintf(state, "%x", rand_int);

    // build URL
    char *auth_url = build_auth_url(state);

    // send request to browser
    // and start loopback
    printf("Waiting for user response...\n");
    browser_openurl(auth_url);
    free(auth_url);

    // possible race condition starting
    // the server AFTER launching
    // the browser, but...only if the user
    // is like, impossibly fast
    querystring = start_server(dummy);

    size_t elem_count = 0; /* count of parsed_strarray elements */
    char **parsed_strarray; /* query string tokenized */
    Hashtable *ht; /* query string parsed key-value pairs */
    char *auth_code = NULL;  /* pointer to authorization cde */  

    int has_parse_error = 0;  /* did we get an error parsing the query string? */
    if (querystring) {
        // first get the string array
        parsed_strarray = parse_query_str(querystring, &elem_count);

        // parse string array into hashtable
        ht = create_ht_from_strarray(parsed_strarray, elem_count);

        char *recv_state = hashtable_get(ht, "state");
        if (recv_state == NULL) {
            fprintf(stderr, "Error: received state was NULL.\n");
            has_parse_error = 1;
            goto gc_querystring;
        }
        if (strcmp(recv_state, state) != 0) {
            fprintf(stderr, "Error: received state [%s] does not match initial state [%s].\n", recv_state, state);
            has_parse_error = 1;
            goto gc_querystring;
        }
        printf("Received state [%s] OK.\n", recv_state);

        auth_code = hashtable_get(ht, "code");  // should return NULL if not exists
        if (!auth_code) {
            fprintf(stderr, "Error parsing query string.\n");
            has_parse_error = 1;
            goto gc_querystring;
        }
        printf("Received authorization code [%s].\n", auth_code);

        gc_querystring:
            free(querystring);
            if (has_parse_error) {
                fprintf(stderr, "Exiting...\n");
                exit(1);
            }
    }
    else {
        fprintf(stderr, "Error occured getting query string.\n");
        exit(1);
    }

    printf("Exchanging code for bearer token...\n");

    // exchange authorization code for bearer token
    char *exchange_url = GOOGLE_EXCHANGE_URL;
    char *content_type = "application/x-www-form-urlencoded";
    char *post_data = build_code_exchange_data(auth_code);

    // printf("post data %s\n", post_data);

    MemoryStruct mem;
    memory_struct_init(&mem);

    http_post_no_auth(exchange_url, content_type, post_data, &code_exchange_callback, (void*)&mem);

    printf("%lu bytes retrieved.\n", (unsigned long)mem.size);

    // write to file encrypted
    crypto_encrypt_to_file(mem.memory, mem.size);

    // copy to buffer to return
    char *ret_buffer = calloc(mem.size + 1, sizeof(*ret_buffer));
    memcpy(ret_buffer, mem.memory, mem.size);

    if (tokenlength) {
        *tokenlength = mem.size;
    }

    // gc
    free(mem.memory);
    free(post_data);
    strarray_free(parsed_strarray, elem_count);
    if (ht != NULL) {
        hashtable_destroy(ht);
    }    

    return ret_buffer;
}


static int jsoneq(const char *json, jsmntok_t *tok, const char *s) {
    if (tok->type == JSMN_STRING && (int) strlen(s) == tok->end - tok->start &&
            strncmp(json + tok->start, s, tok->end - tok->start) == 0) {
        return 0;
    }
    return -1;
}


char* json_get(const char *json_string, char *key) {
    /*
     * We expect the JSON to look something like:
        {
          "access_token":"1/fFAGRNJru1FTz70BzhT3Zg",
          "expires_in":3920,
          "token_type":"Bearer",
          "refresh_token":"1/xEoDL4iW3cxlI7yDbSRFYNG01kVKM2C-259HOF2aQbI"
        }
     */

    char *value = NULL;
    int i;
    int r;
    jsmn_parser p;
    jsmntok_t t[256]; /* We expect no more than this many tokens */

    jsmn_init(&p);
    r = jsmn_parse(&p, json_string, strlen(json_string), t, sizeof(t)/sizeof(t[0]));
    if (r < 0) {
        printf("Failed to parse JSON:\n");
        switch (r) {
            case JSMN_ERROR_INVAL:
                printf("\tBad token--JSON string is corrupted.\n");
                break;
            case JSMN_ERROR_NOMEM:
                printf("\tNot enough memory provided for the tokens.\n");
                break;
            case JSMN_ERROR_PART:
                printf("\tJSON string too short. Expected more JSON data.\n");
            default:
                printf("\tUnknown error. Error code: %d\n", r);
                break;

        }
        return NULL;
    }

    /* Assume the top-level element is an object */
    if (r < 1 || t[0].type != JSMN_OBJECT) {
        printf("Object expected.\n");
        return NULL;
    }

    /* Loop over all keys of the root object */
    for (i = 1; i < r; i++) {
        if (jsoneq(json_string, &t[i], key) == 0) {
            jsmntok_t token_value = t[i + 1];
            /* We may use strndup() to fetch string value */
            int width = token_value.end - token_value.start;
            // printf("Value: %.*s\n", width, json_string + token_value.start);
            value = strndup(json_string + token_value.start, width);
            break;  
        } 
    }

    return value;
}


/**
 * Calls the token validation endpoint for Google
 */
int isvalidtoken(char *token) {
    if (!token) {
        return 0;
    }
    size_t urlsize = strlen(GOOGLE_TOKEN_CHECK_URL) + strlen(token);
    char *endpoint = calloc(urlsize + 1, sizeof(*endpoint));
    if (!endpoint) {
        fprintf(stderr, "Failed to allocate memory.\n");
        return 0;
    }
    sprintf(endpoint, "%s%s", GOOGLE_TOKEN_CHECK_URL, token);

    MemoryStruct mem;
    memory_struct_init(&mem);
    
    // call HTTP GET
    http_get(endpoint, NULL, &code_exchange_callback, (void*)&mem);
    free(endpoint);

    printf("%lu bytes retrieved.\n", (unsigned long)mem.size);

    char *zero_terminated_buffer = malloc(mem.size + 1 * sizeof(*zero_terminated_buffer));
    memcpy(zero_terminated_buffer, mem.memory, mem.size);
    zero_terminated_buffer[mem.size] = '\0';
    printf("%s\n", zero_terminated_buffer);
    // parse the JSON
    int isvalid = 0;
    char *error = json_get(zero_terminated_buffer, "error");
    if (error) {
        // isvalid is false, so leave alone
        free(error);
    }

    // a valid response should have a "issued_to"
    char *issued_to = json_get(zero_terminated_buffer, "issued_to");
    if (issued_to) {
        isvalid = 1;
        free(issued_to);
    }
    free(mem.memory);
    free(zero_terminated_buffer);
    return isvalid;
}


/**
 * Tries to read bearer token from encrypted file. If failed, then
 * gets it from the server.
 */
char* getgooglebearertoken() {
    size_t creds_len = 0;

    // first attempt to get it from the file
    char *saved_creds = crypto_decrypt_from_file();
    // printf("decrypted : %s\n", saved_creds);

    // if failed, then get new credentials
    if (saved_creds == NULL) {
        saved_creds = getfreshcredentials(&creds_len);
    }

    // if still null, return NULL
    if (saved_creds == NULL) {
        return NULL;
    }

    // get the bearer token
    char *bearer_token = json_get(saved_creds, "access_token");

    int token_is_valid = isvalidtoken(bearer_token);

    // probably expired or just bad data
    if (!token_is_valid) {
        saved_creds = getfreshcredentials(&creds_len);
    }

    bearer_token = json_get(saved_creds, "access_token");

    free(saved_creds); // saved_creds cannot be NULL
    return bearer_token;
}


