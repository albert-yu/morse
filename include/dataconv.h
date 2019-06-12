#ifndef DATACONV_H
#define DATACONV_H

#include <stdlib.h>


#define NUM_BYTES_IN_UNSIGNED_LONG 4
#define UNSIGNED_LONG_HEX_LEN (NUM_BYTES_IN_UNSIGNED_LONG * 2)


#ifdef __cplusplus
extern "C" {
#endif


/*
 * Converts an ASCII char to its
 * decimal value. E.g. '0' -> 0
 */
int dec_char_to_int(char c); 


/* 
 * Is this a decimal string?
 */
int is_decimal_str(const char *str); 


/*
 * Squares a number
 */
size_t square(size_t num); 


/*
 * Exponentiation
 */
size_t expt(size_t base, size_t pow);


/*
 * Converts a decimal string to a size_t data
 * type.
 */
size_t decimal_to_size_t(char *digits);


/*
 * Converts a size_t to a NULL-terminated string.
 * Returns NULL if failed.
 */
char* size_t_to_str(size_t num);


/*
 * Converts a size_t to a NULL-terminated string,
 * but with padding zeroes.
 * For example, converting 15 with max_digits 4 gives
 * "0015".
 * Returns NULL if max_digits < num's number of digits.
 */
char* size_t_to_str_padded(size_t num, size_t max_digits);


/*
 * Is this character a hex char?
 */
int is_hex(char c);


/*
 * Convert bytes to hex string by parsing two chars at a time.
 * Need to free resulting pointer's memory.
 */
char* bytes_to_hex(unsigned char *bytes, size_t bytes_len);


/*
 * Convert hex string to bytes by parsing two chars at a time
 * Need to free resulting pointer's memory.
 * Adapted from https://stackoverflow.com/a/46210658
 */
unsigned char* hex_to_bytes_with_len(char *hex_string, size_t hex_length);

unsigned char* hex_to_bytes(char *hex_string);


/**
 * Convert unsigned long to null-terminated byte array.
 * Caller must free returned buffer.
 * Adapted from: https://stackoverflow.com/a/3784478/9555588
 */
unsigned char* unsigned_long_to_bytes(unsigned long n);


/**
 * Convert bytes to unsigned long
 */
unsigned long bytes_to_unsigned_long(unsigned char *bytes);


/**
 * Convert hex string to unsigned long. 
 * Will truncate anything longer than 8 chars.
 */
unsigned long hex_bytes_to_unsigned_long(char *hex_bytes);

#ifdef __cplusplus
}
#endif


#endif
