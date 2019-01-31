// #include <cmark-gfm.h>
#include <stdio.h>
#include <curl/curl.h>
#include "file.h"
#include "mime.h"
#include "send.h"
#include "authenticate.h"


/**
 * Check if file exists on disk
 * https://stackoverflow.com/a/230068/9555588
 */
int fileexists(char *filename) {
    if (access(filename, F_OK) != -1) {
        return 1;
    } 
    // file doesn't exist otherwise
    return 0;
}


/**
 * Returns a nulled-out character buffer. Must be freed
 * by caller.
 */
char* getcharbuf(size_t buf_size) {
    char *buf = (char*)calloc(buf_size, sizeof(*buf));
    if (buf) {
        return buf;
    } else {
        fprintf(stderr, "Unable to allocate buffer for string.\n");
        exit(EXIT_FAILURE);
    }

}


/**
 * Returns an empty string that needs to be freed.
 */
char* getemptystr() {
    char *empty_str = "\0";
    size_t size = 2;
    char *buf_empty = getcharbuf(size);
    strcpy(buf_empty, empty_str);
    return buf_empty;
}
 

/**
 * Is this string syntatically a valid extension? 
 */
int isvalidext(char *extension, size_t len) {
    // named falsy instead of false in case stdbool is used later
    int falsy = 0;  
    if (len == 0) {
        return falsy;
    }
    if (len > MAX_FILE_EXT_CHAR_COUNT) {
        // we will just consider it invalid
        // if it is not a file type for sending email
        return falsy;
    }
    char *ptr = extension;
    char c;
    while ((c = *ptr) != 0) {
        if (!isalpha(c)) {
            return falsy;
        }
        ++ptr;
    }

    return 1; // true
}


int sendmail(char *to, char *cc, char *bcc, 
             char *subject, 
             char *body, char *mimetype) {

    
    return 0;
}

#define FROM    "GOOGLE_EMAIL_W_BRACKETS"
#define TO      "GOOGLE_EMAIL_W_BRACKETS"
#define CC      ""

static const char *headers_text[] = {
  "Date: Wed, 30 Jan 2019 17:02:43 +0100",
  "To: " TO,
  "From: " FROM " (Example User)",
  // "Cc: " CC " (Another example User)",
  "Message-ID: <dcd7cb38-11db-487a-9f3a-e652a9458efd@"
    "rfcpedant.example.org>",
  "Subject: example sending a MIME-formatted message",
  NULL
};

static const char inline_text[] =
  "This is the inline text message of the e-mail.\r\n"
  "\r\n"
  "  It could be a lot of lines that would be displayed in an e-mail\r\n"
  "viewer that is not able to handle HTML.\r\n";

static const char inline_html[] =
  "<html><body>\r\n"
  "<p>This is the inline <b>HTML</b> message of the e-mail.</p>"
  "<br />\r\n"
  "<p>It could be a lot of HTML data that would be displayed by "
  "e-mail viewers able to handle HTML.</p>"
  "</body></html>\r\n";


int test_smtp(void) {
    CURL *curl;
    CURLcode res = CURLE_OK;
    struct curl_slist *headers = NULL;
    struct curl_slist *recipients = NULL;
    struct curl_slist *slist = NULL;
    curl_mime *mime;
    curl_mime *alt;
    curl_mimepart *part;
    const char **cpp;

    printf("Getting the bearer token...\n");
    char *bearer_token = getbearertoken();
    printf("token: %s\n", bearer_token);

    if (!bearer_token) {
        fprintf(stderr, "Failed to get bearer token.\n");
        exit(1);
    }

    curl = curl_easy_init();
    if (curl) {
        printf("curl init OK\n");       
        /* This is the URL for your mailserver */
        curl_easy_setopt(curl, CURLOPT_URL, "smtps://smtp.gmail.com:465");

        /* Note that this option isn't strictly required, omitting it will result
         * in libcurl sending the MAIL FROM command with empty sender data. All
         * autoresponses should have an empty reverse-path, and should be directed
         * to the address in the reverse-path which triggered them. Otherwise,
         * they could cause an endless loop. See RFC 5321 Section 4.5.5 for more
         * details.
         */
        curl_easy_setopt(curl, CURLOPT_MAIL_FROM, FROM);

        // use SSL
        curl_easy_setopt(curl, CURLOPT_USE_SSL, CURLUSESSL_ALL);

        // set a timeout
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 60L);

        // verbose output
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

        /* Add two recipients, in this particular case they correspond to the
         * To: and Cc: addressees in the header, but they could be any kind of
         * recipient. */
        recipients = curl_slist_append(recipients, TO);
        recipients = curl_slist_append(recipients, CC);
        curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);

        /* Build and set the message header list. */
        for(cpp = headers_text; *cpp; cpp++)
            headers = curl_slist_append(headers, *cpp);

        // Add authorization
        // size_t token_len = strlen(bearer_token);
        // size_t buffer_size_auth_header = 50 + token_len;
        // char auth_header [buffer_size_auth_header];
        // sprintf(auth_header, "Authorization: Bearer %s", bearer_token);
        // printf("%s\n", auth_header);
        // headers = curl_slist_append(headers, auth_header);

        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        curl_easy_setopt(curl, CURLOPT_XOAUTH2_BEARER, bearer_token);
        curl_easy_setopt(curl, CURLOPT_SASL_IR, 1L);
        curl_easy_setopt(curl, CURLOPT_USERNAME, GOOGLE_EMAIL);

        /* Build the mime message. */
        mime = curl_mime_init(curl);

        /* The inline part is an alternative proposing the html and the text
           versions of the e-mail. */
        alt = curl_mime_init(curl);

        /* HTML message. */
        part = curl_mime_addpart(alt);
        curl_mime_data(part, inline_html, CURL_ZERO_TERMINATED);
        curl_mime_type(part, "text/html");

        /* Text message. */
        part = curl_mime_addpart(alt);
        curl_mime_data(part, inline_text, CURL_ZERO_TERMINATED);

        /* Create the inline part. */
        part = curl_mime_addpart(mime);
        curl_mime_subparts(part, alt);
        curl_mime_type(part, "multipart/alternative");
        slist = curl_slist_append(NULL, "Content-Disposition: inline");
        curl_mime_headers(part, slist, 1);

        /* Add the README file as an attachment. */
        part = curl_mime_addpart(mime);
        curl_mime_filedata(part, "README.md");
        curl_easy_setopt(curl, CURLOPT_MIMEPOST, mime);

        printf("Sending message....\n");
        /* Send the message */
        res = curl_easy_perform(curl);

        /* Check for errors */
        if(res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n",
                  curl_easy_strerror(res));
        }
            
        /* Free lists. */
        curl_slist_free_all(recipients);
        curl_slist_free_all(headers);

        /* curl won't send the QUIT command until you call cleanup, so you should
         * be able to re-use this connection for additional messages (setting
         * CURLOPT_MAIL_FROM and CURLOPT_MAIL_RCPT as required, and calling
         * curl_easy_perform() again. It may not be a good idea to keep the
         * connection open for a very long time though (more than a few minutes
         * may result in the server timing out the connection), and you do want to
         * clean up in the end.
         */
        curl_easy_cleanup(curl);

        /* Free multipart message. */
        curl_mime_free(mime);
    }
    free(bearer_token);

    return (int)res;
}


