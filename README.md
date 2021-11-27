# rudolf

Little helper library to retrieve puzzle input from [Advent of Code](https://adventofcode.com/) and store it in a local SQLite database. Depends on [libcurl](https://curl.se/) and [libsqlite3](https://sqlite.org/).

## Usage

```c
#include "rudolf.h"

int main()
{
    char* input = rudolf_get_input(2020, 1);
    if (input) {
        printf("%s", input);
        free(input);
    }

    return 0;
}
```

You must store your Advent of Code session cookie as `cookie.txt` (in the [Netscape cookie file format](https://curl.se/docs/http-cookies.html)) in the same directory as your executable.

