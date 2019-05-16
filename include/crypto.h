#ifndef CRYPTO_H
#define CRYPTO_H

#include <stdio.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Convert bytes to hex string by parsing two chars at a time.
 * Need to free resulting pointer's memory.
 */
char* bytes_to_hex(unsigned char *bytes, size_t bytes_len);

/**
 * Generate random integer
 */
uint32_t gen_random_int(void);


/**
 * Populate a pre-allocated string buffer
 */
void populate_strbuffer(char *buffer, size_t buf_size);


/**
 * Encrypt a message of size msg_len. Caller must
 * free the returned buffer.
 */
char* crypto_encrypt(char *message, size_t msg_len);


/**
 * Encrypt a message of size msg_len and write
 * result to a pre-determined file location.
 */
void crypto_encrypt_to_file(char *message, size_t msg_len);


/**
 * Decrypt an encrypted message (ciphertext)
 */
char* crypto_decrypt(unsigned char *ciphertext, size_t ciphertext_len, size_t decrypted_len);


/**
 * Decrypt a pre-determined file's contents. Returns NULL if not found or failed.
 */
char* crypto_decrypt_from_file(void);


#ifdef __cplusplus
}
#endif

#endif