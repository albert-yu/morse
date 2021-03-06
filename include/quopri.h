#ifndef QUOPRI_H
#define QUOPRI_H


/* 
 * Module for dealing with quoted printable data
 * translated from the CPython source at:
 * https://github.com/python/cpython/blob/3.8/Lib/quopri.py
 */

#ifdef __cplusplus
extern "C" {
#endif


/*
 * Read 'input', apply quoted-printable decoding, and 
 * return the output.
 * If 'header' is true, decode underscore as space (per RFC 1522).
 */
char* quopri_decode(const char *input, int header);

#ifdef __cplusplus
}
#endif

#endif

