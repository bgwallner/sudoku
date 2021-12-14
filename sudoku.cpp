// sudoku.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <stdio.h>
#include <limits.h>

/******************* DEFINES ******************/

#define N 9 /* size of puzzle */
#define SUDOKU_SUM 45 /* sum of row, col or group */
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

/******************* LOCAL FUNCTION DECLARATIONS *****************/

static unsigned char read_puzzle_from_txt(unsigned int puzzle[N][N]);
static void print_puzzle(unsigned int puzzle[N][N]);
static unsigned char is_in_row_valid(unsigned int uiValue, unsigned int puzzle[N][N],
                                     unsigned int uiRow, unsigned int uiCol);
static unsigned char is_in_col_valid(unsigned int uiValue, unsigned int puzzle[N][N],
                                     unsigned int uiRow, unsigned int uiCol);
static unsigned char is_in_group_valid(unsigned int uiValue, unsigned int puzzle[N][N],
                                       unsigned int uiRow, unsigned int uiCol);
static void increase_recursion_counter(void);
static unsigned char get_first_free_element(unsigned int puzzle[N][N],
                                            unsigned int* puiRow, unsigned int* puiCol);
static unsigned char validate_9_by_9_puzzle(unsigned int puzzle[N][N]);
static unsigned char solve_puzzle(unsigned int puzzle[N][N]);

/*****************************************************************/

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

            /* Assign uiValue if valid [1,N] */
            if (uiValue >= 1 && uiValue <= N)
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
                                     unsigned int uiRow, unsigned int uiCol)
{
    unsigned int uiIndex;
    unsigned char ucStatus;

    /* Boundary check */
    if ((uiRow >= N) || (uiCol >= N))
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
                                     unsigned int uiRow, unsigned int uiCol)
{
    unsigned int uiIndex;
    unsigned char ucStatus;

    /* Boundary check */
    if ((uiRow >= N) || (uiCol >= N))
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
                                       unsigned int uiRow, unsigned int uiCol)
{
    /* TODO */
    return E_OK;
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

/* Get the first available element (value=0) */
static unsigned char get_first_free_element(unsigned int puzzle[N][N],
                                            unsigned int* puiRow, unsigned int* puiCol)
{
    unsigned int uiRow, uiCol;
    unsigned char ucStatus;

    /* This function could utilize a stack with pairs (uiRow, uiCol) having */
    /* an element with value=0. However, to tedious to implement in C with  */
    /* no built-in support. Instead, do search from (0,0) in linear         */
    /* time.                                                                */

    ucStatus = E_NOT_OK;
    for (uiRow = 0; uiRow < N; uiRow++)
    {
        for (uiCol = 0; uiCol < N; uiCol++)
        {
            if (0 == puzzle[uiRow][uiCol])
            {
                /* Found available element */
                *puiRow = uiRow;
                *puiCol = uiCol;
                ucStatus = E_OK;
                goto end; /* Forgive me, but better than return E_OK IMO */
            }
        }
    }
    end:
    return ucStatus;
}

/* Function to validate a 9x9 puzzle */
static unsigned char validate_9_by_9_puzzle(unsigned int puzzle[N][N])
{
    unsigned int uiRow, uiCol, uiSum;
    unsigned char ucStatus;

    ucStatus = E_OK;
    /* Check all rows that sum is 45 */
    for (uiRow = 0; uiRow < N; uiRow++)
    {
        uiSum = 0;
        for (uiCol = 0; uiCol < N; uiCol++)
        {
            uiSum += puzzle[uiRow][uiCol];
        }

        if (SUDOKU_SUM != uiSum)
        {
            ucStatus = E_NOT_OK;
            goto end;
        }
    }

    /* Check all coloumns that sum is 45 */
    for (uiCol = 0; uiCol < N; uiCol++)
    {
        uiSum = 0;
        for (uiRow = 0; uiRow < N; uiRow++)
        {
            uiSum += puzzle[uiRow][uiCol];
        }

        if (SUDOKU_SUM != uiSum)
        {
            ucStatus = E_NOT_OK;
            goto end;
        }
    }

    /* Check all that all group with centers in uiRow,uiCol=1,4,7 have sum 45 */
    for (uiRow = 1; uiRow < N; uiRow += 3)
    {
        for (uiCol = 1; uiCol < N; uiCol += 3)
        {
            uiSum = puzzle[uiRow][uiCol] + puzzle[uiRow - 1][uiCol] + puzzle[uiRow + 1][uiCol] + \
                puzzle[uiRow][uiCol - 1] + puzzle[uiRow][uiCol + 1] + puzzle[uiRow - 1][uiCol + 1] + \
                puzzle[uiRow + 1][uiCol - 1] + puzzle[uiRow - 1][uiCol - 1] + puzzle[uiRow + 1][uiCol + 1];
            
            if (SUDOKU_SUM != uiSum)
            {
                ucStatus = E_NOT_OK;
                goto end;
            }
        }
    }

    end:
    return ucStatus;
}

/* Recursive solver */
static unsigned char solve_puzzle(unsigned int puzzle[N][N])
{
    unsigned int uiRow, uiCol, uiValue;
    unsigned char ucStatus;

    /* Inrease counter for every recursion */
    increase_recursion_counter();
    
    /* Find first element with value 0 */
    if (E_OK == get_first_free_element(puzzle, &uiRow, &uiCol)) /* <--- here iteration in uiRow,uiCol takes place */
    {
            /* Test all values from 1..N */
            for (uiValue = 1; uiValue <= N; uiValue++)
            {
                /* Check if uiValue is valid for (uiRow, uiCol) */
                if ((E_OK == is_in_col_valid(uiValue, puzzle, uiRow, uiCol)) &&
                    (E_OK == is_in_row_valid(uiValue, puzzle, uiRow, uiCol)) &&
                    (E_OK == is_in_group_valid(uiValue, puzzle, uiRow, uiCol)))
                {
                    /* Assign possible candidate */
                    puzzle[uiRow][uiCol] = uiValue;
                    if (E_OK != solve_puzzle(puzzle))
                    {
                        /* Did not solve puzzle */
                        puzzle[uiRow][uiCol] = 0;
                    }
                }
                else
                {
                    /* TODO */
                }
            }
    }
    else
    {
        /* No zeros found, we are done */
        ucStatus = E_OK;
    }
    return ucStatus;
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

    /* Invoke recursive puzzle solver */
    (void)solve_puzzle(puzzle);
    
    /* Print the (hopefully) solved puzzle */
    printf("\n");
    print_puzzle(puzzle);

    printf("\n");
    printf("Number of recursions needed: %d\n", recursion_ctr.remainder);

    /* Check that puzzle have correct sums */
    if (E_OK == validate_9_by_9_puzzle(puzzle))
    {
        printf("\n");
        printf("Puzzle validated SUCCESSFUL.\n");
    }
    else
    {
        printf("\n");
        printf("Puzzle validated FAILED.\n");
    }

    printf("\n");
    printf("Program END.\n");
    printf("\n");
    printf("Press ENTER key to Continue\n");
    printf("\n");
    getchar();

    return 1;
}
