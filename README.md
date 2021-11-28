# rudolf

Little helper library to retrieve puzzle input from [Advent of Code](https://adventofcode.com/) and store it in a local SQLite database. Requires [libcurl](https://curl.se/) and [libsqlite3](https://sqlite.org/).

## Usage

```c
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

You must store your Advent of Code session cookie as `cookie.txt` (in the [Netscape cookie file format](https://curl.se/docs/http-cookies.html)) in the working directory for the API query to work.
