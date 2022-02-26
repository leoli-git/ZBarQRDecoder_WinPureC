/**
 * @file main.h
 * @author leoli (jafee201153@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-12-14
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/*  */
#pragma region includes

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#endif

#pragma endregion includes

/*  */
#pragma region typedef

#pragma endregion typedef

/*  */
#pragma region defines

#ifndef MAX_PATH
#define MAX_PATH 260
#endif

/* printf color */
#define NONE                "\033[m"
#define RED                 "\033[0;32;31m"
#define LIGHT_RED           "\033[1;31m"
#define GREEN               "\033[0;32;32m"
#define LIGHT_GREEN         "\033[1;32m"
#define BLUE                "\033[0;32;34m"
#define LIGHT_BLUE          "\033[1;34m"
#define DARY_GRAY           "\033[1;30m"
#define CYAN                "\033[0;36m"
#define LIGHT_CYAN          "\033[1;36m"
#define PURPLE              "\033[0;35m"
#define LIGHT_PURPLE        "\033[1;35m"
#define BROWN               "\033[0;33m"
#define YELLOW              "\033[1;33m"
#define LIGHT_GRAY          "\033[0;37m"
#define WHITE               "\033[1;37m"

#define LOG(fmt, ...)       printf(fmt, ##__VA_ARGS__)
#define LOG_INFO(fmt, ...)  printf(LIGHT_GREEN fmt NONE, ##__VA_ARGS__)
#define LOG_WARN(fmt, ...)  printf(YELLOW fmt NONE, ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...) printf(RED fmt NONE, ##__VA_ARGS__)

#pragma endregion defines

/*  */
#pragma region Exported Functions

#pragma endregion Exported Functions

#ifdef __cplusplus
}
#endif

#endif  // __MAIN_H
