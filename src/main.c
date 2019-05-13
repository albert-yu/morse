#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "morse.h"
#include "secrets.h"
#include "dataconv.h"

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

    list_last_n("[Gmail]/All Mail", 100);
    // morse_list_folders();
    return 0;
}

