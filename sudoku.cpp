// sudoku.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>
#include <time.h>

/******************* DEFINES ******************/

#define SUDOKU_SUM 45 /* sum of row, col or group */

/* Status codes */
#define E_OK             0x01
#define E_NOT_OK         0x00
#define E_MAX_RECURSIONS 0x02
#define E_INDEX_ERROR    0xFF

/* User defined */
#define GNU_LINUX           0
#define N                   9 /* size of puzzle */
#define MAX_NBR_RECURSIONS  1000000 /* Needs to be set 'fair' since otherwise we spend to much time in recursion */

/* Input from Vaderlinds Stora Sudoku boken. */
//#define FILE_NAME          "vaderlinds1.txt"       /* Level 1*/
//#define FILE_NAME          "vaderlinds30.txt"      /* Level 2*/
//#define FILE_NAME          "vaderlinds70.txt"      /* Level 3 */
//#define FILE_NAME          "vaderlinds116.txt"     /* Level 4 */
//#define FILE_NAME          "vaderlinds150.txt"     /* Level 5*/
//#define FILE_NAME          "vaderlinds180.txt"     /* Level 6*/
#define FILE_NAME          "vaderlinds190.txt"     /* Level 7 */

/* Using input-files above and plotting in Matlab, excluding */
/* values that 'stick' out (probably well suited order for   */
/* the recursive procedure). At around 55000 the curve goes  */
/* balistic (upper limits).                                  */
#define LEVEL_LOW    5000
#define LEVEL_MEDIUM 10000
#define LEVEL_HARD   55000

#define NEW_PUZZLE_NBR_CLUES 20

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
static unsigned char solve_puzzle_increment(unsigned int puzzle[N][N]);
static unsigned char solve_puzzle_decrement(unsigned int puzzle[N][N]);
static unsigned char is_equal(unsigned int puzzle[N][N],
                              unsigned int puzzlecopy[N][N]);

/*****************************************************************/

/* Reading the puzzle from FILE_NAME */
static unsigned char read_puzzle_from_txt( unsigned int puzzle[N][N] )
{
    unsigned int uiRow, uiCol, uiValue;
    char cValue = 0;
    FILE* fhPuzzle;
    
#if ( GNU_LINUX == 1 )
    fhPuzzle = fopen(FILE_NAME, "r");
#else
    fopen_s(&fhPuzzle, FILE_NAME, "r");
#endif

    uiRow = 0;
    uiCol = 0;
    if (NULL != fhPuzzle)
    {
        while (cValue != EOF && uiRow < N)
        {
            cValue = getc(fhPuzzle);

            /* Check values newline & EOF */
            if (('\n' == cValue) || (EOF == cValue))
            {
                /* Only allowed at N */
                if (N != uiCol && uiRow < N)
                {
                    return E_NOT_OK;
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
    return E_OK;
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

    /* Boundary check */
    if ((uiRow >= N) || (uiCol >= N))
    {
        return E_INDEX_ERROR;
    }
    
    for (uiIndex = 0; uiIndex < N; uiIndex++)
    {
        /* Coloumn is running index */
        if (uiIndex != uiCol)
        {
            if (uiValue == puzzle[uiRow][uiIndex])
            {
                return E_NOT_OK;
            }
        }
    }

    return E_OK;
}

/* Check if provided uiValue is already present in uiCol. */
static unsigned char is_in_col_valid(unsigned int uiValue, unsigned int puzzle[N][N],
                                     unsigned int uiRow, unsigned int uiCol)
{
    unsigned int uiIndex;

    /* Boundary check */
    if ((uiRow >= N) || (uiCol >= N))
    {
        return E_INDEX_ERROR;
    }

    for (uiIndex = 0; uiIndex < N; uiIndex++)
    {
        /* Row is running index */
        if (uiIndex != uiRow)
        {
            if (uiValue == puzzle[uiIndex][uiCol])
            {
                return E_NOT_OK;
            }
        }
    }

    return E_OK;
}

/* Check if provided uiValue is already present in group belonging to */
/* in which uiRow and uiCol is present.                               */
static unsigned char is_in_group_valid(unsigned int uiValue, unsigned int puzzle[N][N],
                                       unsigned int uiRow, unsigned int uiCol)
{
    unsigned int uiRowStart, uiColStart, uiRowIndex, uiColIndex;

    /* Boundary check */
    if ((uiRow >= N) || (uiCol >= N))
    {
        return E_INDEX_ERROR;
    }

    /* Find row and col start, e.g. group2 starts at col=3 and ends col=5 */
    /* and 3 mod 3 = 0, 4 mod 3 = 1, 5 mod 3 = 2. Use this to find start. */
    uiRowStart = uiRow - uiRow % 3;
    uiColStart = uiCol - uiCol % 3;

    for (uiRowIndex = uiRowStart; uiRowIndex < (uiRowStart + 3); uiRowIndex++)
    {
        for (uiColIndex = uiColStart; uiColIndex < (uiColStart + 3); uiColIndex++)
        {
            if (uiValue == puzzle[uiRowIndex][uiColIndex])
            {
                return E_NOT_OK;
            }
        }
    }

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

    /* This function could utilize a stack with pairs (uiRow, uiCol) having */
    /* an element with value=0. However, to tedious to implement in C with  */
    /* no built-in support. Instead, do search from (0,0) in linear         */
    /* time.                                                                */

    for (uiRow = 0; uiRow < N; uiRow++)
    {
        for (uiCol = 0; uiCol < N; uiCol++)
        {
            if (0 == puzzle[uiRow][uiCol])
            {
                /* Found available element */
                *puiRow = uiRow;
                *puiCol = uiCol;
                return E_OK;
            }
        }
    }
    return E_NOT_OK;
}

/* Function to validate a 9x9 puzzle */
static unsigned char validate_9_by_9_puzzle(unsigned int puzzle[N][N])
{
    unsigned int uiRow, uiCol, uiSum;

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
            return E_NOT_OK;
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
            return E_NOT_OK;
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
                return E_NOT_OK;
            }
        }
    }

    return E_OK;
}

/* Recursive solver - find first OR unique solution. I.e. if     */
/* first found number will be assigned e.g. 3 and a solution is  */
/* found then we don't know if some number in 4-9 would generate */
/* a correct solution. This can be achieved by running uiValue   */
/* for-loop from N->1 instead. If the same solution is achieved  */
/* solution is unique.                                           */
static unsigned char solve_puzzle_increment(unsigned int puzzle[N][N])
{
    unsigned int uiRow, uiCol, uiValue;

    /* Inrease counter for every recursion */
    increase_recursion_counter();

    /* Check that max number of recursions not exceeded */
    if (recursion_ctr.remainder > MAX_NBR_RECURSIONS)
    {
        return E_MAX_RECURSIONS;
    }
    
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
                    if (E_OK == solve_puzzle_increment(puzzle))
                    {
                        /* Puzzle solved, we are done. This status comes  */
                        /* from get_first_free_element() in one-step-down */
                        /* call to solve_puzzle() when no more zeros have */
                        /* been found.                                    */
                        return E_OK;
                    }
                    else
                    {
                        /* Did not solve puzzle */
                        puzzle[uiRow][uiCol] = 0;
                    }
                }
                else
                {
                    /* Do nothing */
                }
            }
    }
    else
    {
        /* No zeros found, we are done */
        return E_OK;
    }
    /* Puzzle not yet solved, "rolling back" since */
    /* we set puzzle[uiRow][uiCol] = 0             */
    return E_NOT_OK;
}

/* Solved puzzle by decrementing uiValue */
static unsigned char solve_puzzle_decrement(unsigned int puzzle[N][N])
{
    unsigned int uiRow, uiCol, uiValue;

    /* Inrease counter for every recursion */
    increase_recursion_counter();

    /* Check that max number of recursions not exceeded */
    if (recursion_ctr.remainder > MAX_NBR_RECURSIONS)
    {
        return E_MAX_RECURSIONS;
    }

    /* Find first element with value 0 */
    if (E_OK == get_first_free_element(puzzle, &uiRow, &uiCol)) /* <--- here iteration in uiRow,uiCol takes place */
    {
        /* Test all values from N..1 */
        for (uiValue = N; uiValue >= 1; uiValue--)
        {
            /* Check if uiValue is valid for (uiRow, uiCol) */
            if ((E_OK == is_in_col_valid(uiValue, puzzle, uiRow, uiCol)) &&
                (E_OK == is_in_row_valid(uiValue, puzzle, uiRow, uiCol)) &&
                (E_OK == is_in_group_valid(uiValue, puzzle, uiRow, uiCol)))
            {
                /* Assign possible candidate */
                puzzle[uiRow][uiCol] = uiValue;
                if (E_OK == solve_puzzle_decrement(puzzle))
                {
                    /* Puzzle solved, we are done. This status comes  */
                    /* from get_first_free_element() in one-step-down */
                    /* call to solve_puzzle() when no more zeros have */
                    /* been found.                                    */
                    return E_OK;
                }
                else
                {
                    /* Did not solve puzzle */
                    puzzle[uiRow][uiCol] = 0;
                }
            }
            else
            {
                /* Do nothing */
            }
        }
    }
    else
    {
        /* No zeros found, we are done */
        return E_OK;
    }
    /* Puzzle not yet solved, "rolling back" since */
    /* we set puzzle[uiRow][uiCol] = 0         */
    return E_NOT_OK;
}

/* Check if puzzles are the same */
static unsigned char is_equal(unsigned int puzzle[N][N],
                              unsigned int puzzlecopy[N][N])
{
    unsigned int uiRow, uiCol;
    for (uiRow = 0; uiRow < N; uiRow++)
    {
        for (uiCol = 0; uiCol < N; uiCol++)
        {
            if (puzzlecopy[uiRow][uiCol] != puzzle[uiRow][uiCol])
            {
                return E_NOT_OK;
            }
        }
    }
    return E_OK;
}

int main()
{
    unsigned int puzzle[N][N] = { 0 }; /* puzzle[row][col] = puzzle[y][x] */
    unsigned int puzzlecopy[N][N] = { 0 };
    unsigned int newpuzzle_increment[N][N] = {0};
    unsigned int newpuzzle_decrement[N][N] = {0};
    unsigned int newpuzzlecopy[N][N] = {0};
    unsigned char ucStatus = E_NOT_OK;
    unsigned char ucUniqueSolution;
    unsigned int uiRow, uiCol, uiValue, uiIterations;
    time_t t;

    /* Init recursion counter */
    recursion_ctr.remainder = 0;
    recursion_ctr.integer_nbr = 1;

    printf("\n");
    printf("Program START...\n");
    printf("\n");

    /************** PART 1 ***************/

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
        return 1;
    }

    /* Print the puzzle */
    print_puzzle(puzzle);

    /* Copy puzzle to puzzlecopy */
    memcpy(puzzlecopy, puzzle, N * N * sizeof(unsigned int));

    printf("\n");
    printf("Press ENTER key to Continue\n");
    getchar();
    /************** PART 2 ***************/

    printf("\n");
    printf("               PART 2 - Solving the puzzle.\n");

    /* Invoke recursive puzzle solver */
    ucStatus = solve_puzzle_increment(puzzle);
    if (E_NOT_OK == ucStatus)
    {
        printf("\n");
        printf(" - RESULT: Puzzle could not be solved.\n");
        return 1;
    }
    else if (E_MAX_RECURSIONS == ucStatus)
    {
        printf("\n");
        printf(" - RESULT: Maximum numbers of recusions reached when solving puzzle.\n");
        return 1;
    }
    else
    {
        printf("\n");
        printf(" - RESULT: Puzzle solved SUCCESSFUL (incremental).\n");
        printf("\n");
        printf("\n");
        print_puzzle(puzzle);
    }

    printf("\n");
    printf(" - Number of recursions needed: %lu\n", recursion_ctr.remainder);
    printf("\n");
    printf(" - Assumption: The complexity is a measure of how hard the puzzle is.\n");
    if (recursion_ctr.remainder < LEVEL_LOW)
    {
        printf("\n");
        printf(" - The level of the puzzle is considered LOW.\n");
    }
    else if (recursion_ctr.remainder < LEVEL_MEDIUM)
    {
        printf("\n");
        printf(" - The level of the puzzle is considered MEDIUM.\n");
    }
    else if (recursion_ctr.remainder < LEVEL_HARD)
    {
        printf("\n");
        printf(" - The level of the puzzle is considered HARD.\n");
    }
    else
    {
        printf("\n");
        printf(" - The level of the puzzle is considered SAMURAI.\n");
    }

    /* Check that puzzle have correct sums */
    if (E_OK == validate_9_by_9_puzzle(puzzle))
    {
        printf("\n");
        printf(" - RESULT: Puzzle validated SUCCESSFUL (test of validation function).\n");
    }
    else
    {
        printf("\n");
        printf(" - RESULT: Puzzle validated FAILED (test of validation function).\n");
    }

    /* Solve by decrementing */

    printf("\n");
    printf(" - Start to verify if solution is unique...\n");
    printf("\n");
    printf(" - The Puzzle was solved SUCCESSFUL (decremental).\n");
    ucStatus = solve_puzzle_decrement(puzzlecopy);
    printf("\n");
    print_puzzle(puzzlecopy);

    if (E_OK == ucStatus)
    {
        if (E_OK == is_equal(puzzle, puzzlecopy))
        {
            printf("\n");
            printf(" - RESULT: Verified unique solution.\n");
        }
        else
        {
            printf("\n");
            printf(" - RESULT: Other solutions exist.\n");
        }
    }
    else
    {
        printf("\n");
        printf(" - RESULT: It was not possible to solve in decrementing order.\n");
    }

    printf("\n");
    printf("Press ENTER key to Continue\n");
    getchar();

    /************** PART 3 ***************/

    /* Try some random placement of numbers, then try to */
    /* solve the puzzle. If fail, try again...           */

    printf("\n");
    printf("               PART 3 - Creating a puzzle.\n");

    printf("\n");
    printf(" - Be patient...\n");

    uiIterations = 0;
    ucStatus = E_NOT_OK;
    ucUniqueSolution = 0;

    srand((unsigned) time(&t));
    while(0 == ucUniqueSolution)
    {
        while( uiIterations < NEW_PUZZLE_NBR_CLUES )
        {
            uiRow = rand() % (N+1);
            uiCol = rand() % (N+1);
            uiValue= rand() % (N+1);

            recursion_ctr.remainder = 0;
            /* Only place if value is zero */
            if( 0 == newpuzzle_increment[uiRow][uiCol] )
            {
                /* Check if uiValue is valid for (uiRow, uiCol) */
                if ((E_OK == is_in_col_valid(uiValue, newpuzzle_increment, uiRow, uiCol)) &&
                    (E_OK == is_in_row_valid(uiValue, newpuzzle_increment, uiRow, uiCol)) &&
                    (E_OK == is_in_group_valid(uiValue, newpuzzle_increment, uiRow, uiCol)))
                {
                    newpuzzle_increment[uiRow][uiCol] = uiValue;
                    newpuzzlecopy[uiRow][uiCol] = uiValue;

                    /* Try to solve the puzzle by incrementing and decrementing */
                    if (E_OK == solve_puzzle_increment(newpuzzle_increment))
                    {
                        uiIterations++;

                        /* Copy puzzlecopy to puzzle */
                        if( uiIterations < NEW_PUZZLE_NBR_CLUES)
                        {
                            memcpy(newpuzzle_increment, newpuzzlecopy, N * N * sizeof(unsigned int));
                        }
                    }
                    else
                    {
                        /* Not possible to solve, reset last assignment */
                        newpuzzle_increment[uiRow][uiCol] = 0;
                        newpuzzlecopy[uiRow][uiCol] = 0;
                    }
                }
            }
        }

        /* Solve puzzle decremental */
        recursion_ctr.remainder = 0;

        /* Set init values for solving decremental */
        memcpy(newpuzzle_decrement, newpuzzlecopy, N * N * sizeof(unsigned int));

        /* Solve puzzle by decrementing */
        (void)solve_puzzle_decrement(newpuzzle_decrement);


        /* Check if solutions are equal */
        if (E_OK == is_equal(newpuzzle_increment, newpuzzle_decrement))
        {
            /* A unique solution has been found */
            ucUniqueSolution = 1;
        }
        else
        {
            /* Reset puzzles */
            uiIterations = 0;
            memset(newpuzzle_increment, 0, N * N*sizeof(unsigned int));
            memset(newpuzzle_decrement, 0, N * N * sizeof(unsigned int));
            memset(newpuzzlecopy, 0, N * N * sizeof(unsigned int));
        }
    }

    printf("\n");
    printf("RESULT: A unique solution has been found.\n");
    printf("\n");
    printf("Number of elements puzzle: %d\n", uiIterations + 1);
    printf("\n");
    print_puzzle(newpuzzlecopy);
    printf("\n");
    print_puzzle(newpuzzle_increment);
    printf("\n");
    printf("Number of recursions needed: %lu\n", recursion_ctr.remainder);

    printf("\n");
    printf("Program END.\n");
    printf("\n");
    printf("Press ENTER key to Continue\n");
    printf("\n");
    getchar();

    return 0;
}
