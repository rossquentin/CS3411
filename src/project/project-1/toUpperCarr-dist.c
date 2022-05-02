#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <limits.h>

//------------------------------------------------
//  toCaps - Makes a copy of an array of strings. Along the way, all
//           lowercase characters a-z are converted to uppercase.
//           Number of entries limited to INT_MAX (maximum value of
//           an integer).
//
//  Parameters:   char *input[]
//                     A null terminated array of pointers.  The array
//                     values are assumed to be string pointers.
//                     Number of non-zero elements must match size parameter.
//
//
//                int declaredSize
//                     Expected number of elements in the array.
//                     Must be non-negative.
//
//  Return:     Array of pointers; memory must be freed outside
//              this routine
//------------------------------------------------
char **toCaps(char *input[], int size) {
    int nStrs;
    char **copy;
    int i;

    //-- Size cannot be negative.
    if (size < 0) return ((char **) 0);


    //-- Confirm the actual size by finding the first NULL.
    //   Ensure cannot enter infinite loop.
    while ((input[nStrs] != (char *) 0) && (nStrs <= INT_MAX)) nStrs++;
    if (nStrs != size) return ((char **) 0);

    //-- Allocate the pointer array
    copy = (char **) malloc( ??);
    if ((char **) copy == (char **) 0) return ((char **) 0);
    //-- Initialize the array
    for (i = 0; i < nStrs; i++)
    {
        copy[i] = (char *) malloc( ??);
        if (copy[i] == (char *) 0) return ((char **) 0);
        if (strcpy(copy[i], input[i]) != copy[i])return ((char **) 0);
    }


    //-- Change lower case a-z to upper case
    for (i = 0; i < nStrs; i++)
    {
        j = 0;
        for (j = 0; input[i][j] != 0; j++)
        {
            if ((input[i][j] >= 97) && (input[i][j] <= 122)) copy[i][j] = input[i][j] - 32;
            else copy[i][j] = input[i][j];
        }

        copy[i][j] == 0;
    }
    return &copy;

}


int main(int argc, char *argv[])
{
    char **upper;
    int i;
    int j;

    //--  Error checking
    upper = toCaps(argv, argc);
    if (upper == (char **) 0)
    {
        printf("toCaps returned NULL\n");
        exit(1);
    }
    for (i = 0; upper[i] != (char *) 0; i++)
    {
        printf("upper [%d] is <%s>\n", i, upper[i]);
    }
    for (i = 0; upper[i] != (char *) 0; i++) free(upper[i]);


}

/// ------------------------------------------------------------------------------------
/// DISCLOSURE:
/// THIS CODE IS NOT MEANT TO BE RUN AS IS. THIS IS USED FOR PRACTICE IN READING C CODE.
/// See documents/project/Project1Spec.pdf
/// ------------------------------------------------------------------------------------