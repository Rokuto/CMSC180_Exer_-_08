#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>


int main(int argc, char **argv) {
    int size, rank;
    int n = 10000, root = 0;
    int i = 0;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int *globaldata = NULL;
    int *localdata = NULL;

    if (( localdata = malloc(sizeof(int) * n*n/size) ) == NULL) {
        printf("Malloc error");
        exit(1);
    }

    if (rank == 0) {
        if (( globaldata = malloc(n*n*sizeof(int) )) == NULL) {
            printf("Malloc error");
            exit(1);
        }
        
        for (i = 0; i < n * n; i++){
            globaldata[i] = 1;
        }
    }

    /* Scatter */
    MPI_Scatter(globaldata, n*n/size, MPI_INT, localdata, n*n/size, MPI_INT, root, MPI_COMM_WORLD);

    /* Column sums */
    for (i = n/size; i < n*n/size; ++i){
        localdata[i] = localdata[i] + localdata[i-(n/size)];
    }

    /* Get the last row */
    for (i = 0; i < n/size; ++i){
        localdata[i] = localdata[(n*n/size)-i-1];
    }

    
    /* Gather */
    MPI_Gather(localdata, n/size, MPI_INT, globaldata, n/size, MPI_INT, root, MPI_COMM_WORLD);

    if (rank == 0) {
        printf("Processor %d has sums: ", rank);
        for (i = 0; i < n; i++){
            printf("%d ", globaldata[i]);
        }
        printf("\n");
        free(globaldata);
    }

    free(localdata);

    MPI_Finalize();
    return 0;
}
