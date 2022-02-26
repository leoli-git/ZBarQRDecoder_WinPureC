/**
 * @file main.c
 * @author leoli (jafee201153@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-12-14
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "main.h"

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

#include "QRSample.h"
#include "bitmap.h"
#include "image.h"
#include "zbar.h"

#pragma endregion includes

/*  */
#pragma region typedef

typedef struct {
    int count;
    const zbar_symbol_t **symbols;
} symbol_set_t;

typedef enum {
    o_help,
    o_version,
    o_test,
    o_file,
    o_cvt,
} option_t;

typedef struct
{
    char *full_name;
    char *short_name;
    option_t option;
    char *description;
} option_info_t;

#pragma endregion typedef

/*  */
#pragma region Private Function Prototypes

symbol_set_t *scan_image_from_file(const char *file);

#pragma endregion Private Function Prototypes

/*  */
#pragma region defines

#pragma endregion defines

/*  */
#pragma region variables

const option_info_t option_info[] = {
    {"help",    "h", o_help,    "display this help and exit"                                          },
    {"version", "v", o_version, "output version information and exit"                                 },
    {"test",    "t", o_test,    "test the program"                                                    },
    {"file",    "f", o_file,    "input file name"                                                     },
    {"cvt",     "c", o_cvt,     "convert bitmap to byte array and write to header file (experimental)"},
};

#pragma endregion variables

/*  */
#pragma region External Functions

#pragma endregion External Functions

/*  */
#pragma region Exported Functions

int main(int argc, char **argv) {
    option_t option = o_help;
    char *pfile     = (char *)malloc(MAX_PATH * sizeof(char));
    if (argc > 1) {
        if (strcasecmp(argv[1], "-h") == 0 || strcasecmp(argv[1], "--help") == 0) {
            option = o_help;
        }
        if (strcasecmp(argv[1], "-v") == 0 || strcasecmp(argv[1], "--version") == 0) {
            option = o_version;
        }
        if (strcasecmp(argv[1], "-t") == 0 || strcasecmp(argv[1], "--test") == 0) {
            option = o_test;
        }
        if (strcasecmp(argv[1], "-f") == 0 || strcasecmp(argv[1], "--file") == 0) {
            option = o_file;
            pfile  = argv[2];
        }
        if (strcasecmp(argv[1], "-c") == 0 || strcasecmp(argv[1], "--cvt") == 0) {
            option = o_cvt;
            pfile  = argv[2];
        }
    } else {
        printf("Please input the full path of the image file: ");
        scanf("%s", pfile);
        option = o_file;
    }

    switch (option) {
        case o_version: {
            unsigned major, minor;
            zbar_version(&major, &minor);
            LOG_INFO("zbar version %d.%d\n", major, minor);
        } break;

        case o_test: {
            LOG_INFO("Run Self Test.\n");
            bitmap_t bitmap = {0, 0, 0, NULL};
            bitmap.width    = QR_SAMPLE_WIDTH;
            bitmap.height   = QR_SAMPLE_HEIGHT;
            bitmap.data     = (unsigned char *)QRSample;
            bitmap.bits     = 8;
            bitmap_write((bitmap_t *)&bitmap, ".\\resource.bmp");
            LOG_INFO("Write QR Sample to %s\n", pfile);
            pfile                    = ".\\resource.bmp";
            symbol_set_t *symbol_set = scan_image_from_file(pfile);
            if (symbol_set) {
                for (int i = 0; i < symbol_set->count; i++) {
                    const zbar_symbol_t *symbol = symbol_set->symbols[i];
                    LOG_INFO("Type: %s\n", zbar_get_symbol_name(zbar_symbol_get_type(symbol)));
                    LOG_INFO("Data Length: %d\n", zbar_symbol_get_data_length(symbol));
                    LOG_INFO("Data: %s\n", zbar_symbol_get_data(symbol));

                    char *str = "134583789727716556";
                    int ret   = 0;
                    for (size_t i = 0; i < min(zbar_symbol_get_data_length(symbol) - 1, 18); i++) {
                        ret = zbar_symbol_get_data(symbol)[i] == str[i] ? 1 : 0;
                    }
                    LOG_INFO("Compare Result: %s\n", ret ? "Success" : "Fail");
                }
            } else {
                LOG_WARN("No symbol found.\n");
            }
            remove(pfile);
        } break;

        case o_file: {
            LOG_INFO("File: %s\n", pfile);
            symbol_set_t *symbol_set = scan_image_from_file(pfile);
            if (symbol_set) {
                for (int i = 0; i < symbol_set->count; i++) {
                    const zbar_symbol_t *symbol = symbol_set->symbols[i];
                    LOG_INFO("Type: %s\n", zbar_get_symbol_name(zbar_symbol_get_type(symbol)));
                    LOG_INFO("Data Length: %d\n", zbar_symbol_get_data_length(symbol));
                    LOG_INFO("Data: %s\n", zbar_symbol_get_data(symbol));
                }
            } else {
                LOG_WARN("No symbol found.\n");
            }
        } break;

        case o_cvt: {
            LOG_INFO("File: %s\n", pfile);
            // convert bitmap to byte array and write to header file
            bitmap_t bitmap = {0, 0, 0, NULL};
            bitmap_read(&bitmap, pfile);
            int img_width                 = bitmap.width;
            int img_height                = bitmap.height;
            const unsigned char *img_data = bitmap.data;
            FILE *fp                      = fopen("QRSample.h", "w");
            if (fp) {
                LOG_INFO("Write QR Sample to %s\n", pfile);
                fprintf(fp, "#ifndef _RESOURCE_H_\n");
                fprintf(fp, "#define _RESOURCE_H_\n");
                fprintf(fp, "\n");
                fprintf(fp, "#define QR_SAMPLE_WIDTH %d\n", img_width);
                fprintf(fp, "#define QR_SAMPLE_HEIGHT %d\n", img_height);
                fprintf(fp, "\n");
                fprintf(fp, "const unsigned char QRSample[] = {\n");
                for (size_t i = 0; i < img_width * img_height; i++) {
                    fprintf(fp, "0x%02X, ", img_data[i]);
                    if ((i + 1) % 16 == 0) {
                        fprintf(fp, "\n");
                    }
                }
                fprintf(fp, "};\n");
                fprintf(fp, "\n");
                fprintf(fp, "#endif\n");
                fclose(fp);
            }
            LOG_INFO("Convert Done.\n");
        } break;

        case o_help:
        default:
            LOG("Usage: zbar [OPTION]... [FILE]...\n");
            for (size_t i = 0; i < sizeof(option_info) / sizeof(option_info_t); i++) {
                LOG(" -%s, --%s\t%s\n", option_info[i].short_name,
                    option_info[i].full_name, option_info[i].description);
            }
            break;
    }

    return 0;
}

#pragma endregion Exported Functions

/*  */
#pragma region Private Functions

symbol_set_t *scan_image_from_file(const char *file) {
    bitmap_t bitmap = {0, 0, 0, NULL};
    bitmap_read(&bitmap, file);
    int img_width                 = bitmap.width;
    int img_height                = bitmap.height;
    const unsigned char *img_data = bitmap.data;
    LOG_INFO("Image Size: %d x %d\n", img_width, img_height);

    int ret                       = 0;
    zbar_image_scanner_t *scanner = zbar_image_scanner_create();
    ret                           = zbar_image_scanner_set_config(scanner, ZBAR_NONE, ZBAR_CFG_ENABLE, 0);

    ret = zbar_image_scanner_set_config(scanner, ZBAR_QRCODE, ZBAR_CFG_ENABLE, 1);
    ret = zbar_image_scanner_set_config(scanner, ZBAR_EAN2, ZBAR_CFG_ENABLE, 1);
    ret = zbar_image_scanner_set_config(scanner, ZBAR_EAN5, ZBAR_CFG_ENABLE, 1);
    ret = zbar_image_scanner_set_config(scanner, ZBAR_EAN8, ZBAR_CFG_ENABLE, 1);
    ret = zbar_image_scanner_set_config(scanner, ZBAR_UPCE, ZBAR_CFG_ENABLE, 1);
    ret = zbar_image_scanner_set_config(scanner, ZBAR_ISBN10, ZBAR_CFG_ENABLE, 1);
    ret = zbar_image_scanner_set_config(scanner, ZBAR_UPCA, ZBAR_CFG_ENABLE, 1);
    ret = zbar_image_scanner_set_config(scanner, ZBAR_EAN13, ZBAR_CFG_ENABLE, 1);
    ret = zbar_image_scanner_set_config(scanner, ZBAR_ISBN13, ZBAR_CFG_ENABLE, 1);
    ret = zbar_image_scanner_set_config(scanner, ZBAR_I25, ZBAR_CFG_ENABLE, 1);
    ret = zbar_image_scanner_set_config(scanner, ZBAR_CODE39, ZBAR_CFG_ENABLE, 1);
    ret = zbar_image_scanner_set_config(scanner, ZBAR_PDF417, ZBAR_CFG_ENABLE, 1);
    ret =
        zbar_image_scanner_set_config(scanner, ZBAR_CODE128, ZBAR_CFG_ENABLE, 1);

    zbar_image_t *image = zbar_image_create();
    zbar_image_set_size(image, img_width, img_height);
    unsigned long format = zbar_fourcc_parse("Y800");
    zbar_image_set_format(image, format);
    zbar_image_set_data(image, (unsigned char *)img_data, img_width * img_height,
                        NULL);

    ret = zbar_scan_image(scanner, image);

    const char *data     = NULL;
    unsigned int dataLen = 0;

    symbol_set_t *symbol_set = (symbol_set_t *)malloc(sizeof(symbol_set_t));
    symbol_set->count        = 0;
    symbol_set->symbols      = NULL;

    if (ret > 0) {
        const zbar_symbol_t *symbol = zbar_image_first_symbol(image);
        for (; symbol; symbol = zbar_symbol_next(symbol)) {
            const zbar_symbol_t **ptr = (const zbar_symbol_t **)realloc(
                symbol_set->symbols, (symbol_set->count + 1) * sizeof(zbar_symbol_t *));
            if (ptr == NULL) {
                free(symbol_set->symbols);
                symbol_set->symbols = NULL;
                symbol_set->count   = 0;
                break;
            }
            symbol_set->symbols                    = ptr;
            symbol_set->symbols[symbol_set->count] = symbol;
            symbol_set->count++;
        }
    } else {
        symbol_set = NULL;
    }

    zbar_image_destroy(image);
    zbar_image_scanner_destroy(scanner);

    return symbol_set;
}

#pragma endregion Private Functions

#ifdef __cplusplus
}
#endif
