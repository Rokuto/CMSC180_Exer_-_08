/*
    sudo apt-get install libcr-dev mpich2 mpich2-doc

 */

#include <math.h>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    int size, rank, nsize, sizeN;

    int *globaldata = NULL;
    int *localdata = NULL;
    int *displ = NULL;
    int *sizes = NULL;
    
    int mul = 10000;
    int n = (mul * atoi(argv[1]));
    int i = 0, j = 0;
    // int k = 0, l = 0;
    int root = 0;
    int sn = (n * n);
    int localsize;
    int resizer;

    // int m[n][n]; // use in data manipulation


    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    displ = malloc(sizeof(int) * size);
    sizes = malloc(sizeof(int) * size);

    nsize = (sn/size);
    sizeN = (n/size);

    localsize = (int)(ceil((double)n/(double)size) * n);
    if(size == 3){
        localsize = localsize + n;
    }

    if (( localdata = malloc( localsize * sizeof(int) ) ) == NULL) {
        printf("Malloc error2\n");
        exit(1);
    }


    if (rank == 0) {
        if (( globaldata = malloc(sn * sizeof(int))) == NULL) {
            printf("Malloc error1\n");
            exit(1);
        }

        /* Create the Matrix */
        // k = 0;
        // for (i = 0; i < n; ++i){
        //     for (j = 0; j < n; ++j){
        //         m[i][j] = 1;
        //         // printf("%i ", m[i][j]);
        //     }
        //     // printf("\n");
        // }
        // // printf("\n");


        /* Manipulate the orientation (for sending) */
        // k = 0;
        // for (l = 0; l < size; ++l){
        //     for (j = 0; j < n; ++j){
        //         for (i = 0; i < sizeN; ++i){
        //             globaldata[k++] = m[j][i + (l * sizeN)];
        //             // printf("%i ", m[j][i + (l * sizeN)]);
        //         }
        //         if(size == 3 && l == 2){
        //             for (i; i + (l * sizeN) < n; ++i){
        //                 globaldata[k++] = m[j][i + (l * sizeN)];
        //             }
        //         }
        //         // printf("\n");
        //     }
        // }
        // printf("\n");


        /* (to be used in the exer) since random and less memory used */
        for(i = 0; i < sn; i++){
            globaldata[i] = 1;
            //printf("%i ", globaldata[i]);
        }
        printf("\n");
        
    }
    
    /* Calculations of sizes and displacement for 3 => processors*/
    if(size <= 3){    
        for (i = 1; i <= size; ++i){
            displ[i-1] = (i-1) * (n/size * n);
            
            if(size == 3 && i == size){
                sizes[i-1] = sn - displ[i-1];
            }else{
                sizes[i-1] = (i) * (n/size * n) - displ[i-1];
            }
            // printf("displ:%i  size:%i\n", displ[i-1], sizes[i-1]);
        }
        // printf("\n");
    }

    /* Calculation of sizes and displacement for 4 processors */
    if(size == 4){
        for (i = 1; i <= size; ++i){

            displ[i-1] = (i-1) * localsize;
            if(displ[i-1] > sn){
                displ[i-1] = sn;
            }

            if(i == size){
                sizes[i-1] = (sn) - displ[i-1];
                // printf("%i :\n", (sn) - (int)(ceil((double)n/(double)size) * n) );
            }else{
                sizes[i-1] = ((i) * (localsize) - displ[i-1]);
                // printf("%i : %i\n", displ[i-1], sizes[i-1] );
            }           
        }
        // printf("\n");
    }


    /* Initialize localdata */
    for (i = 0; i < localsize; ++i){
        localdata[i] = 0;
        // printf("%i ", localdata[0]);
    }
    // printf("\n");

    /* Scatter */
    // MPI_Scatter(&(globaldata[0]), nsize, MPI_INT, localdata, nsize, MPI_INT, root, MPI_COMM_WORLD);
    MPI_Scatterv(&(globaldata[0]), sizes, displ, MPI_INT, localdata, localsize, MPI_INT, root, MPI_COMM_WORLD);
    

    // printf("Before:");
    // for (i = 0; i < localsize; ++i){
    //     printf("%i ", localdata[i]);
    //     //localdata[i] = localdata[i] + localdata[i-(sizeN)];
    // }
    // printf("\n");
    resizer = (sizes[rank]/n);

    /* Column sums */
    for (i = resizer; i < sizes[rank]; ++i){
        localdata[i] = localdata[i] + localdata[(i - resizer)];
        // printf("%i ", localdata[i]);
    }

    /* Get the last row */
    // printf("After:");
    for (i = ( sizes[rank] - resizer ); i < sizes[rank]; ++i){
        // printf("%i:%i ", i % resizer, localdata[i]);
        localdata[i % resizer] = localdata[i];
    }

    /* Recompute sizes and displacement to be send */
    for (i = 1; i <=size; ++i){
        sizes[i-1] = (sizes[i-1]/n);
        // printf("size %i\n", sizes[i-1]);
        if(i < size){
            displ[i] = displ[i-1]+sizes[i-1];
            // printf("%i\n", displ[i]);
        }
    }
    // printf("\n");

    // printf("Then:");
    // for (i = 0; i < localsize; ++i){
    //     printf("%i ", localdata[i]);
    //     //localdata[i] = localdata[i] + localdata[i-(sizeN)];
    // }
    // printf("\n");

    /* Gather */
    MPI_Barrier(MPI_COMM_WORLD);
    //MPI_Gather(localdata, sizeN, MPI_INT, globaldata, sizeN, MPI_INT, root, MPI_COMM_WORLD);
    MPI_Gatherv(localdata, sizes[rank], MPI_INT, globaldata, sizes, displ, MPI_INT, root, MPI_COMM_WORLD);

    if (rank == 0) {
        printf("Processor %d has sums: ", rank);
        for (i = 0; i < n; i++){
            printf("%d ", globaldata[i]);
        }
        printf("\n");
        free(globaldata);
    }

    free(localdata);
    free(displ);
    free(sizes);

    MPI_Finalize();
    return 0;
}
