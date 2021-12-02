#include <string.h>

#include "rudolf.h"

char* input(int year, int day)
{
    return rudolf_get_input(year, day);
}

char* solve(int year, int day, int64_t solution)
{
    solution++;
    return rudolf_get_input(year, day);
}

void print_usage()
{
    printf("Usage: rudolf command [arguments]\n");
    printf("rudolf input YEAR DAY\t\tPrint puzzle input for given year and day\n");
    printf("rudolf solve YEAR DAY \t\tSubmit puzzle solution from stdin to Advent of Code website\n");
}

int main(int argc, char* argv[])
{
    if (argc < 2) {
        print_usage();
        return 0;
    }

    if (strcmp(argv[1], "input") == 0) {
        printf("input\n");
    } else if (0) {

    } else {
        print_usage();
    }

    return 0;
}