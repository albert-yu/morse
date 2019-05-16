#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "morse.h"


int main(int argc, char *argv[]) {  

    // list_last_n("[Gmail]/All Mail", 1);
    // list_last_n("INBOX", 10);
    // morse_list_folders();
    MorseClient *client = morse_client_login(MailProvider_Google);
    char *email = NULL;
    if (argc == 2) {
        email = argv[1];
    }
    else {
        email = "test@foo.com";
    }
    if (client) {
        SmtpRequest request;

        request.to = email;
        request.cc = NULL;
        request.bcc = NULL;
        request.subject = "Test email";
        request.body = "This is a test email.";
        request.mimetype = "text/plain";
        request.attachments = NULL;
        morse_client_sendmail(client, &request);
        morse_client_logout(client);
    }
    return 0;
}

