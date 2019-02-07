#include <stdlib.h>
#include <string.h>
// #include <cmark-gfm.h>

#include "authenticate.h"
#include "send.h"


int main(int argc, char *argv[]) {  
    // size_t n = 0;
    char *token = getgooglebearertoken();
    if (token) {
        printf("token: %s\n", token);
        int isvalid = isvalidtoken(token);
        fputs(isvalid ? "true\n" : "false\n", stdout);
        free(token);
        token = NULL;
    }

    // test_smtp();
    // char *messageid = generate_messageid();
    // if (messageid) {
    //     printf("message ID: %s\n", messageid);
    //     free(messageid);
    // }


    return 0;
}
