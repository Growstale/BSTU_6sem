#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#define FILE_NAME "text.txt"
#define MAP_SIZE 8192 // 8 * 1024
#define OFFSET 4096 // 4 * 1024
#define NEW_DATA "Hello, world! This is new data written to the mapped file.\n"

void handle_error(const char *message) {
    perror(message);
    exit(EXIT_FAILURE);
}

int main() {
    int fd;
    struct stat file_info;

    fd = open(FILE_NAME, O_RDWR | O_CREAT, 0666);
    if (fd == -1) {
        handle_error("Failed to open/create file");
    }

    void *mapped_memory = mmap(NULL, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, OFFSET);
    if (mapped_memory == MAP_FAILED) {
        handle_error("Failed to create memory mapping");
    }

    printf("Original content of the mapped region:\n%s\n", (char*)mapped_memory);

    printf("Writing new data to the mapped region...\n");
    memcpy(mapped_memory, NEW_DATA, strlen(NEW_DATA));

    printf("Process PID: %d\n", getpid());
    printf("Press ENTER to continue and unmap the memory...\n");
    getchar();

    printf("Syncing memory to disk...\n");
    if (msync(mapped_memory, MAP_SIZE, MS_SYNC) == -1) {
        handle_error("Failed to sync memory to disk");
    }

    printf("Cleaning up...\n");
    if (munmap(mapped_memory, MAP_SIZE) == -1) {
        handle_error("Failed to unmap memory");
    }

    if (close(fd) == -1) {
        handle_error("Failed to close file");
    }

    printf("Done. Check the file '%s'\n", FILE_NAME);

    return EXIT_SUCCESS;
}
