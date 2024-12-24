#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/sysinfo.h>

// Structure to pass data to each thread
typedef struct {
    int *array;
    int start;
    int end;
} ThreadData;

// Merge function to merge two halves
void merge(int arr[], int start, int mid, int end) {
    int n1 = mid - start + 1;
    int n2 = end - mid;

    // Temporary arrays
    int *left = (int *)malloc(n1 * sizeof(int));
    int *right = (int *)malloc(n2 * sizeof(int));

    for (int i = 0; i < n1; i++)
        left[i] = arr[start + i];
    for (int i = 0; i < n2; i++)
        right[i] = arr[mid + 1 + i];

    int i = 0, j = 0, k = start;
    while (i < n1 && j < n2) {
        if (left[i] <= right[j])
            arr[k++] = left[i++];
        else
            arr[k++] = right[j++];
    }

    while (i < n1)
        arr[k++] = left[i++];
    while (j < n2)
        arr[k++] = right[j++];

    free(left);
    free(right);
}

// Merge Sort function
void merge_sort(int arr[], int start, int end) {
    if (start < end) {
        int mid = start + (end - start) / 2;

        merge_sort(arr, start, mid);
        merge_sort(arr, mid + 1, end);
        merge(arr, start, mid, end);
    }
}

// Thread function for sorting a segment
void *thread_merge_sort(void *arg) {
    ThreadData *data = (ThreadData *)arg;
    merge_sort(data->array, data->start, data->end);
    pthread_exit(NULL);
}

// Main function
int main() {
    int num_cores = get_nprocs(); // Get the number of processor cores
    printf("Number of processor cores: %d\n", num_cores);

    int size;
    printf("Enter size of the array: ");
    scanf("%d", &size);

    int *array = (int *)malloc(size * sizeof(int));
    printf("Enter %d elements of the array:\n", size);
    for (int i = 0; i < size; i++)
        scanf("%d", &array[i]);

    pthread_t threads[num_cores];
    ThreadData thread_data[num_cores];
    int segment_size = size / num_cores;

    // Divide array into segments and create threads
    for (int i = 0; i < num_cores; i++) {
        thread_data[i].array = array;
        thread_data[i].start = i * segment_size;
        thread_data[i].end = (i == num_cores - 1) ? (size - 1) : ((i + 1) * segment_size - 1);

        if (pthread_create(&threads[i], NULL, thread_merge_sort, &thread_data[i]) != 0) {
            perror("Error creating thread");
            free(array);
            return 1;
        }
    }

    // Join threads
    for (int i = 0; i < num_cores; i++) {
        if (pthread_join(threads[i], NULL) != 0) {
            perror("Error joining thread");
            free(array);
            return 1;
        }
    }

    // Merge all segments
    for (int i = 0; i < num_cores - 1; i++) {
        int start = thread_data[0].start;
        int mid = thread_data[i].end;
        int end = thread_data[i + 1].end;
        merge(array, start, mid, end);
    }

    printf("Sorted array:\n");
    for (int i = 0; i < size; i++)
        printf("%d ", array[i]);
    printf("\n");

    free(array);
    return 0;
}
