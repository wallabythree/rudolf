#pragma once

#include <string.h>

/**
  * @brief Retrieve puzzle input for given year and day
  * 
  * @param year
  * @param day
  * @return Pointer to char buffer containing puzzle input
  */ 
char* rudolf_get_input(int year, int day);

/**
 * @brief Split a string into an array of substrings with given delimiters
 * 
 * @param dest Where to store pointer to array of substrings
 * @param input Pointer to input string
 * @param delimiters Pointer to const string with delimiters
 * @param count Where to store array size
 * @returns Array of substrings
 */
int rudolf_split(
    char*** dest,
    char* input,
    const char* delimiters,
    size_t* count
);

/**
 * @brief Measure execution time of a function without parameters
 * 
 * @param fn Pointer to function
 * @return double Execution time in seconds
 */
double rudolf_time_fn(void (*fn)());
