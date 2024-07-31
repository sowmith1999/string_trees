// https://db.in.tum.de/~freitag/papers/p29-neumann-cidr20.pdf
// This is string representation from the above paper, and is also used in cedar db
// https://cedardb.com/blog/german_strings/

#include <cstddef>
#include <cstring>
#include <iostream>
#include <ostream>
#include <stdint.h>

constexpr std::size_t SHORT_MAX = 12;

void print_char_array(const char* arr, size_t length) { fwrite(arr, sizeof(char), length, stdout); }

typedef struct {
    uint32_t length; // 4 bytes for length
    union {
        struct {
            char data[12]; // 12 bytes for short string data
        } short_str;
        struct {
            char prefix[4];      // 4 bytes for prefix
            const char* pointer; // 8 bytes for pointer/offset
        } long_str;
    } content;
} uString;

void init_uString(uString* str, const char* data) {
    std::size_t length = strlen(data);
    str->length = length;
    std::cout << "the length is " << length << std::endl;

    if (length <= SHORT_MAX) {
        memcpy(str->content.short_str.data, data, length);
    } else {
        memcpy(str->content.long_str.prefix, data, 4);
        str->content.long_str.pointer = data;
    }
}

void print_uString(uString* str) {
    if (str->length <= 12) {
        std::cout << str->content.short_str.data << std::endl;
    } else {
        std::cout << "Prefix: " << str->content.long_str.prefix << std::endl;
        print_char_array(str->content.long_str.pointer, str->length);
    }
}

int main() {
    const char arr[] = "Welcome World!";
    // const char arr[] = "Hello!";
    uString* str = (uString*)malloc(sizeof(uString));
    init_uString(str, arr);
    print_uString(str);
    free(str);
    return 0;
}
