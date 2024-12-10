// Authors: Caitlyn Lynch and Kristian Morgan

#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>  // Added for S_IRWXU
#include <sys/mman.h>
#include <time.h>

// Function prototypes
void dear_old_dad(sem_t *mutex, int *bankAccount);
void poor_student(sem_t *mutex, int *bankAccount);

int main(int argc, char **argv) {
    int fd, zero = 0, *bankAccount;
    sem_t *mutex;

    // Open a memory-mapped file to share the bank account variable
    fd = open("bank_account.txt", O_RDWR | O_CREAT, S_IRWXU);
    write(fd, &zero, sizeof(int)); // Initialize shared memory to 0
    bankAccount = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);

    // Initialize a semaphore for mutual exclusion
    if ((mutex = sem_open("bank_semaphore", O_CREAT, 0644, 1)) == SEM_FAILED) {
        perror("Semaphore initialization failed");
        exit(1);
    }

    srand(time(NULL)); // Seed random number generator

    // Create parent (Dear Old Dad) process
    if (fork() == 0) {
        dear_old_dad(mutex, bankAccount);
        exit(0);
    }

    // Create child (Poor Student) process
    if (fork() == 0) {
        poor_student(mutex, bankAccount);
        exit(0);
    }

    // Keep the parent running indefinitely
    while (1) {
        sleep(1);
    }

    return 0;
}

void dear_old_dad(sem_t *mutex, int *bankAccount) {
    while (1) {
        sleep(rand() % 5); // Sleep for 0-4 seconds
        printf("Dear Old Dad: Attempting to Check Balance\n");

        sem_wait(mutex); // Enter critical section
        int localBalance = *bankAccount;
        int randomNum = rand();
        if (randomNum % 2 == 0) { // Even: Attempt to deposit
            if (localBalance < 100) {
                int amount = rand() % 100;
                if (rand() % 2 == 0) { // Even: Deposit money
                    localBalance += amount;
                    printf("Dear Old Dad: Deposits $%d / Balance = $%d\n", amount, localBalance);
                } else { // Odd: No deposit
                    printf("Dear Old Dad: Doesn't have any money to give\n");
                }
                *bankAccount = localBalance; // Update shared memory
            } else {
                printf("Dear Old Dad: Thinks Student has enough Cash ($%d)\n", localBalance);
            }
        } else { // Odd: Just check balance
            printf("Dear Old Dad: Last Checking Balance = $%d\n", localBalance);
        }
        sem_post(mutex); // Exit critical section
    }
}

void poor_student(sem_t *mutex, int *bankAccount) {
    while (1) {
        sleep(rand() % 5); // Sleep for 0-4 seconds
        printf("Poor Student: Attempting to Check Balance\n");

        sem_wait(mutex); // Enter critical section
        int localBalance = *bankAccount;
        int randomNum = rand();
        if (randomNum % 2 == 0) { // Even: Attempt to withdraw
            int need = rand() % 50;
            printf("Poor Student needs $%d\n", need);
            if (need <= localBalance) { // Withdraw if enough balance
                localBalance -= need;
                printf("Poor Student: Withdraws $%d / Balance = $%d\n", need, localBalance);
            } else { // Not enough cash
                printf("Poor Student: Not Enough Cash ($%d)\n", localBalance);
            }
            *bankAccount = localBalance; // Update shared memory
        } else { // Odd: Just check balance
            printf("Poor Student: Last Checking Balance = $%d\n", localBalance);
        }
        sem_post(mutex); // Exit critical section
    }
}
