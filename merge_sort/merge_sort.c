#include <mpi.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#define BUFFER_SIZE 32

void sort_leaf(int *n) {
    int buffer;
    if (n[0] > n[1]) {
        buffer = n[1];
        n[1] = n[0];
        n[0] = buffer;
    }
}

/**
 * Arrays a and b must be sorted.
 * Result store the merge of a and b.
 * result size must be at least of size_a + size_b.
 */
void merge_sorted_arrays(const int *a, int size_a, const int *b, int size_b, int *result) {
    int res_idx = 0;
    int a_idx = 0;
    int b_idx = 0;
    int res_size = size_a + size_b;

    while (res_idx < res_size) {
        if (a_idx == size_a || (b_idx < size_b && b[b_idx] < a[a_idx])) {
            result[res_idx++] = b[b_idx++];
        }
        else {
            result[res_idx++] = a[a_idx++];
        }
    }
}

void merge_comm(int *result, int *my_part, int sub_rank, MPI_Comm sub_comm) {
    int buffer[2];
    if (sub_rank == 1) {
        MPI_Bcast(my_part, 2, MPI_INT, 1, sub_comm);
    }
    else {
        MPI_Bcast(buffer, 2, MPI_INT, 1, sub_comm);
        merge_sorted_arrays(buffer, 2, my_part, 2, result);
    }
}

void last_merge(int *result, int world_rank, MPI_Comm gather_comm) {
    int recvbuff[12];
    int gather_rank;
    MPI_Comm_rank(gather_comm, &gather_rank);
    MPI_Gather(result, 4, MPI_INT, recvbuff, 4, MPI_INT, 0, gather_comm);

    if (world_rank == 0) {
        int *a = recvbuff;
        int *b = &recvbuff[4];
        int *c = &recvbuff[8];
        int first_merge[8];
        merge_sorted_arrays(a, 4, b, 4, first_merge);
        merge_sorted_arrays(c, 4, first_merge, 8, result);
        for (int i = 0; i < 12; ++i) {
            printf("%d ", result[i]);
        }
        printf("\n");   
    }
}

void merge(int *result, int *scattered, int world_rank, int sub_rank, MPI_Comm sub_comm) {
    sort_leaf(scattered);
    merge_comm(result, scattered, sub_rank, sub_comm);
    MPI_Comm gather_comm;

    MPI_Comm_split(MPI_COMM_WORLD, sub_rank + 3, 0, &gather_comm);
    if (sub_rank == 0) {
        last_merge(result, world_rank, gather_comm);
    }
}

void set_randoms_12(int *randoms) {
    for (int i = 0; i < 12; ++i) {
        randoms[i] = rand() % 100;
    }
}

int main(void) {
    srand(time(NULL));
    int randoms[12];
    int scattered[2];
    set_randoms_12(randoms);

    MPI_Init(NULL, NULL);
    int world_size;
    int world_rank;
    int sub_rank;
    int sub_size;
    MPI_Comm sub_comm;

    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    if (world_rank == 0) {
        for (int i = 0; i < 12; i++) {
            printf("%d ", randoms[i]);
        }
        printf("\n");
    }

    int color = world_rank % 3;

    MPI_Scatter(randoms, 2, MPI_INT, scattered, 2, MPI_INT, 0, MPI_COMM_WORLD);

    MPI_Comm_split(MPI_COMM_WORLD, color, 0, &sub_comm);
    MPI_Comm_rank(sub_comm, &sub_rank);
    MPI_Comm_size(sub_comm, &sub_size);

    merge(randoms, scattered, world_rank, sub_rank, sub_comm);

    MPI_Finalize();

    return 0;
}
