#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "dataconv.h"


#define ASCII_ZERO 48
#define ASCII_NINE 57


// is this a valid decimal character?
int is_decimal_char(char c) {
    return (c <= ASCII_NINE && c >= ASCII_ZERO);
}


/*
 * Converts an ASCII char to its
 * decimal value. E.g. '0' -> 0
 */
int dec_char_to_int(char c) {
    if (!is_decimal_char(c)) {
        fprintf(stderr, "%c is not a valid decimal digit.\n", c);
        // just return 0 if not a valid decimal
        // character, I guess
        return 0;
    }
    return ((int)c - ASCII_ZERO);
}


// is this a decimal string?
int is_decimal_str(const char *str) {
    while (*str) {
        if (!is_decimal_char(*str)) {
            return 0;
        }
        str++;
    }

    return 1;
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
        return total;
    }
    else if ((pow % 2) == 0) {
        return expt_iter(square(base), pow / 2, total);
    }
    else {
        return expt_iter(base, pow - 1, base * total);
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
        size_t value_to_add = (expt(BASE, j) * dec);
        value += value_to_add;
    }
    // printf("%zu\n", value);
    return value;
}


size_t count_digits(size_t num) {
    size_t BASE = 10;
    size_t num_digits = 1;

    while (num >= expt(BASE, num_digits)) {
        num_digits++;
    }
    return num_digits;
}


char* size_t_to_str(size_t num) {
    size_t length;
    length = count_digits(num);
    char *retval = calloc(length + 1, sizeof(*retval));
    sprintf(retval, "%zu", num);
    return retval;
}
