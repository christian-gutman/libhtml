#include "core_html.h"


int encode_uri_component(const char *src, int src_len, char *target)
{
    if (src == NULL || target == NULL || src_len <= 0)
    {
        if (target != NULL)
        {
            *target = '\0';
        }
        return 0;
    }

    char *p = target;
    const char *hex = "0123456789ABCDEF";

    for (int i = 0; i < src_len; i++)
    {
        unsigned char c = src[i];
        if (isalnum(c) || strchr("-_.!~*'()", c) != NULL)
        {
            *p++ = c;
        }
        else
        {
            *p++ = '%';
            *p++ = hex[c >> 4];
            *p++ = hex[c & 15];
        }
    }
    *p = '\0';
    return (int)(p - target);
}

int decode_uri_component(const char *src, int src_len, char *target)
{
    if (src == NULL || target == NULL || src_len <= 0)
    {
        if (target != NULL)
        {
            *target = '\0';
        }
        return 0;
    }

    char *p = target;

    for (int i = 0; i < src_len; i++)
    {
        if (src[i] == '%' && (i + 2) < src_len && isxdigit((unsigned char)src[i + 1]) && isxdigit((unsigned char)src[i + 2]))
        {
            char hex[3];
            hex[0] = src[i + 1];
            hex[1] = src[i + 2];
            hex[2] = '\0';

            *p++ = (char)strtol(hex, NULL, 16);
            i += 2;
        }
        else
        {
            *p++ = src[i];
        }
    }
    *p = '\0';
    return (int)(p - target);
}

int html_entities(const char *src, int src_len, char *target)
{
    if (src == NULL || target == NULL || src_len <= 0)
    {
        if (target != NULL)
        {
            *target = '\0';
        }
        return 0;
    }

    char *p = target;

    for (int i = 0; i < src_len; i++)
    {
        switch (src[i])
        {
        case '&':
            strcpy(p, "&amp;");
            p += 5;
            break;
        case '<':
            strcpy(p, "&lt;");
            p += 4;
            break;
        case '>':
            strcpy(p, "&gt;");
            p += 4;
            break;
        case '"':
            strcpy(p, "&quot;");
            p += 6;
            break;
        case '\'':
            strcpy(p, "&#39;");
            p += 5;
            break;
        default:
            *p++ = src[i];
            break;
        }
    }
    *p = '\0';
    return (int)(p - target);
}

static const char b64_table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

void base64_url_encode(const char* input, int input_len, char* output) {
    int i = 0, j = 0;
    
    while (i < input_len) {
        unsigned int octet_a = i < input_len ? (unsigned char)input[i++] : 0;
        unsigned int octet_b = i < input_len ? (unsigned char)input[i++] : 0;
        unsigned int octet_c = i < input_len ? (unsigned char)input[i++] : 0;

        unsigned int triple = (octet_a << 16) + (octet_b << 8) + octet_c;

        output[j++] = b64_table[(triple >> 18) & 0x3F];
        output[j++] = b64_table[(triple >> 12) & 0x3F];
        output[j++] = b64_table[(triple >> 6) & 0x3F];
        output[j++] = b64_table[triple & 0x3F];
    }
 
    int mod = input_len % 3;
    if (mod == 1) j -= 2;
    else if (mod == 2) j -= 1;

    output[j] = '\0';
 
    for (int k = 0; k < j; k++) {
        if (output[k] == '+') output[k] = '-';
        else if (output[k] == '/') output[k] = '_';
    }
}
static int get_b64_value(char c) {
    if (c >= 'A' && c <= 'Z') return c - 'A';
    if (c >= 'a' && c <= 'z') return c - 'a' + 26;
    if (c >= '0' && c <= '9') return c - '0' + 52;
    if (c == '+') return 62;
    if (c == '/') return 63;
    return -1;
} 

int base64_url_decode(const char* input, char* output) {
    int input_len = strlen(input);
    if (input_len == 0) return 0; 

    int alloc_len = ((input_len + 3) / 4) * 4;
    char* b64_standard = (char*)malloc(alloc_len + 1);
    if (!b64_standard) return 0;
 
    for (int k = 0; k < input_len; k++) {
        if (input[k] == '-') b64_standard[k] = '+';
        else if (input[k] == '_') b64_standard[k] = '/';
        else b64_standard[k] = input[k];
    }
 
    int j = input_len;
    while (j < alloc_len) {
        b64_standard[j++] = '=';
    }
    b64_standard[alloc_len] = '\0';
 
    int i = 0;
    int out_idx = 0;
    while (i < alloc_len) { 

        char c1 = b64_standard[i++];
        char c2 = b64_standard[i++];
        char c3 = b64_standard[i++];
        char c4 = b64_standard[i++];

        int v1 = get_b64_value(c1);
        int v2 = get_b64_value(c2);
        int v3 = (c3 == '=') ? 0 : get_b64_value(c3);
        int v4 = (c4 == '=') ? 0 : get_b64_value(c4);
 
        unsigned int triple = (v1 << 18) + (v2 << 12) + (v3 << 6) + v4;
 
        //if (out_idx < triple) output[out_idx++] = (triple >> 16) & 0xFF;
        output[out_idx++] = (triple >> 16) & 0xFF; 
        if (c3 != '=') output[out_idx++] = (triple >> 8) & 0xFF;
        if (c4 != '=') output[out_idx++] = triple & 0xFF;
    }

    output[out_idx] = '\0'; 
    
    free(b64_standard);
    return out_idx; 
}

