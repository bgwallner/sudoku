// sudoku.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <stdio.h>
#include <limits.h>

/******************* DEFINES ******************/

#define N 9 /* size of puzzle */
#define FILE_NAME "puzzle1.txt"
#define INDEX_ERROR 0xFF
#define E_OK        0x01
#define E_NOT_OK    0x00

/**** File-global variables & definitions *****/

typedef struct
{
    unsigned long remainder;
    unsigned long integer_nbr;
} recursion_ctr_t;

static recursion_ctr_t recursion_ctr;


/* Reading the puzzle from FILE_NAME */
static unsigned char read_puzzle_from_txt( unsigned int puzzle[N][N] )
{
    unsigned char ucStatus = E_OK;
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
            cValue = getc(fhPuzzle);

            /* Check values newline & EOF */
            if (('\n' == cValue) || (EOF == cValue))
            {
                /* Only allowed at N */
                if (N != uiCol && uiRow < N)
                {
                    ucStatus = E_NOT_OK;
                    break;
                }
            }

            /* Convert char to uint */
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
    return ucStatus;
}

/* Print the puzzle to console */
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

/* Check if provided uiValue is already present in uiRow. */
static unsigned char is_in_row_valid(unsigned int uiValue, unsigned int puzzle[N][N],
                                     unsigned char uiRow, unsigned char uiCol)
{
    unsigned short uiIndex;
    unsigned char ucStatus;

    /* Boundary check */
    if ((uiRow > (N - 1)) || (uiCol > (N - 1)))
    {
        return INDEX_ERROR;
    }
    
    ucStatus = E_OK;
    for (uiIndex = 0; uiIndex < N; uiIndex++)
    {
        /* Coloumn is running index */
        if (uiIndex != uiCol)
        {
            if (uiValue == puzzle[uiRow][uiIndex])
            {
                ucStatus = E_NOT_OK;
                break;
            }
        }
    }
    return ucStatus;
}

/* Check if provided uiValue is already present in uiCol. */
static unsigned char is_in_col_valid(unsigned int uiValue, unsigned int puzzle[N][N],
                                     unsigned char uiRow, unsigned char uiCol)
{
    unsigned short uiIndex;
    unsigned char ucStatus;

    /* Boundary check */
    if ((uiRow > (N - 1)) || (uiCol > (N - 1)))
    {
        return INDEX_ERROR;
    }

    ucStatus = E_OK;
    for (uiIndex = 0; uiIndex < N; uiIndex++)
    {
        /* Row is running index */
        if (uiIndex != uiRow)
        {
            if (uiValue == puzzle[uiIndex][uiCol])
            {
                ucStatus = E_NOT_OK;
                break;
            }
        }
    }
    return ucStatus;
}

/* Check if provided uiValue is already present in group belonging to */
/* in which uiRow and uiCol is present.                               */
static unsigned char is_in_group_valid(unsigned int uiValue, unsigned int puzzle[N][N],
                                       unsigned char uiRow, unsigned char uiCol)
{
    return E_NOT_OK;
}

/* Increases counter in every recursion to determine complexity */
/* Max validity for function ULONG_MAX^2.                       */
static void increase_recursion_counter(void)
{
    /* Check if reached ULLONG_MAX (64-bits limits.h) */
    if (ULLONG_MAX == recursion_ctr.remainder &&
        ULLONG_MAX == recursion_ctr.integer_nbr)
    {
        /* Not much to do */
        recursion_ctr.integer_nbr = 0;
        recursion_ctr.remainder = 0;
    }
    else if (ULLONG_MAX == recursion_ctr.remainder)
    {
        recursion_ctr.integer_nbr++;
        recursion_ctr.remainder = 0;
    }
    else
    {
        recursion_ctr.remainder++;
    }
}

/* Function validate a 3x3 puzzle */
static unsigned char validate_3_by_3_puzzle(unsigned int puzzle[N][N])
{
    /* 1) Check that all rows have sum=45 */
    /* 2) Check that all cols have sum=45 */
    /* 3) Check that all groups have sum=45. uiRow=2,5,8 uiCol=2,5,8 and */
    /*    a[row][col]+a[col-1][row]+a[col+1][row]+... = 45               */

    return E_NOT_OK;
}

int main()
{
    unsigned int puzzle[N][N] = { 0 }; /* puzzle[row][col] = puzzle[y][x] */

    /* Init recursion counter */
    recursion_ctr.remainder = 0;
    recursion_ctr.integer_nbr = 1;

    printf("\n");
    printf("Program START...\n");
    printf("\n");

    printf("             PART 1 - Reading from file.\n");
    printf("\n");
    /* Read puzzle-data from text-file */
    if (E_NOT_OK == read_puzzle_from_txt(puzzle))
    {
        printf("\n");
        printf("ERROR: Incorrect text-file input.\n");
        printf("\n");
        printf("Program TERMINATED.\n");
        printf("\n");
        printf("Press ENTER key to Continue\n");
        printf("\n");
        getchar();
        return 0;
    }

    /* Print the puzzle */
    print_puzzle(puzzle);

    printf("\n");
    printf("               PART 1 - COMPLETED.\n");

    printf("\n");
    printf("               PART 2 - Solving the puzzle.\n");

    printf("\n");
    printf("Program END.\n");
    printf("\n");
    printf("Press ENTER key to Continue\n");
    printf("\n");
    getchar();

    return 1;
}
