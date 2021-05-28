#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "pongo.h"
#include "pongo_doom_vfs.h"
#include "doom1shareware.h"

struct pongo_file {
    char* file_name;
    unsigned char* file_data;
    size_t file_size;
};

struct pongo_fd_info {
    struct pongo_file* file;
    unsigned int pos;
    int in_use;
};

static struct pongo_file* pongo_files;
static size_t num_pongo_files;
static struct pongo_fd_info fds[30]; // Hacky hack hack, should malloc/realloc or use a linked list

#define NUM_PONGO_FILES 1

void init_pongo_vfs() {

    memset(fds, 0, sizeof(fds));

    static struct pongo_file s_pongo_files[NUM_PONGO_FILES];

    struct pongo_file doom1_wad;
    doom1_wad.file_name = "./doom1.wad";
    doom1_wad.file_data = doom1_shareware_wad;
    doom1_wad.file_size = doom1_shareware_wad_len;

    s_pongo_files[0] = doom1_wad;

    pongo_files = s_pongo_files;
    num_pongo_files = NUM_PONGO_FILES;

}

struct pongo_file* find_pongo_file(const char *pathname) {
   // printf("PONGO:, find file called...\n");
    for (size_t i = 0; i < num_pongo_files; i++) {
        //printf("PONGO find: i:%d\n", i);
        int result = strcmp(pathname, pongo_files[i].file_name);
        //printf("PONGO find result: %d\n", result);
        if (!strcmp(pathname, pongo_files[i].file_name)) {
            return &(pongo_files[i]);
        }
    }
    return NULL;
}

//TODO errno setting stuff
int access(const char *pathname, int mode) {
    struct pongo_file* file = find_pongo_file(pathname);
    if (file == NULL) {
        return -1;
    }

    // Don't support Writing or Executing
    if ((mode & (X_OK | W_OK)) != 0) {
        return -1;
    }

    //printf("Found: %s\n", pathname);
    return 0;
}

int fstat(int fd, struct stat *buf) {

    // STDIN, STDOUT, STDERR
    if (fd <= 2) {
        return -1;
    }

    if (fds[fd].in_use) {
        buf->st_size = fds[fd].file->file_size;
        return 0;
    }

    return -1;
}

int open(const char* path, int flags) {
    struct pongo_file* file = find_pongo_file(path);
    if (file == NULL) {
        return -1;
    }

    // Find first available descriptor
    //printf("Finding descriptor for open...\n");
    int i = 0;
    for (i = 3; i < sizeof(fds)/sizeof(fds[0]); i++) {
        if (fds[i].in_use == 0) {
            fds[i].in_use = 1;
            fds[i].file = file;
            fds[i].pos = 0;
            //printf("Found one, i:%d...\n", i);
            return i;
        }
    }
    
    // No available descriptors
    //printf("Didnt find one? i:%d...\n", i);
    return -1;
}


void close(int handle) {
    if (handle < sizeof(fds)/sizeof(fds[0])) {
        fds[handle].in_use = 0;
    }
}

ssize_t read(int fd, void *buf, size_t count) {
    //printf("Reading from fd: %d, in_use: %d, count:%u\n", fd, fds[fd].in_use, count);
    if (fd >= 3) {
        if (fds[fd].in_use) {
            struct pongo_file* file = fds[fd].file;
            size_t bytes_left = file->file_size - fds[fd].pos;
            size_t bytes_to_read = count;
            void* data_pos = &file->file_data[fds[fd].pos];
            if (count > bytes_left) {
                bytes_to_read = bytes_left;
            }
            //printf("Reading %u bytes at offset %u\n", bytes_to_read, fds[fd].pos);
            if (bytes_to_read > 0) {
                memcpy(buf, data_pos, bytes_to_read);
                char* c_buf = (char*)buf;
                //printf("%c %c %c %c\n", c_buf[0], c_buf[1], c_buf[2], c_buf[3]);
                fds[fd].pos += bytes_to_read;
            }

            return bytes_to_read;
        }
    }
    return -1;
}

off_t lseek(int fd, off_t offset, int whence) {
    if (fd >= 3) {
        if (fds[fd].in_use) {
            struct pongo_file* file = fds[fd].file;
      
            switch(whence) {
                case SEEK_SET:
                    if (offset <= file->file_size) {
                        fds[fd].pos = offset;
                        return fds[fd].pos;
                    }
                    return -1; // Only support Reads and now beyond EOF

                case SEEK_CUR:
                    if (offset + fds[fd].pos <= file->file_size) {
                        fds[fd].pos += offset;
                        return fds[fd].pos;
                    }
                    return -1; // Only support Reads and now beyond EOF

                case SEEK_END:
                    if (offset == 0) {
                        fds[fd].pos = file->file_size;
                        return fds[fd].pos;
                    }
                    return -1; // Only support Reads and now beyond EOF
            }
        }
    }
    return -1;
}


FILE * fopen ( const char * filename, const char * mode ) {
    return open(filename, mode);
}

int fseek ( FILE * stream, long int offset, int origin ) {
    return lseek(stream, offset, origin);
}

size_t fread ( void * ptr, size_t size, size_t count, FILE * stream ) {
    return read(stream, ptr, (size * count)); // TODO should check size * count
}

long int ftell ( FILE * stream ) {
    int fd = (int)stream;
    if (fd >= 3) {
        if (fds[fd].in_use) {
            return fds[fd].pos;
        }
    }
    return -1;
}

int feof ( FILE * stream ) {
    int fd = (int)stream;
    if (fd >= 3) {
        if (fds[fd].in_use) {
            return fds[fd].pos >= fds[fd].file->file_size;
        }
    }
    return 0;
}

int fclose ( FILE * stream ) {
    close(stream);
    return 0;
}