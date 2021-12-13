// sudoku.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <stdio.h>

#define N 9 /* size of puzzle */
#define FILE_NAME "puzzle1.txt"


static unsigned char read_puzzle_from_txt( unsigned int puzzle[N][N] )
{
    unsigned char ucStatus = 1;
    unsigned int uiRow, uiCol, uiValue;
    char cValue = 0;
    FILE* fhPuzzle;
    
    fopen_s(&fhPuzzle, FILE_NAME, "r");

    uiRow = 0;
    uiCol = 0;
    if (NULL != fhPuzzle)
    {
        while (cValue != EOF)
        {
            /* Check if boundarys OK */
            if (1 == ucStatus)
            {
                cValue = getc(fhPuzzle);

                /* Check values newline & EOF */
                if (('\n' == cValue) || (EOF == cValue))
                {
                    if (N != uiCol && uiRow < N)
                    {
                        ucStatus = 0;
                        break;
                    }
                }

                uiValue = cValue - '0';

                /* Assign uiValue if valid [1,9] */
                if (uiValue >= 1 && uiValue <= 9)
                {
                    puzzle[uiRow][uiCol] = (unsigned int)uiValue;
                }

                /* Check if end of row */
                if ('\n' == cValue)
                {
                    uiCol = 0;
                    uiRow++;
                }
                else
                {
                    uiCol++;
                }
            }
        }
    }
    return ucStatus;
}

static void print_puzzle(unsigned int puzzle[N][N])
{
    unsigned int uiRow, uiCol;

    printf("                  SODUKU PUZZLE\n");
    printf("\n");
    printf("  --------------------------------------------\n");
    for (uiRow = 0; uiRow < N; uiRow++)
    {
        for (uiCol = 0; uiCol < N; uiCol++)
        {
            if( (N-1) == uiCol)
            {
                printf("  | %d  |", puzzle[uiRow][uiCol]);
            }
            else
            {
                printf("  | %d", puzzle[uiRow][uiCol]);
            }
        }
        printf("\n");
        printf("  --------------------------------------------\n");
    }
}

int main()
{
    unsigned int puzzle[N][N] = { 0 };

    printf("\n");
    printf("Program START...\n");
    printf("\n");

    printf("             PART 1 - Reading from file.\n");
    printf("\n");
    /* Read puzzle-data from text-file */
    if (0 == read_puzzle_from_txt(puzzle))
    {
        printf("Incorrect text-file input\n");
        printf("Program END.");
        return 0;
    }

    /* Print the puzzle */
    print_puzzle(puzzle);

    printf("\n");
    printf("               PART 1 - COMPLETED.\n");

    printf("\n");
    printf("               PART 2 - Solving the puzzle.\n");

    printf("\n");
    printf("Program END.");

    return 1;
}

