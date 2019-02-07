#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "send.h"
#include "secrets.h"

int main(int argc, char *argv[]) {  

    char *to = GOOGLE_EMAIL;
    char *from = GOOGLE_EMAIL;
    char *subject = "Test";
    char *body = "<html><body>\r\n"
          "<h1>Heading</h1>"
          "<p>This is the inline <b>HTML</b> message of the e-mail.</p>"
          "<br />\r\n"
          "<p>It could be a lot of HTML data that would be displayed by "
          "e-mail viewers able to handle HTML.</p>"
          "</body></html>\r\n";

    int res = sendmail(from, to, NULL, NULL, 
             subject, body, "text/html",
             NULL);

    printf("Status: %d\n", res);

    return 0;
}
