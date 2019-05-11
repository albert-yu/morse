#ifndef DATACONV_H
#define DATACONV_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Converts a decimal string to a size_t data
 * type.
 */
size_t decimal_to_size_t(char *digits);

#ifdef __cplusplus
}
#endif


#endif