#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "dataconv.h"


/*
 * Converts an ASCII char to its
 * decimal value. E.g. '0' -> 0
 */
int dec_char_to_int(char c) {
    int result;
    int ZERO = 48;
    int NINE = 57;
    if (c > NINE || c < ZERO) {
        // just return 0 if not a valid decimal
        // character, I guess
        return 0;
    }

    return ((int)c - ZERO);
}


/*
 * Squares a number
 */
size_t square(size_t num) {
    return (num * num);
}


/*
 * The compiler should be able to transform this into 
 * a loop if optimizations are turned on.
 */
size_t expt_iter(size_t base, size_t pow, size_t total) {
    if (pow == 0) {
        return (total * 1);
    }
    else if ((pow % 2) == 0) {
        return expt_iter(base, pow / 2, total * square(base));
    }
    else {
        return expt_iter(base, pow - 1, total * base);
    }
}


size_t expt(size_t base, size_t pow) {
    return expt_iter(base, pow, 1);
}


size_t decimal_to_size_t(char *digits) {
    size_t len = strlen(digits);
    size_t value = 0;

    size_t BASE = 10;
    size_t i, j;
    char c;
    for (i = len, j = 0; i > 0; i--, j++) {
        c = digits[i - 1];
        int dec = dec_char_to_int(c);
        // printf("%zu, %zu\n", i, j); 
        value += (expt(BASE, j) * dec);
    }
    return value;
}
