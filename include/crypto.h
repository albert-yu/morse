#ifndef CRYPTO_H
#define CRYPTO_H

#include <stdio.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Generate random integer
 */
uint32_t gen_random_int();


/**
 * Populate a pre-allocated string buffer
 */
void populate_strbuffer(char *buffer, size_t buf_size);


char* crypto_encrypt(char *message, size_t msg_len);

void crypto_encrypt_to_file(char *message, size_t msg_len);

char* crypto_decrypt(unsigned char *ciphertext, size_t ciphertext_len, size_t decrypted_len);

char* crypto_decrypt_from_file();

#ifdef __cplusplus
}
#endif

#endif