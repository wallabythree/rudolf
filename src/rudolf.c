#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <curl/curl.h>

typedef struct Response {
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

size_t write_callback(char* data, size_t size, size_t nmemb, void* userdata)
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

int main()
{
    CURL* curl;

    curl = curl_easy_init();
    if (!curl) {
        return 1;
    }

    Response res = { 0 }; 
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*) &res);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &write_callback);
    curl_easy_setopt(curl, CURLOPT_URL, "https://ifconfig.io");
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    
    CURLcode res_code = curl_easy_perform(curl);

    if (res_code != CURLE_OK) {
        fprintf(
            stderr, 
            "curl_easy_perform() failed: %s\n",
            curl_easy_strerror(res_code)
        );
    }

    for (size_t i = 0; i < res.size; i++) {
        fprintf(stdout, "%c", res.data[i]);
    }

    if (res.data) {
        free(res.data);
    }

    curl_easy_cleanup(curl);

    return 0;
}

