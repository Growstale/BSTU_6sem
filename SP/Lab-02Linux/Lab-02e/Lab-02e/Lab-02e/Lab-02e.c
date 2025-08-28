// Writer (Lab-02e)
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <errno.h>
#include <string.h>
#include <inttypes.h>
#include <time.h>

#define SEMAPHORE_NAME "/Lab02_Semaphore"
#define INIT_SEMAPHORE_NAME "/Lab02_InitSem"
#define VIEW_SIZE 65536
#define ITERATIONS 1 // Reduced for debugging
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

void reader_process(int* data) {
    sem_t* semaphore = sem_open(SEMAPHORE_NAME, 0);
    if (semaphore == SEM_FAILED) {
        handle_error("sem_open failed (reader)");
    }

    sem_t* init_semaphore = sem_open(INIT_SEMAPHORE_NAME, 0);
    if (init_semaphore == SEM_FAILED) {
        sem_close(semaphore);
        handle_error("sem_open init_semaphore failed (reader)");
    }

    print_timestamp("Reader: Waiting for initialization...");
    if (sem_wait(init_semaphore) == -1) {
        sem_close(semaphore);
        sem_close(init_semaphore);
        handle_error("sem_wait init_semaphore failed (reader)");
    }
    print_timestamp("Reader: Initialization complete.");
    sem_close(init_semaphore);

    for (int i = 0; i < ITERATIONS; ++i) {
        print_timestamp("Reader: BEFORE sem_wait");

        struct timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);
        ts.tv_sec += 5;

        if (sem_timedwait(semaphore, &ts) == -1) {
            if (errno == ETIMEDOUT) {
                print_timestamp("Reader: sem_timedwait TIMED OUT!");
            }
            else {
                perror("sem_timedwait failed (reader)");
            }
            sem_close(semaphore);
            exit(EXIT_FAILURE);
        }

        print_timestamp("Reader: AFTER sem_wait");

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
    sem_close(semaphore);
    print_timestamp("Reader: Resources released. Exiting");
}

int main() {
    int* data;
    sem_t* semaphore;
    sem_t* init_semaphore;

    data = (int*)mmap(NULL, ARRAY_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (data == MAP_FAILED) {
        handle_error("mmap failed");
    }

    semaphore = sem_open(SEMAPHORE_NAME, O_CREAT, 0666, 1);
    if (semaphore == SEM_FAILED) {
        munmap(data, ARRAY_SIZE);
        handle_error("sem_open failed");
    }

    init_semaphore = sem_open(INIT_SEMAPHORE_NAME, O_CREAT, 0666, 0);
    if (init_semaphore == SEM_FAILED) {
        sem_close(semaphore);
        sem_unlink(SEMAPHORE_NAME);
        munmap(data, ARRAY_SIZE);
        handle_error("sem_open init_semaphore failed");
    }

    pid_t pid = fork();
    if (pid == -1) {
        handle_error("fork failed");
    }

    if (pid == 0) {
        // Child process (Reader)
        print_timestamp("Reader (child) started.");
        reader_process(data);
        exit(EXIT_SUCCESS);
    }

    // Parent process (Writer)
    print_timestamp("Writer: Signaling init_semaphore.");
    sem_post(init_semaphore);
    sem_close(init_semaphore);

    int* write_data = (int*)malloc(ARRAY_SIZE);
    if (write_data == NULL) {
        sem_close(semaphore);
        sem_unlink(SEMAPHORE_NAME);
        sem_unlink(INIT_SEMAPHORE_NAME);
        munmap(data, ARRAY_SIZE);
        handle_error("Could not allocate memory");
    }

    for (int i = 0; i < INT_ARRAY_SIZE; ++i) {
        write_data[i] = i;
    }

    for (int i = 0; i < ITERATIONS; ++i) {
        print_timestamp("Writer: BEFORE sem_wait");
        if (sem_wait(semaphore) == -1) {
            handle_error("sem_wait failed (writer)");
        }
        print_timestamp("Writer: AFTER sem_wait");

        memcpy(data, write_data, VIEW_SIZE);

        print_timestamp("Writer: BEFORE sem_post");
        if (sem_post(semaphore) == -1) {
            handle_error("sem_post failed (writer)");
        }
        print_timestamp("Writer: AFTER sem_post");
        print_timestamp("Writer: Wrote iteration.");
        usleep(150000);
    }

    free(write_data);
    print_timestamp("Writer: Finished writing. Waiting for reader...");
    waitpid(pid, NULL, 0);

    print_timestamp("Writer: Reader finished.");
    sem_close(semaphore);
    sem_unlink(SEMAPHORE_NAME);
    sem_unlink(INIT_SEMAPHORE_NAME);
    munmap(data, ARRAY_SIZE);

    print_timestamp("Writer: Resources released. Exiting");
    return 0;
}
