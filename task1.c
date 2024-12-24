#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <ctype.h> // For tolower()

#define NUM_THREADS 26 // Number of threads, one for each letter of the alphabet

// Structure to hold data passed to each thread
typedef struct {
    char letter;            // The letter this thread will count
    const char *filename;   // The name of the file to read from
    int count;              // The count of occurrences of the letter
} ThreadData;

// Thread function to count occurrences of a specific letter in the file
void *count_letters(void *arg) {
    ThreadData *data = (ThreadData *)arg; // Cast argument to ThreadData structure
    FILE *file = fopen(data->filename, "r"); // Open the file for reading
    if (!file) { // Check if file opened successfully
        perror("Error opening file");
        data->count = 0; // Set count to 0 if file can't be opened
        pthread_exit(NULL); // Exit the thread
    }

    char ch;
    int count = 0;

    // Read the file character by character
    while ((ch = fgetc(file)) != EOF) {
        // If the character matches the thread's letter (case-insensitive), increment count
        if (tolower(ch) == data->letter)
            count++;
    }
    fclose(file); // Close the file after reading

    data->count = count; // Store the count in the ThreadData structure
    printf("Letter '%c': %d\n", data->letter, count); // Print the result for this thread
    pthread_exit(&data->count); // Return the count as the thread's result
}

int main() {
    char filename[256]; // Buffer to store the file name
    printf("Enter filename: ");
    scanf("%s", filename); // Take file name as input from the user

    pthread_t threads[NUM_THREADS]; // Array to store thread identifiers
    ThreadData thread_data[NUM_THREADS]; // Array to store data for each thread
    int total_count = 0; // Variable to store the total count of letters

    // Create 26 threads, one for each letter from 'a' to 'z'
    for (int i = 0; i < NUM_THREADS; i++) {
        thread_data[i].letter = 'a' + i; // Assign the letter to count (a, b, c, ..., z)
        thread_data[i].filename = filename; // Pass the file name to each thread
        thread_data[i].count = 0; // Initialize the count to 0

        // Create the thread and pass the ThreadData structure as argument
        if (pthread_create(&threads[i], NULL, count_letters, &thread_data[i]) != 0) {
            perror("Error creating thread"); // Print error if thread creation fails
            return 1; // Exit the program if thread creation fails
        }
    }

    // Wait for all threads to complete
    for (int i = 0; i < NUM_THREADS; i++) {
        void *result; // Pointer to store the thread's result
        if (pthread_join(threads[i], &result) != 0) { // Join the thread
            perror("Error joining thread"); // Print error if thread join fails
            return 1; // Exit the program if thread join fails
        }
        total_count += *(int *)result; // Add the thread's result to the total count
    }

    // Print the total number of alphabetic characters
    printf("Total alphabetic characters: %d\n", total_count);
    return 0; // Exit the program successfully
}
