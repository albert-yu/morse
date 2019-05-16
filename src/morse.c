#include <stdio.h>
#include <stdlib.h>
#include "authenticate.h"
#include "curlyfries.h"
#include "morse.h"
#include "receive.h"


void config_gmail_client(MorseClient *client) {
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
    client->mailprovider = provider; 

    switch (provider) {
        case MailProvider_Google:
            config_gmail_client(client);
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
        if (client->bearertoken)
            free(client->bearertoken);
        if (client->user_email)
            free(client->user_email);
        if (client->curl_imap)
            curl_easy_cleanup(client->curl_imap);
        free(client);
        client = NULL;
    }
}


int morse_client_sendmail(MorseClient *client, SmtpRequest *smtp_request) {
    int status = -1;
    switch (client->mailprovider) {
        case MailProvider_Google:
            status = morse_sendmail_google(client->bearertoken, smtp_request);
            break;
        default:
            fprintf(stderr, "Provider is not supported.\n");
            break;
    }
    return status;
}


/* 
 * Executes an IMAP command and returns the response.
 */
ImapResponse* morse_client_exec_raw_imap(MorseClient *client, char *command) {
    return morse_exec_imap_stateful(client->curl_imap, command);
}

