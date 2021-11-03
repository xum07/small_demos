#ifndef CLION_UTILS_H
#define CLION_UTILS_H

#define PRINT_INFO(format, ...) cli_print("INFO", __FILE__, __LINE__, format, ##__VA_ARGS__)
#define PRINT_WARNING(format, ...) cli_print("WARNING",  __FILE__, __LINE__, format, ##__VA_ARGS__)
#define PRINT_ERROR(format, ...) cli_print("ERROR",  __FILE__, __LINE__, format, ##__VA_ARGS__)

#ifdef __cplusplus
extern "C" {
#endif

void cli_print(const char *errLevel, const char *file, size_t line, const char *format, ...);

#ifdef __cplusplus
}
#endif

#endif //CLION_UTILS_H
