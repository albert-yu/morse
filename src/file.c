#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

#include "file.h"


#ifdef _WIN32

//define something for Windows (32-bit and 64-bit, this part is common)
// https://stackoverflow.com/a/42906151
#include <windows.h>
#define mkdir(dir, mode) _mkdir(dir)
#define open(name, ...) _open(name, __VA_ARGS__)
#define read(fd, buf, count) _read(fd, buf, count)
#define close(fd) _close(fd)
#define write(fd, buf, count) _write(fd, buf, count)
#define dup2(fd1, fd2) _dup2(fd1, fd2)
#define unlink(file) _unlink(file)
#define rmdir(dir) _rmdir(dir)
#define getpid() _getpid()
#define usleep(t) Sleep((t)/1000)
#define sleep(t) Sleep((t)*1000)

   #ifdef _WIN64
      //define something for Windows (64-bit only)
   #else
      //define something for Windows (32-bit only)
   #endif
#endif  /* _WIN32 */


/**
 * Loads a file into memory and returns a pointer to the data.
 * 
 * Buffer is not NUL-terminated.
 */
struct file_data* file_load(char *filename) {
    char *buffer, *p;
    struct stat buf;
    int bytes_read, bytes_remaining, total_bytes = 0;

    // Get the file size
    if (stat(filename, &buf) == -1) {
        return NULL;
    }

    // Make sure it's a regular file
    if (!(buf.st_mode & S_IFREG)) {
        return NULL;
    }

    // Open the file for reading
    FILE *fp = fopen(filename, "rb");

    if (fp == NULL) {
        return NULL;
    }

    // Allocate that many bytes
    bytes_remaining = buf.st_size;
    p = buffer = malloc(bytes_remaining);

    if (buffer == NULL) {
        return NULL;
    }

    // Read in the entire file
    while (bytes_read = fread(p, 1, bytes_remaining, fp), bytes_read != 0 && bytes_remaining > 0) {
        if (bytes_read == -1) {
            free(buffer);
            return NULL;
        }

        bytes_remaining -= bytes_read;
        p += bytes_read;
        total_bytes += bytes_read;
    }

    // Allocate the file data struct
    struct file_data *filedata = malloc(sizeof *filedata);

    if (filedata == NULL) {
        free(buffer);
        return NULL;
    }

    filedata->data = buffer;
    filedata->size = total_bytes;

    return filedata;
}


/**
 * Frees memory allocated by file_load().
 */
void file_free(struct file_data *filedata) {
    free(filedata->data);
    free(filedata);
}


void combine_path_and_filename(char *result, char *path, char *filename) {
    sprintf(result, "%s/%s", path, filename);
}


/**
 * Writes null-terminated data to the file. Should
 * create the file if it does not exist.
 * @data - the data to write
 * @destination - the destination folder
 * @filename - the name of the file
 *
 * https://stackoverflow.com/a/12510903
 */
int file_write(char *data, char *destination, char *filename) {
    // does the folder exist?
    struct stat st = {0};
    if (stat(destination, &st) == -1) {
        // create dir if not exists
        mkdir(destination, 0700);
    }

    // combine path and file name
    size_t combined_size = strlen(destination) + strlen(filename) + 1;
    char combined_path [combined_size];
    combine_path_and_filename(combined_path, destination, filename);

    FILE *fp = fopen(combined_path, "ab+");
    freopen(NULL, "w+", fp);
    fprintf(fp, "%s", data);
    fclose(fp);

    return 0;
}

