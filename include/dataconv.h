#ifndef DATACONV_H
#define DATACONV_H

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

#ifdef __cplusplus
}
#endif


#endif