#include <stdlib.h>
#include <stdio.h>

void printRecursiveRangeNumbers(int start, int stop) {
    if (start <= stop) {
        printf("%d-", start);
        printRecursiveRangeNumbers(start + 1, stop);
    }
}

void printGotoRangeNumbers(int start, int stop) {
 topOfFunction:
    if (start <= stop) {
        printf("%d-", start);
        start += 1;
        goto topOfFunction;
    }
}

void printNoRecursionRangeNumbers(int start, int stop) {
    while (start <= stop) {
        printf("%d-", start);
        start += 1;
    }
}


int main(int argc, char *argv[])
{
    (void) argc;
    (void) argv[0];

    printRecursiveRangeNumbers(0, 10);
    putchar('\n');
    printGotoRangeNumbers(0, 10);
    putchar('\n');
    printNoRecursionRangeNumbers(0, 10);
    return 0;
}