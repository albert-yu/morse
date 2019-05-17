#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "morse.h"


int main(int argc, char *argv[]) {  
    MorseClient *client = morse_client_login(MailProvider_Google);
    char *email = NULL;
    if (argc == 2) {
        email = argv[1];
    }

    if (client) {
        if (email) {
            printf("Sending to: %s\n", email);
            SmtpRequest request;
            request.to = email;
            request.cc = NULL;
            request.bcc = NULL;
            request.subject = "Test email";
            request.body = "This is a test email.";
            request.mimetype = "text/plain";
            request.attachments = NULL;
            morse_client_sendmail(client, &request);
        }
        else {
            printf("Receiving email.\n");
        }
        morse_client_logout(client);
    }
    return 0;
}

