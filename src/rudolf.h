#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include <sqlite3.h>
#include <curl/curl.h>

#include "rudolf.h"

#define DB_NAME "rudolf.db"
#define BASE_URL "https://adventofcode.com/%d/day/%d/input"
#define COOKIEJAR "cookie.txt"

typedef struct timed_t {
    int64_t value;
    double time;
} timed_t;

typedef struct Response {
    CURLcode code;
    char* data;
    size_t size;
} Response;

/**
  * @brief Callback to store data received by CURL in a single char buffer
  *
  * Based on example provided in CURLOPT_WRITEFUNCTION documentation available
  * at https://curl.se/libcurl/c/CURLOPT_WRITEFUNCTION.html.
  *
  * @param data Pointer to CURL received data
  * @param size Always 1 (see CURL_WRITEFUNCTION documentation)
  * @param nmemb Size of CURL received data in bytes
  * @param userdata Destination for callback output, set by CURLOPT_WRITEDATA
  * @return Bytes processed by callback (equal to nmemb, unless error)
  */
static size_t write_cb(char* data, size_t size, size_t nmemb, void* userdata)
{
    size *= nmemb;
    Response* res = (Response*) userdata;

    char* new_data = realloc(res->data, res->size + size);
    if (!new_data) {
        return 0;
    }

    memcpy(new_data + res->size, data, size);
    res->data = new_data;
    res->size += size;

    return size;
}

/**
  * @brief Get puzzle input from Advent of Code web API
  *
  * @param year
  * @param day
  * @return Pointer to char buffer with puzzle input or NULL on error
  */
static char* api_get_input(int year, int day)
{
    CURL* curl;

    curl = curl_easy_init();
    if (!curl) {
        return NULL;
    }

    // construct URL to query from base_url and the given year and day
    size_t url_length = snprintf(NULL, 0, BASE_URL, year, day) + 1;
    char* url = malloc(url_length);
    snprintf(url, url_length, BASE_URL, year, day);

    Response res = { 0 }; 
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*) &res);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &write_cb);
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_COOKIEFILE, COOKIEJAR);
    
    res.code = curl_easy_perform(curl);
    if (res.code != CURLE_OK) {
        fprintf(
            stderr, 
            "curl_easy_perform() failed: %s\n",
            curl_easy_strerror(res.code)
        );
    }
    long http_code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
    
    free(url);
    curl_easy_cleanup(curl);

    if (http_code != 200) {
        fprintf(
            stderr, 
            "HTTP code %ld received: %s\n",
            http_code,
            res.data
        );
        free(res.data);
        return NULL;
    }

    // add null terminator to response data
    char* input = realloc(res.data, res.size + 1);
    if (!input) {
        free(res.data);
        return NULL;
    }
    input[res.size] = 0;

    return input;
}

/**
 * @brief Initialise database conection
 * 
 * @param db Where to store sqlite3 pointer
 * @return int SQLite3 status
 */
static int db_init(sqlite3** db)
{
    int rc = sqlite3_open(DB_NAME, db);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(*db));
        sqlite3_close(*db);
    }

    return rc;
}

/**
 * @brief Close database connection
 * 
 * @param db Pointer to sqlite3 struct
 * @return int SQLite3 status
 */
static int db_close(sqlite3* db)
{
    return sqlite3_close(db);
}

/**
  * @brief Retrieve puzzle input from local database
  * 
  * @param year
  * @param day
  * @returns Pointer to char buffer containing puzzle input, or NULL on failure
  */
static char* db_get_input(sqlite3* db, int year, int day)
{
    const char query[] = "SELECT input FROM puzzles WHERE year = ? AND day = ?";
    sqlite3_stmt* res;

    int rc = sqlite3_prepare_v2(db, query, -1, &res, 0);    
    if (rc != SQLITE_OK) {
        return NULL;
    }

    sqlite3_bind_int(res, 1, year);
    sqlite3_bind_int(res, 2, day);

    rc = sqlite3_step(res);

    char* input = NULL;
    if (rc == SQLITE_ROW) {
        const char* value = (const char*) sqlite3_column_blob(res, 0);
        input = calloc(strlen(value) + 1, sizeof(char));
        memcpy(input, value, strlen(value));
    }
    
    sqlite3_finalize(res);

    return input;
}

/**
  * @brief Store puzzle input in local database
  * 
  * @param year
  * @param day
  * @param input Pointer to char buffer containing puzzle input
  */ 
static int db_put_input(sqlite3* db, int year, int day, char* input)
{
    const char table_query[] = "CREATE TABLE IF NOT EXISTS puzzles (\
        year INTEGER NOT NULL,\
        day INTEGER NOT NULL,\
        input BLOB NOT NULL,\
        PRIMARY KEY (year, day)\
    )";

    sqlite3_stmt* res;
    int rc = sqlite3_prepare_v2(db, table_query, -1, &res, 0);    
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to create table: %s\n", sqlite3_errmsg(db));
        return 1;
    }
    
    rc = sqlite3_step(res);
    sqlite3_finalize(res);

    char insert_query[] = "INSERT INTO puzzles (\
            year,\
            day,\
            input\
        ) VALUES (\
            ?,\
            ?,\
            ?\
        )\
    ";

    rc = sqlite3_prepare_v2(db, insert_query, -1, &res, 0);    
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to insert record: %s\n", sqlite3_errmsg(db));
        return 1;
    }

    sqlite3_bind_int(res, 1, year);
    sqlite3_bind_int(res, 2, day);
    sqlite3_bind_blob(res, 3, (const void*) input, -1, SQLITE_STATIC);

    rc = sqlite3_step(res);

    sqlite3_finalize(res);

    return 0;
}

/**
  * @brief Retrieve puzzle input for given year and day
  * 
  * @param year
  * @param day
  * @return Pointer to char buffer containing puzzle input
  */
char* rudolf_get_input(int year, int day)
{
    sqlite3* db;
    int db_status = db_init(&db);
    if (db_status != 0) {
        return NULL;
    }

    char* data = db_get_input(db, year, day);
    if (!data) {
        data = api_get_input(year, day);
        if (data) {
            db_put_input(db, year, day, data);
        }
    }

    db_close(db);

    return data;
}

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
)
{
    *dest = NULL;
    *count = 0;

    if (input == NULL) {
        return 1;
    }

    // count how many times delimiters occur in input
    size_t len = strlen(input);
    size_t offset = 0;
    while (offset < len) {
        offset = offset + strcspn(input + offset, delimiters) + 1;
        (*count)++;
    }

    char** strings = malloc(*count * sizeof(char*));
    if (!strings) {
        return 1;
    }

    offset = 0;
    for (size_t i = 0; i < *count; i++) {
        size_t len = strcspn(input + offset, delimiters);
        strings[i] = calloc(len + 1, sizeof(char));
        memcpy(strings[i], input + offset, len);
        offset = offset + len + 1;
    }
    *dest = strings;

    return 1;
}

/**
 * @brief Measure execution time of a int64_t function with an input_t parameter
 * 
 * @param fn Pointer to function
 * @param fn Function parameter
 * @return double Execution time in seconds
 */
timed_t* rudolf_time_fn(int64_t (*fn)(char*), char* input)
{
    timed_t* result = calloc(1, sizeof(timed_t));
    if (!result) {
        return NULL;
    }

    clock_t tick = clock();
    result->value = fn(input);
    clock_t tock = clock();
    result->time = (double) (tock - tick) / CLOCKS_PER_SEC;

    return result;
}
