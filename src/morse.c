#include <stdio.h>
#include <stdlib.h>
#include "authenticate.h"
#include "curlyfries.h"
#include "memstruct.h"
#include "morse.h"
#include "statuscodes.h"


void config_client_gmail(MorseClient *client) {
    client->bearertoken = getgooglebearertoken(); 
    if (client->bearertoken) {
        client->user_email = getgmailaddress(
            client->bearertoken);
        printf("Logged in as: %s\n", client->user_email);
        client->curl_imap = get_imap_curl_google(); 
    }
    else {
        fprintf(stderr, "Could not authenticate client for Gmail.\n");
        morse_client_logout(client);
    }
}


/*
 * Creates a mail client with the given provider and
 * logs the user in through the browser.
 */
MorseClient* morse_client_login(MailProvider provider) {
    MorseClient *client = malloc(sizeof(*client));
    if (!client) {
        fprintf(stderr, "Out of memory! Could not alloc MorseClient.\n");
        return NULL;
    }
    *client = (MorseClient) {
        .bearertoken = NULL,
        .user_email = NULL,
        .curl_imap = NULL,
        .mailprovider = provider
    };
    switch (provider) {
        case MailProvider_Google:
            config_client_gmail(client);
            break;
        default:
            fprintf(stderr, "This mail provider is not supported.\n");
            free(client);
            return NULL;
    }
    
    return client;
}


/* 
 * Logs out and garbage collects a heap-allocated MorseClient.
 */ 
void morse_client_logout(MorseClient *client) {
    if (client) {
        if (client->bearertoken) {
            free(client->bearertoken);
        }
        if (client->user_email) {
            free(client->user_email);
        }
        if (client->curl_imap) {
            curl_easy_cleanup(client->curl_imap);
        }
        free(client);
        client = NULL;
    }
}


int morse_client_sendmail(MorseClient *client, SmtpRequest *smtp_request) {
    int status = -1;
    switch (client->mailprovider) {
        case MailProvider_Google:
            status = morse_sendmail_google(
                client->bearertoken, 
                client->user_email, 
                smtp_request);
            break;
        default:
            fprintf(stderr, "Provider is not supported.\n");
            break;
    }
    return status;
}


/*
 * Is this client a valid one?
 * Tries to re-login if the curl_imap member is missing.
 */
static int validate_client(MorseClient *client) {
    int status = 0;
    if (!client) {
        fprintf(stderr, "Client is NULL!\n");
        return -1;
    }
    if (!client->curl_imap) {
        MorseClient *old_c = client;
        client = morse_client_login(client->mailprovider);
        morse_client_logout(old_c); 
        if (!client) {
            fprintf(stderr, "Could not re-login client.\n");
            return -1;
        }
    }
    return status;
}


/* 
 * Executes an IMAP command and returns the response.
 */
ImapResponse* morse_client_exec_raw_imap(MorseClient *client, const char *command, int verbose) {
    ImapResponse *resp = NULL;
    int validation = validate_client(client);
    if (validation == 0) {
        resp =  morse_exec_imap_stateful(client->curl_imap, command, verbose);
    }
    return resp;
}


Mailbox* morse_client_get_mailboxes(MorseClient *client) {
    if (validate_client(client) == 0) {
        return get_mailboxes(client->curl_imap);
    }
    return NULL;
}


ImapResponse* morse_client_select_box(MorseClient *client, const char *box_name) {
    ImapResponse *resp = NULL;
    if (validate_client(client) == 0) {
        resp = select_box(client->curl_imap, box_name);
    }
    return resp;
}


/*
 * Super ugly having the curl callback function exposed,
 * but cannot think of an alternative right now.
 */
ImapResponse* morse_client_idle_on(MorseClient *client, 
                                   curl_debug_callback idle_callback) {
    ImapResponse *resp = NULL;
    resp = imap_response_new();
    if (!resp) {
        fprintf(stderr, "Memory for ImapResponse"
                        " could not be allocated.\n");
        return NULL;
    }

    MorseStatusCode res = MorseStatus_ErrUnknown;
    CURL *curl = client->curl_imap;
    if (!curl) {
        fprintf(stderr, "CURL handle cannot be NULL.\n"); 
        res = MorseStatus_InvalidArg;
        resp->status = (int)res;
        return resp;
    }
    char *command = "IDLE";
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, DEFAULT_IMAP_TIMEOUT);
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, command);

    // set standard callback
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &curl_mem_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)resp->data);

    // IMPORTANT: the response needs to be written to the 
    // ImapReponse struct
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
    if (idle_callback) {
        curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, &idle_callback);
        curl_easy_setopt(curl, CURLOPT_DEBUGDATA, (void*)resp->v_data);
    }
    else {
        // stdout otherwise
        curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, NULL);
        curl_easy_setopt(curl, CURLOPT_DEBUGDATA, NULL);
    }
    // curl_easy_setopt(curl, CURLOPT_DEBUGDATA, (void*)resp->v_data);
    CURLcode curlres = curl_easy_perform(curl);
    if (curlres != CURLE_OK) {
        fprintf(stderr, "curl_easy_perform() failed: %s\n", 
            curl_easy_strerror(curlres));
        resp->status = (int)MorseStatus_CurlError;
    }
    return resp;
}


