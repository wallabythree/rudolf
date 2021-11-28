# rudolf

Little helper library for [Advent of Code](https://adventofcode.com/) in C. Requires [libcurl](https://curl.se/) and [libsqlite3](https://sqlite.org/).

## Features

- `char* rudolf_get_input(int year, int day)`: retrieve puzzle input from https://adventofcode.com. Results are cached in a local SQLite database.
- `int rudolf_split(char*** dest, char* input, const char* delimiters, size_t* count)`: split a string into an array of substrings with the given delimiters. Useful for splitting puzzle input into rows.
- `timed_t rudolf_time_fn(int64_t (*fn)(char*), char* input)`: record return value and execution time of a function with return type `int64_t` and a `char*` parameter in seconds.

## Usage

```c
#include <stdio.h>

#include "rudolf.h"

int main()
{
    // get puzzle input as one long string
    char* input = rudolf_get_input(2020, 8);
    if (!input) {
        return 1;
    }
    printf("%s", input);

    // split puzzle input into rows, then free input
    char** rows = NULL;
    size_t size = 0;
    rudolf_split(&rows, input, "\n", &size);
    free(input);

    // print and free each row, then free the rows array
    for (size_t i = 0; i < size; i++) {
        printf("%lu:\t%s\n", i, rows[i]);
        free(rows[i]);
    }
    free(rows);

    return 0;
}
```

## Configuration

You must store your Advent of Code session cookie as `cookie.txt` (in the [Netscape cookie file format](https://curl.se/docs/http-cookies.html)) in the working directory for the API query to work.
