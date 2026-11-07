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

void merge_comm(int *result, int *my_part, int sub_rank, MPI_Comm sub_comm) {
    int buffer[2];
    printf("merge_comm: my sub_rank %d, my part: %d, %d\n", sub_rank, my_part[0], my_part[1]);
    fflush(stdout);
    if (sub_rank == 1) {
        MPI_Bcast(my_part, 2, MPI_INT, 1, sub_comm);
        printf("sent!\n");
        fflush(stdout);
    }
    else {
        MPI_Bcast(buffer, 2, MPI_INT, 1, sub_comm);
        int my_part_idx = 0;
        int buffer_idx = 0;
        int mp;
        int b;
        while (my_part_idx < 2) {
            mp = my_part[my_part_idx];
            if (buffer_idx < 2) {
                b = buffer[buffer_idx];
                if (mp < b) {
                    result[my_part_idx + buffer_idx] = mp;
                    my_part_idx++;
                }
                else {
                    result[my_part_idx + buffer_idx] = b;
                    buffer_idx++;
                }
            }
            else {
                result[my_part_idx + buffer_idx] = mp;
                my_part_idx++;
            }
        }
        while (buffer_idx < 2) {
            result[my_part_idx + buffer_idx] = buffer[buffer_idx++];
        }
        printf("part_res = %d, %d, %d, %d\n", result[0], result[1], result[2], result[3]);
    }
}

void last_merge(int *result, int world_rank, int sub_rank) {
    if (world_rank == 1 || sub_rank == 0) {
        MPI_Group
        MPI_Comm_create()
    }
    if (world_rank == 1) {
        MPI_Gather()
    }
    if (sub_rank == 0) {
        MPI_Gather()
    }
}

void merge(int *result, int *scattered, int world_rank, int sub_rank, MPI_Comm sub_comm) {
    sort_leaf(scattered);
    merge_comm(result, scattered, sub_rank, sub_comm);
    last_merge(result, world_rank, sub_rank);
}

void set_randoms_12(int *randoms) {
    printf("random init: ");
    for (int i = 0; i < 12; ++i) {
        randoms[i] = rand() % 100;
        printf("%d ", randoms[i]);
    }
    printf("\n");
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
    int color = world_rank % 3;

    MPI_Scatter(randoms, 2, MPI_INT, scattered, 2, MPI_INT, 0, MPI_COMM_WORLD);

    MPI_Comm_split(MPI_COMM_WORLD, color, 0, &sub_comm);
    MPI_Comm_rank(sub_comm, &sub_rank);
    MPI_Comm_size(sub_comm, &sub_size);

    char set_name[10];
    sprintf(set_name, "COMM_%d", color);
    MPI_Comm_set_name(sub_comm, set_name);

    char comm_name[MPI_MAX_OBJECT_NAME];
    int name_len;
    MPI_Comm_get_name(sub_comm, comm_name, &name_len);
    comm_name[name_len] = '\0';

    // printf("my world rank: %d of %d, my color: %d, my sub_rank: %d of %d, my comm name: %s\n My scattered: %d | %d\n",
    //     world_rank, world_size, color, sub_rank, sub_size, comm_name, scattered[0], scattered[1]);

    merge(randoms, scattered, world_rank, sub_rank, sub_comm);

    MPI_Finalize();

    return 0;
}