#include <stdlib.h>
#include <string.h>
// #include <cmark-gfm.h>

#include "authenticate.h"
#include "send.h"
// #include "server.h"
// #include "httpclient.h"
// #include "crypto.h"
// #include "file.h"


int main(int argc, char *argv[]) {  
    // size_t n = 0;
    // char *token = getbearertoken();
    // if (token) {
    //     printf("token: %s\n", token);
    //     free(token);
    //     token = NULL;
    // }

    test_smtp();

    return 0;
}
