#include <string.h>
#include "crypto.h"
#include "dataconv.h"
#include "secrets.h"
#include "file.h"

// libsodium.a
#include "sodium.h"

#define INT_UPPER_BOUND UINT32_MAX
#define CRYPTO_FILEPATH "./temp"
#define CRYPTO_NONCE_FILENAME "state.txt"
#define CRYPTO_SECRET_FILENAME "creds.txt"
#define CRYPTO_DECRYPT_LEN_FILENAME "len.txt"
#define CRYPTO_FILEPATH_SIZE 32



uint32_t gen_random_int() {
    uint32_t ret_val;
    if (sodium_init() < 0) {
        /* panic! the library couldn't be initialized, it is not safe to use */
        fprintf(stderr, "Could not initialize sodium! Exiting...\n");
        exit(1); 
    }

    ret_val = randombytes_uniform(INT_UPPER_BOUND);
    return ret_val;
}


/*
 * Gets the temporary file location for the nonce.
 */
void get_nonce_fileloc(char *buffer) {
    sprintf(buffer, "%s/%s", CRYPTO_FILEPATH, CRYPTO_NONCE_FILENAME);
}


/*
 * Gets the temporary file location for the encrypted secret
 */
void get_secret_fileloc(char *buffer) {
    sprintf(buffer, "%s/%s", CRYPTO_FILEPATH, CRYPTO_SECRET_FILENAME);
}


/*
 * Removes the temporary files from the disk
 */
void rm_temp_files() {
    char buf [CRYPTO_FILEPATH_SIZE];
    memset(buf, 0, CRYPTO_FILEPATH_SIZE);

    // remove file with JSON
    get_secret_fileloc(buf);
    remove(buf);

    // remove file with nonce
    memset(buf, 0, CRYPTO_FILEPATH_SIZE);
    get_nonce_fileloc(buf);
    remove(buf);
}


void populate_strbuffer(char *buffer, size_t buf_size) {
    /* buffer will not be null-terminated */        
    randombytes_buf(buffer, buf_size);
}


unsigned char* generate_random_key() {
    unsigned char *key = 
        calloc(crypto_secretbox_KEYBYTES + 1, sizeof(*key));
    crypto_secretbox_keygen(key);
    return key;
}


/**
 * Writes the nonce to a file. Overwrites any existing contents
 */
void set_nonce(unsigned char *nonce) {
    // make printable with hex chars
    char *hex_str = bytes_to_hex(nonce, crypto_secretbox_NONCEBYTES);

    // write to file
    file_write(hex_str, CRYPTO_FILEPATH, CRYPTO_NONCE_FILENAME);

    free(hex_str);
}


size_t byteslen(unsigned char *bytes) {
    size_t ret_val = 0;
    unsigned char *ptr = bytes;
    while (*ptr) {
        ret_val++;
        ptr++;
    }

    return ret_val;
}


/**
 * Gets the nonce from a file and populates the
 * specified buffer.
 * Returns 0 if success.
 */
int get_nonce(unsigned char *nonce) {
    char nonce_file [CRYPTO_FILEPATH_SIZE];
    get_nonce_fileloc(nonce_file);

    FileData *filestruct = file_load(nonce_file);
    if (filestruct == NULL) {
        printf("File does not exist.\n");
        return 1;
    }

    // file should be stored as hex string
    // bytes should be null-terminated
    unsigned char *bytes = hex_to_bytes_with_len(filestruct->data, filestruct->size);

    // copy result to buffer
    memcpy(nonce, bytes, crypto_secretbox_NONCEBYTES);

    // gc
    free(bytes);
    bytes = NULL;
    file_free(filestruct);

    return 0;
}


/**
 * Generates a new nonce and writes it to a file if 
 * caller specifies
 */
void generate_nonce(unsigned char *nonce, int write_to_file) {
    randombytes_buf(nonce, sizeof nonce);
    if (write_to_file) {
        set_nonce(nonce);
    }
}


/**
 * Populates the buffer with the secret key
 */
void init_secret_key(unsigned char *key_buf) {
    // get secret key and copy to buffer
    char *hex_key = SECRET_KEY; /* from SECRETS_H */
    unsigned char *key_read = hex_to_bytes(hex_key);
    memcpy(key_buf, key_read, crypto_secretbox_KEYBYTES);
    free(key_read);
}


char* crypto_encrypt(char *message, size_t msg_len) {
    const unsigned char *bytes = (const unsigned char *)message;
    size_t CIPHERTEXT_LEN = crypto_secretbox_MACBYTES + msg_len;
    unsigned char key [crypto_secretbox_KEYBYTES];
    unsigned char nonce [crypto_secretbox_NONCEBYTES];
    unsigned char ciphertext [CIPHERTEXT_LEN];

    // get secret key and copy to buffer
    init_secret_key(key);

    // generate nonce and write it to a file
    generate_nonce(nonce, 1);

    // encrypt!
    crypto_secretbox_easy(ciphertext, bytes, msg_len, nonce, key);

    char *returned_buf = calloc(CIPHERTEXT_LEN + 1, sizeof(*returned_buf));
    memcpy(returned_buf, ciphertext, CIPHERTEXT_LEN);

    return returned_buf;
}


void crypto_encrypt_to_file(char *message, size_t msg_len) {
    char *encrypted = crypto_encrypt(message, msg_len);
    size_t CIPHERTEXT_LEN = crypto_secretbox_MACBYTES + msg_len;

    // make printable with hex chars
    char *hex_str = bytes_to_hex((unsigned char*)encrypted, CIPHERTEXT_LEN);

    // encode msg_len (unsigned long) into first four bytes 
    unsigned char *length = unsigned_long_to_bytes(msg_len);
    char *len_as_hex = bytes_to_hex(length, NUM_BYTES_IN_UNSIGNED_LONG);

    // prepend encrypted text with decrypted length 
    // represented as hex string
    size_t size = UNSIGNED_LONG_HEX_LEN + strlen(hex_str) + 2;
    if (size < CIPHERTEXT_LEN) {
        fprintf(stderr, "Integer overflow detected. The message we are encrypting may be too large. Exiting...\n");
        exit(1);
    }

    char string_to_write [size];
    strcpy(string_to_write, len_as_hex);
    strcat(string_to_write, hex_str);

    // write to file
    file_write(string_to_write, CRYPTO_FILEPATH, CRYPTO_SECRET_FILENAME);

    // gc
    free(encrypted);
    free(length);
    free(len_as_hex);
    free(hex_str);
}


char* crypto_decrypt(unsigned char *ciphertext, size_t ciphertext_len, size_t decrypted_len) {
    unsigned char decrypted [decrypted_len];
    unsigned char nonce [crypto_secretbox_NONCEBYTES];
    unsigned char key [crypto_secretbox_KEYBYTES];

    init_secret_key(key);
    int nonce_error = get_nonce(nonce);
    if (nonce_error) {
        return NULL;
    }

    if (crypto_secretbox_open_easy(
        decrypted, ciphertext, ciphertext_len, nonce, key) != 0) {
        /* message forged! */
        return NULL;
    }

    char *ret_val = calloc(decrypted_len + 1, sizeof(*ret_val));
    memcpy(ret_val, decrypted, decrypted_len);
    return ret_val;
}


char* crypto_decrypt_from_file() {
    char filepath [CRYPTO_FILEPATH_SIZE];
    get_secret_fileloc(filepath);

    FileData *filestruct = file_load(filepath);
    if (filestruct == NULL) {
        return NULL;
    }

    char *len_and_encrypted_hex = (char*)filestruct->data;

    // exclude the length at the beginning
    char *encrypted_hex_only = len_and_encrypted_hex + UNSIGNED_LONG_HEX_LEN;
    int size_of_encrypted_hex = filestruct->size - UNSIGNED_LONG_HEX_LEN;

    if (size_of_encrypted_hex < 0) {
        printf("Invalid credentials found in file.\n");
        // invalid credentials!
        return NULL;
    }

    unsigned char *bytes = hex_to_bytes_with_len(encrypted_hex_only, size_of_encrypted_hex);
    size_t decrypted_len = hex_bytes_to_unsigned_long(len_and_encrypted_hex);
    char *decrypted = crypto_decrypt(bytes, size_of_encrypted_hex / 2, decrypted_len);

    // gc
    free(bytes);
    file_free(filestruct);

    return decrypted;
}

