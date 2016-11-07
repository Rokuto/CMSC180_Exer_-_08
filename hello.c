#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>


int *createMatrix (int nrows, int ncols) {
    int *matrix, row[nrows][ncols];
    int h = 0, i, j;

    if (( matrix = malloc(nrows*ncols*sizeof(int))) == NULL) {
        printf("Malloc error");
        exit(1);
    }

    for(i = 0; i < nrows; i++){
        for(j = 0; j < ncols; j++){
            row[i][j] = 1;
            //printf("%d ", row[i][j]);
            h++;
        }
        //printf("\n");
    }

    h = 0;
    for(i = 0; i < nrows; i++){
        for(j = 0; j < ncols; j++){
            matrix[h] = row[j][i];
            h++;
        }
    }

    return matrix;
}

int main(int argc, char **argv) {
    int size, rank;
    int n = 12000, root = 0;
    int i = 0;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int *globaldata = NULL;
    int *localdata = NULL;
    //int *localsum = NULL;
    //int *globalsum = NULL;

    //localsum = malloc(sizeof(int) * n/size);
    localdata = malloc(sizeof(int) * n*n/size);

    if (rank == 0) {
        globaldata = createMatrix(n, n);
        //globalsum = malloc(sizeof(int) * n);

        //printf("Processor %d has data: ", rank);
        /*for (i = 0; i < n * n; i++){
            printf("%d ", globaldata[i]);
        }
        printf("\n");*/
    }

    /* Scatter */
    MPI_Scatter(globaldata, n*n/size, MPI_INT, localdata, n*n/size, MPI_INT, root, MPI_COMM_WORLD);

    /* Column sums */
    for (i = n/size; i < n*n/size; ++i){
        //printf("Processor %d has data %d\n", rank, localdata[i]);
        localdata[i] = localdata[i] + localdata[i-(n/size)];
    }
    /* Print answer 
    // for (int i = 0; i < n*n/size; ++i){
    //     printf("Processor %d doubling the data, now has %d\n", rank, localdata[i]);
    // }*/

    /* Get the last row */
    for (i = 0; i < n/size; ++i){
        //printf("Processor %d data, now has %d\n", rank, localdata[(n*n/size)-i-1]);
        //localsum[i] = localdata[(n*n/size)-i-1];
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
    }

    if (rank == 0){
        free(globaldata);
        //free(globalsum);
    }
    free(localdata);

    //free(localsum);
    //free(localdata);
    MPI_Finalize();
    return 0;
}
