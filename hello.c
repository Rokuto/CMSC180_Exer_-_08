#include <stdio.h>
#include <mpi.h>

int main(int argc, char** argv) {
    int i, myrank, nprocs, number, num_elements_to_send = 1, recv; 
    int num_elements_recv = 1, root = 0;
    int num[4], data[4];

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);

    printf("Hello from processor %d of %d\n", myrank, nprocs);

    if (myrank == 0) {
        number = -1;
        for(i = 0; i < 4; i++){
            data[i] = i;
        }

        //MPI_Send(&num, 4, MPI_INT, 1, 0, MPI_COMM_WORLD);
    }

    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Scatter(data, num_elements_to_send. MPI_INT, &recv, num_elements_recv, MPI_COMM_WORLD);

    if(myrank != 0){
        printf("I %d receive %d\n", myrank, recv);
    }

    MPI_Barrier(MPI_COMM_WORLD);

    MPI_Finalize();
    return 0;
}
