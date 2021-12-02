# rudolf

Little helper library for [Advent of Code](https://adventofcode.com/) in C. Requires [libcurl](https://curl.se/) and [libsqlite3](https://sqlite.org/).

## Features

- `char* rudolf_get_input(int year, int day)`: retrieve puzzle input from https://adventofcode.com. Results are cached in a local SQLite database.
- `int rudolf_split(char*** dest, char* input, const char* delimiters, size_t* count)`: split a string into an array of substrings with the given delimiters. Useful for splitting puzzle input into rows.
- `timed_t rudolf_time_fn(int64_t (*fn)(char*), char* input)`: record return value and execution time of a function with return type `int64_t` and a `char*` parameter. Execution time is measured in seconds.

## Usage

```c
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "rudolf.h"

#define YEAR 2021
#define DAY 1

/**
 * @brief Example solution. Returns string length.
 * 
 * @param input Char buffer with puzzle input
 * @return int64_t with puzzle answer 
 */
int64_t part1(char* input)
{
    size_t len = 0;
    while (input[len] != 0) {
        len++;
    }
    return len;
}

int main()
{
    char* input = rudolf_get_input(YEAR, DAY);
    timed_t* res1 = rudolf_time_fn(&part1, input);
    free(input);

    printf("Advent of Code %d day %d:\n", YEAR, DAY);
    printf("\tpart1:\t%ld\t%.3f ms\n", res1->value, res1->time * 1000);

    free(res1);

    return 0;
}
```

## Configuration

You must store your Advent of Code session cookie as `cookie.txt` (in the [Netscape cookie file format](https://curl.se/docs/http-cookies.html)) in the working directory for the API query to work.
