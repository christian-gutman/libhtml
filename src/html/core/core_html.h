#ifndef CORE_HTML_H
#define CORE_HTML_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "core.h"

CORE_API int html_entities(const char *src, int src_len, char *target);
CORE_API int decode_uri_component(const char *src, int src_len, char *target);
CORE_API int encode_uri_component(const char *src, int src_len, char *target);

CORE_API void base64_url_encode(const char* input, int input_len, char* output);
CORE_API int base64_url_decode(const char* input, char* output);

#ifdef __cplusplus
}
#endif

#endif // CORE_HTML_H