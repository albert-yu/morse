#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "morse.h"
#include "secrets.h"
#include "dataconv.h"

int main(int argc, char *argv[]) {  

    // list_last_n("[Gmail]/All Mail", 1);
    list_last_n("INBOX", 10);
    // morse_list_folders();
    return 0;
}

