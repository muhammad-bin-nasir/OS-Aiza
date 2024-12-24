#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define SIZE 3 // Matrix size (3x3)

// Structure to pass data to each thread
typedef struct {
    int row;       // Row index for the resultant matrix
    int col;       // Column index for the resultant matrix
    int (*A)[SIZE]; // Pointer to matrix A
    int (*B)[SIZE]; // Pointer to matrix B
    int *result;   // Pointer to store the resultant element
} ThreadData;

// Thread function to calculate a single element in the resultant matrix
void *calculate_element(void *arg) {
    ThreadData *data = (ThreadData *)arg;
    int sum = 0;

    for (int k = 0; k < SIZE; k++) {
        sum += data->A[data->row][k] * data->B[k][data->col];
    }

    *data->result = sum; // Store the calculated value in the resultant matrix
    pthread_exit(NULL);  // Exit the thread
}

int main() {
    int A[SIZE][SIZE], B[SIZE][SIZE], C[SIZE][SIZE]; // Matrices A, B, and C (resultant)
    pthread_t threads[SIZE * SIZE];                 // Array to store thread IDs
    ThreadData thread_data[SIZE * SIZE];            // Array to store thread data

    // Input for matrix A
    printf("Enter elements of 3x3 matrix A:\n");
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            scanf("%d", &A[i][j]);
        }
    }

    // Input for matrix B
    printf("Enter elements of 3x3 matrix B:\n");
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            scanf("%d", &B[i][j]);
        }
    }

    // Create threads for each element of the resultant matrix
    int thread_count = 0;
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            thread_data[thread_count].row = i;
            thread_data[thread_count].col = j;
            thread_data[thread_count].A = A;
            thread_data[thread_count].B = B;
            thread_data[thread_count].result = &C[i][j];

            // Create a thread to calculate the C[i][j] element
            if (pthread_create(&threads[thread_count], NULL, calculate_element, &thread_data[thread_count]) != 0) {
                perror("Error creating thread");
                return 1;
            }

            thread_count++;
        }
    }

    // Wait for all threads to complete
    for (int i = 0; i < SIZE * SIZE; i++) {
        if (pthread_join(threads[i], NULL) != 0) {
            perror("Error joining thread");
            return 1;
        }
    }

    // Print the resultant matrix
    printf("Resultant matrix C:\n");
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            printf("%d ", C[i][j]);
        }
        printf("\n");
    }

    return 0;
}
