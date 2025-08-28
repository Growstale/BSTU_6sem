// Lab-02e2.c (Reader)
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <errno.h>
#include <string.h>
#include <inttypes.h>
#include <time.h>

#define FILE_MAPPING_NAME "Lab-02"
#define SEMAPHORE_NAME "/Lab02_Semaphore"
#define VIEW_SIZE 65536
#define ITERATIONS 1
#define ARRAY_SIZE 655360
#define INT_ARRAY_SIZE (ARRAY_SIZE / sizeof(int))


void handle_error(const char* message) {
    perror(message);
    exit(EXIT_FAILURE);
}

void print_timestamp(const char* message) {
    time_t rawtime;
    struct tm* timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    printf("[%02d:%02d:%02d] %s\n", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, message);
    fflush(stdout);
}

int main(int argc, char* argv[]) {
    print_timestamp("Reader started.");

    if (argc != 4) { // Now expecting 4 arguments
        fprintf(stderr, "Usage: %s <memory_address> <semaphore_name> <init_semaphore_name>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    void* data_addr;
    uintptr_t temp_addr;
    if (sscanf(argv[1], "%" SCNxPTR, &temp_addr) != 1) {
        fprintf(stderr, "Error: Invalid memory address format.\n");
        exit(EXIT_FAILURE);
    }
    data_addr = (void*)temp_addr;
    int* data = (int*)data_addr;

    const char* init_semaphore_name = argv[3]; // Get the init semaphore name

    sem_t* semaphore = sem_open(argv[2], 0);  // Use argv[2] for the semaphore name
    if (semaphore == SEM_FAILED) {
        handle_error("sem_open failed (reader)");
    }

    // READER *CREATES AND INITIALIZES* the init semaphore.
    sem_t* init_semaphore = sem_open(init_semaphore_name, O_CREAT, 0666, 0);
    if (init_semaphore == SEM_FAILED) {
        sem_close(semaphore);
        handle_error("sem_open init_semaphore failed (reader)");
    }


    print_timestamp("Reader: Signaling initialization complete.");
    sem_post(init_semaphore); // Signal the writer that the reader is ready.
    sem_close(init_semaphore);


    for (int i = 0; i < ITERATIONS; ++i) {
        print_timestamp("Reader: BEFORE sem_wait");
        struct timespec ts;
        if (clock_gettime(CLOCK_REALTIME, &ts) == -1) {
            sem_close(semaphore);
            handle_error("clock_gettime failed (reader)");
        }
        ts.tv_sec += 5; // Timeout after 5 seconds

        int result = sem_timedwait(semaphore, &ts); // Use sem_timedwait
        if (result == -1) {
            if (errno == ETIMEDOUT) {
                print_timestamp("Reader: sem_timedwait TIMED OUT!");
            }
            else {
                perror("sem_timedwait failed (reader)");
            }
            sem_close(semaphore);
            exit(EXIT_FAILURE); // Exit on timeout or error
        }

        print_timestamp("Reader: AFTER sem_wait");
        print_timestamp("Reader: Reading iteration.");
        for (int j = 0; j < VIEW_SIZE / sizeof(int); ++j) {
            printf("%d ", data[j]);
        }
        printf("\n");
        fflush(stdout);

        print_timestamp("Reader: BEFORE sem_post");
        if (sem_post(semaphore) == -1) {
            sem_close(semaphore);
            handle_error("sem_post failed (reader)");
        }
        print_timestamp("Reader: AFTER sem_post");
    }

    print_timestamp("Reader: Finished reading.");
    //getchar(); // Not needed for cleanup
    sem_close(semaphore);
    print_timestamp("Reader: Resources released. Exiting");
    return 0;
}