#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "morse.h"
#include "secrets.h"

int main(int argc, char *argv[]) {  

    char *to = TEST_RECIPIENTS;  // inside secrets.h

    // char *from = GOOGLE_EMAIL;
    char *subject = "Test (ignore this)";
    char *body = "<html><body>\r\n"
          "<h1>Heading</h1>"
          "<p>This is the inline <b>HTML</b> message of the e-mail.</p>"
          "<br />\r\n"
          "<p>It could be a lot of HTML data that would be displayed by "
          "e-mail viewers able to handle HTML.</p><code>foo();</code>"
          "</body></html>\r\n";

    char *bearertoken = getgooglebearertoken();

    // int res = morse_sendmail(bearertoken, to, NULL, TEST_CC_RECIPIENTS, 
    //          subject, body, "text/html",
    //          NULL);

    // printf("Status: %d\n", res);


    int res = morse_retrieve_last_n(bearertoken, 10);
    printf("Status: %d\n", res);

    if (bearertoken) {
        free(bearertoken);  
    }
    
    return 0;
}
