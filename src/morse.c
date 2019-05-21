#include <stdio.h>
#include <stdlib.h>
#include "authenticate.h"
#include "curlyfries.h"
#include "morse.h"
#include "receive.h"


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
    }
    return status;
}


/* 
 * Executes an IMAP command and returns the response.
 */
ImapResponse* morse_client_exec_raw_imap(MorseClient *client, char *command) {
    ImapResponse *resp = NULL;
    int validation = validate_client(client);
    if (validation == 0) {
        resp =  morse_exec_imap_stateful(client->curl_imap, command);
    }
    return resp;
}


Mailbox* morse_client_get_mailboxes(MorseClient *client) {
    if (validate_client(client) == 0) {
        return get_mailboxes(client->curl_imap);
    }
    return NULL;
}


ImapResponse* morse_client_select_box(MorseClient *client, char *box_name) {
    ImapResponse *resp = NULL;
    if (validate_client(client) == 0) {
        resp = select_box(client->curl_imap, box_name);
    }
    return resp;
}


int morse_client_begin_idle(MorseClient *client) {
    int res = 0;
    if (validate_client(client) == 0) {
        res = begin_idle(client->curl_imap);
    }
    return res;
}

