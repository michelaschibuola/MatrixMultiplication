#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <string.h>
#include <math.h>

// Constants that caracterize the matrices
#define N 10

#define MAX_INPUT 50

// Input and output matrices
int a[N][N], b[N][N], c[N][N];

// Functions
void randomMatrix(int m[N][N]);
void printMatrix(int m[N][N]);

int main(int argc, char **argv)
{
    uintmax_t start, end;

    // Initialize MPI
    int rank, nproc;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nproc);
    MPI_Status status;
    
    // Compute information about the processor work load
    int cellsPerRank = ((N * N) % nproc) == 0 ? (N*N)/nproc : ((N*N)/nproc)+1;

    int initialRow = ((rank * cellsPerRank) + 0) / N;
    int finalRow = ((rank * cellsPerRank) + cellsPerRank - 1) / N;
    
    int initialCol = ((rank * cellsPerRank) + 0) % N;
    int finalCol = ((rank * cellsPerRank) + cellsPerRank - 1) % N;
    
    int cellCount = finalRow < N ? cellsPerRank : cellsPerRank - ((finalRow - N)*N) - (finalCol + 1);

    finalRow = finalRow < N ? finalRow : N - 1;
    finalCol = ((rank * cellsPerRank) + cellsPerRank - 1) / N < N ? finalCol : N - 1;
    

    if (rank == 0)
    {
        printf("\n ----- MULTIPLICATION OF TWO %dx%d MATRICES USING %d PROCESSORS ----- \n\n", N, N, nproc);

        // Matrix random initialization
        printf("Matrix initialization...\n\n");
        randomMatrix(a);
        randomMatrix(b);

        printf("Multiplication...\n\n");
        // Start time
        start = clock();

        int pInitialRow, pFinalRow;
        for (int p = 1; p < nproc; p++)
        {
            // Receive the initial and final row of each processor
            MPI_Recv(&pInitialRow, 1, MPI_INT, p, 0, MPI_COMM_WORLD, &status);
            MPI_Recv(&pFinalRow, 1, MPI_INT, p, 0, MPI_COMM_WORLD, &status);

            // Send the needed data to each processor
            MPI_Send(&a[pInitialRow][0], N * (pFinalRow - pInitialRow + 1), MPI_INT, p, 1, MPI_COMM_WORLD);
            MPI_Send(&b, N * N, MPI_INT, p, 1, MPI_COMM_WORLD);
        }
    }
    else
    {
        // Send the initial and final row to the master
        MPI_Send(&initialRow, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        MPI_Send(&finalRow, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);

        // Receive the needed data from the master
        MPI_Recv(&a[initialRow][0], N * (finalRow - initialRow + 1), MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
        MPI_Recv(&b, N * N, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
    }

    // Each processor computes a part of the resulting matrix
    int i, j, x;
    for (int count = 0; count < cellCount; count++)
    {
        i = ((rank * cellsPerRank) + count) / N;
        j = ((rank * cellsPerRank) + count) % N;
        if (i < N && j < N)
        {
            c[i][j] = 0;
            for (x = 0; x < N; x++)
            {
                c[i][j] += (a[i][x] * b[x][j]);
            }
        }
    }

    if (rank != 0)
    {
        // Send the part of the resulting matrix to the master
        MPI_Send(&initialRow, 1, MPI_INT, 0, 2, MPI_COMM_WORLD);
        MPI_Send(&initialCol, 1, MPI_INT, 0, 2, MPI_COMM_WORLD);
        MPI_Send(&cellCount, 1, MPI_INT, 0, 2, MPI_COMM_WORLD);
        MPI_Send(&c[initialRow][initialCol], cellCount, MPI_INT, 0, 2, MPI_COMM_WORLD);
    }
    else
    {
        int pInitialRow, pInitialCol, pCellCount;
        for (int p = 1; p < nproc; p++)
        {
            // Receive the part of the resulting matrix from each processor
            MPI_Recv(&pInitialRow, 1, MPI_INT, p, 2, MPI_COMM_WORLD, &status);
            MPI_Recv(&pInitialCol, 1, MPI_INT, p, 2, MPI_COMM_WORLD, &status);
            MPI_Recv(&pCellCount, 1, MPI_INT, p, 2, MPI_COMM_WORLD, &status);
            MPI_Recv(&c[pInitialRow][pInitialCol], pCellCount, MPI_INT, p, 2, MPI_COMM_WORLD, &status);
        }
        end = clock();

        // Print matrices, just for small dimensions
        if(N <= 10)
        {
            printf("Matrix A:\n");
            printMatrix(a);
            printf("Matrix B:\n");
            printMatrix(b);
            printf("Matrix C = AxB:\n");
            printMatrix(c);
        }
        else
        {
            printf("Resulting matrix calculated\n");
        }
        printf("\n");

        // Print execution time
        printf("Elapsed time: %ju - %ju = %ju ticks\n", end, start, end - start);
        printf("Elapsed time: %8.6f seconds == %8.6e seconds\n", (double)(end - start) / CLOCKS_PER_SEC, (double)(end - start) / CLOCKS_PER_SEC);
        printf("\n");
    }

    // Finalize MPI
    MPI_Finalize();

    return 0;
}

// Functions to create a random NxN matrix
void randomMatrix(int m[N][N])
{
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            m[i][j] = 1 + rand() % MAX_INPUT;
        }
    }
}

// Function to print a NxN matrix
void printMatrix(int m[N][N])
{
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            printf(" %d\t", m[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}
