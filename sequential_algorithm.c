#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

// Constants that caracterize the matrices
#define N 10
#define MAX_INPUT 50

int a[N][N], b[N][N], c[N][N];

// Functions
void randomMatrix(int m[N][N]);
void matrixMultiplication(int a[N][N], int b[N][N], int c[N][N]);
void printMatrix(int m[N][N]);

int main(void)
{
    uintmax_t start, end;

    printf("\n ----- MULTIPLICATION OF TWO %dx%d MATRICES SEQUENTIALLY ----- \n\n", N, N);

    printf("Matrix initialization...\n\n");
    // Matrices initialization
    randomMatrix(a);
    randomMatrix(b);

    printf("Multiplication...\n\n");
    // Multiplication
    start = clock();
    matrixMultiplication(a, b, c);
    end = clock();

    // Print matrices, just for small dimensions
    if (N <= 10)
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

    return 0;
}

// Functions to create a random NxN matrix
void randomMatrix(int m[N][N])
{
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            m[i][j] = 1 + rand() % 20;
        }
    }
}

// Function to sequentially multiply two NxN matrices
void matrixMultiplication(int a[N][N], int b[N][N], int c[N][N])
{
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            c[i][j] = 0;
            for (int x = 0; x < N; x++)
            {
                c[i][j] += a[i][x] * b[x][j];
            }
        }
    }
}

// Function to print a NxN matrix
void printMatrix(int m[N][N])
{
    for (int r = 0; r < N; r++)
    {
        for (int c = 0; c < N; c++)
        {
            printf(" %d\t", m[r][c]);
        }
        printf("\n");
    }
    printf("\n");
}
