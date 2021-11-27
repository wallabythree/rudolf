# rudolf

Little helper library to retrieve puzzle input from [Advent of Code](https://adventofcode.com/) and store it in a local [SQLite](https://sqlite.org/) database.

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

