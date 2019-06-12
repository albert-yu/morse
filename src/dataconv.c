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
    char *retval = NULL;
    length = count_digits(num);
    retval = calloc(length + 1, sizeof(*retval));
    if (retval)
        sprintf(retval, "%zu", num);
    return retval;
}


/*
 * Computes the number of base 10 digits in the
 * number and fills in the string buffer
 * with zeroes until it reaches the max number 
 * of digits.
 * Allocated buffer should be large enough for
 * padding and number.
 */
char* size_t_to_str_padded(size_t num, size_t max_digits) {
    size_t num_digits = count_digits(num);
    char *retval = NULL;
    
    // size_t is unsigned, so we cannot
    // subtract and check for negativity
    if (num_digits > max_digits) {
        return NULL;
    }
    char *as_str = size_t_to_str(num);
    retval = calloc(max_digits + 1, sizeof(*retval));
    if (as_str && retval) {
        // add the padding
        char zero = '0';
        size_t num_zeros = max_digits - num_digits; 
        for (size_t i = 0; i < num_zeros; i++) {
            retval[i] = zero;
        }
        strcat(retval, as_str);
    }
    if (as_str) {
        free(as_str);
    }
    
    return retval;
}


/* ------HEX STUFF--------------*/

/*
 * Is this a hex character?
 */
int is_hex(char c) {
    int is_hex_alpha = (c <= 'f' && c >= 'a') || (c <= 'F' && c >= 'A');
    return is_decimal_char(c) || is_hex_alpha; 
}


/*
 * Convert bytes to hex string by parsing two chars at a time.
 * Need to free resulting pointer's memory.
 * Adapted from https://stackoverflow.com/a/46210658
 */
char* bytes_to_hex(unsigned char *bytes, size_t bytes_len) {
    char *hex = calloc(bytes_len * 2, sizeof(*hex));

    for (size_t i = 0, j = 0; i < bytes_len; ++i, j += 2) {
        sprintf(hex + j, "%02x", bytes[i] & 0xff);
    }
    return hex;
}


/*
 * Convert hex string to bytes by parsing two chars at a time
 * Need to free resulting pointer's memory.
 * Adapted from https://stackoverflow.com/a/46210658
 */
unsigned char* hex_to_bytes_with_len(char *hex_string, size_t hex_length) {
    // not handling the odd case--yet
    if ((hex_length % 2) != 0) {
        fprintf(stderr, "Passed in string must have even length."
                        " Got length %zu instead.\n", hex_length);
        return NULL;
    }

    size_t ascii_length = hex_length / 2;
    // printf("%zu\n", ascii_length);
    unsigned char *byte_array = calloc(ascii_length + 1, sizeof(*byte_array));
    size_t i = 0;
    size_t j = 0;
    // printf("foo!\n");
    for (; j < hex_length; ++i, j += 2) {
        int val [1];
        sscanf(hex_string + j, "%2x", val);
        byte_array[i] = val[0];
        byte_array[i + 1] = '\0';
    }
    return byte_array;
}


unsigned char* hex_to_bytes(char *hex_string) {
    size_t hex_length = strlen(hex_string);
    return hex_to_bytes_with_len(hex_string, hex_length);
}


/**
 * Convert unsigned long to null-terminated byte array.
 * Caller must free returned buffer.
 * Adapted from: https://stackoverflow.com/a/3784478/9555588
 */
unsigned char* unsigned_long_to_bytes(unsigned long n) {
    int size = NUM_BYTES_IN_UNSIGNED_LONG + 1; // 4 + 1
    unsigned char *bytes = calloc(size, sizeof(*bytes));
    bytes[0] = (n >> 24) & 0xff;
    bytes[1] = (n >> 16) & 0xff;
    bytes[2] = (n >> 8) & 0xff;
    bytes[3] = n & 0xFF;
    return bytes;
}


/**
 * Convert bytes to unsigned long
 */
unsigned long bytes_to_unsigned_long(unsigned char *bytes) {
    unsigned long value = 0;
    value += (bytes[0] << 24) & 0xffff;
    value += (bytes[1] << 16) & 0xffff;
    value += (bytes[2] << 8) & 0xffff;
    value += (bytes[3]) & 0xffff;
    return value;
}


/**
 * Convert hex string to unsigned long. Will truncate anything longer than 8 chars.
 */
unsigned long hex_bytes_to_unsigned_long(char *hex_bytes) {
    unsigned char *bytes = hex_to_bytes_with_len(hex_bytes, UNSIGNED_LONG_HEX_LEN);
    return bytes_to_unsigned_long(bytes);
}
 
