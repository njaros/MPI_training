#include <mpi.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char** argv) {

        MPI_Init(NULL, NULL);      // initialize MPI environment
        int world_size; // number of processes
        MPI_Comm_size(MPI_COMM_WORLD, &world_size);

        int world_rank; // the rank of the process
        MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

        char processor_name[MPI_MAX_PROCESSOR_NAME]; // gets the name of the processor
        int name_len;
        MPI_Get_processor_name(processor_name, &name_len);

        pid_t pid = getpid();

        printf("Hello world from processor %s, rank %d out of %d processors. process %d\n",
                processor_name, world_rank, world_size, pid);
                
        if (world_rank == 0) {
                MPI_Bcast("Broadcasted hello", 18, MPI_BYTE, 0, MPI_COMM_WORLD);
                for (int i = 1; i < world_size; i++) {
                        char to_send[8];
                        sprintf(to_send, "hello %d", i);
                        MPI_Send(to_send, 8, MPI_BYTE, i, 0, MPI_COMM_WORLD);
                }
                sleep(1);
        }
        else {
                char buffer[18];
                MPI_Bcast(buffer, 18, MPI_BYTE, 0, MPI_COMM_WORLD);
                printf("Me, %d, received from 0: %s\n", world_rank, buffer);

                MPI_Recv(buffer, 8, MPI_BYTE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                printf("Me, %d, received from 0: %s\n", world_rank, buffer);
        }

        MPI_Barrier(MPI_COMM_WORLD);

        MPI_Finalize(); // finish MPI environment
}