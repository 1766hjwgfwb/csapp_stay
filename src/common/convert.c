#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include<string.h>
#include<headers/common.h>


// * convert uint32_t to float
uint32_t uint2float(uint32_t u) {
    return 0x0;
}


// * convert string to uint64
uint64_t string2uint(const char* str) {
    return string2uint_range(str, 0, -1);
}

// * @brief DFA
uint64_t string2uint_range(const char* str, int start, int end) {
    // start: starting index inclusive
    // end: ending index includsive
    end = (end == -1) ? strlen(str) - 1 : end;
    uint64_t uv = 0;
    int sign_bit = 0; // 0 - positive; 1 - negative
    // DFA: deterministic finite automata to scan string and get value
    int state = 0;
    
    for (int i = start; i <= end; ++ i) {
        char c = str[i];
        if (state == 0) {
            if (c == '0') {
                state = 1;
                uv = 0;
                continue;
            }
            else if ('1' <= c && c <= '9') {
                state = 2;
                uv = c - '0';
                continue;
            }
            else if (c == '-') {
                state = 3;
                sign_bit = 1;
                continue;
            }
            else if (c == ' ') {
                state = 0;
                continue;
            }
            else
                goto fail;
        }
        else if (state == 1) {
            if ('0' <= c && c <= '9') {
                state = 2;
                uv = uv * 10 + c - '0';
                continue;
            }
            else if (c == 'x') {
                state = 4;
                continue;
            }
            else if (c == ' ') {
                state = 6;
                continue;
            }
            else 
                goto fail;
        }
        else if (state == 2) {
            if ('0' <= c && c <= '9') {
                state = 2;
                uint64_t pv = uv;
                uv = uv * 10 + c - '0';
                // maybe overflow
                if (pv > uv) {
                    printf("(uint64_t)%s overflow: cannot convert\n", str);
                    goto fail;
                }
                continue;
            }
            else if (c == ' ') {
                state = 6;
                continue;
            }
            else 
                goto fail;
        }
        else if (state == 3) {
            if (c == '0') {
                state = 1;
                continue;
            }
            else if ('1' <= c && c <= '9') {
                state = 2;
                uv = c - '0';
                continue;
            }
            else
                goto fail;
        }
        else if (state == 4) {
            if ('0' <= c && c <= '9') {
                state = 5;
                uv = uv * 16 + c - '0';
                continue;
            }
            else if ('a' <= c && c <= 'f') {
                state = 5;
                uv = uv * 16 + c - 'a' + 10;
                continue;
            }
            else
                goto fail;
        }
        else if (state == 5) {
            // hex
            if ('0' <= c && c <= '9') {
                state = 5;
                uint64_t pv = uv;
                uv = uv * 16 + c - '0';
                // maybe overflow
                if (pv > uv) {
                    printf("(uint64_t)%s overflow: cannot convert\n", str);
                    goto fail;
                }
                continue;
            }
            else if ('a' <= c && c <= 'f') {
                state = 5;
                uint64_t pv = uv;
                uv = uv * 16 + c - 'a' + 10;
                // maybe overflow
                if (pv > uv) {
                    printf("(uint64_t)%s overflow: cannot convert\n", str);
                    goto fail;
                }
                continue;
            }
            else
                goto fail;
        }
        else if (state == 6) {
            if (c == ' ') {
                state = 6;
                continue;
            }
            else
                goto fail;
        }
    }
    if (sign_bit == 0)
        return uv;
    else if (sign_bit == 1) {
        if ((uv & 0x8000000000000000) != 0) {
            printf("(int64_t)%s: signed overflow: cannot convert\n", str);
            exit(0);
        }
        int64_t sv = -1 * (int64_t)uv;
        return *((uint64_t *)&sv);
    }

    fail:
    printf("type converter: <%s> cannot be converted to integer\n", str);
    exit(0);
}