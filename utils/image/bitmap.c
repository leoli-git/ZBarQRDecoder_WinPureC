/*------------------------------------------------------------------------
 *  Copyright 2023 (c) Leo CH Li <jafee201153@gmail.com>
 *------------------------------------------------------------------------*/

#include "bitmap.h"

#include <stdio.h>
#include <stdlib.h>

void write_to_bitmap(const char *name, const unsigned char *raw_img,
                     unsigned int width, unsigned int height,
                     unsigned short bits) {
  if (!(name && raw_img)) {
    perror("Error write_to_bitmap.");
    return;
  }
  struct BitmapFileHeader file_h = {
      .bfTybe = 0x4d42,
      .bfReserved1 = 0,
      .bfReserved2 = 0,
      .bfOffBits = 54,
  };
  file_h.bfSize = file_h.bfOffBits + width * height * bits / 8;
  if (bits == 8) {
    file_h.bfSize += 1024, file_h.bfOffBits += 1024;
  }
  struct BitmapInfoHeader info_h = {
      .biSize = 40,
      .biPlanes = 1,
      .biCompression = 0,
      .biXPelsPerMeter = 0,
      .biYPelsPerMeter = 0,
      .biClrUsed = 0,
      .biClrImportant = 0,
  };
  info_h.biWidth = width;
  info_h.biHeight = height;
  info_h.biBitCount = bits;
  info_h.biSizeImage = width * height * bits / 8;
  if (bits == 8) {
    info_h.biClrUsed = 256;
  }
  FILE *pFile = NULL;
  fopen_s(&pFile, name, "wb+");
  if (!pFile) {
    perror("Error opening file.");
    return;
  }
  fwrite((char *)&file_h, sizeof(char), sizeof(file_h), pFile);
  fwrite((char *)&info_h, sizeof(char), sizeof(info_h), pFile);
  if (bits == 8) {
    for (unsigned i = 0; i < 256; ++i) {
      unsigned char c = i;
      fwrite((char *)&c, sizeof(char), sizeof(unsigned char), pFile);
      fwrite((char *)&c, sizeof(char), sizeof(unsigned char), pFile);
      fwrite((char *)&c, sizeof(char), sizeof(unsigned char), pFile);
      fwrite("", sizeof(char), sizeof(unsigned char), pFile);
    }
  }
  size_t alig = ((width * bits / 8) * 3) % 4;
  for (int j = height - 1; j >= 0; --j) {
    for (unsigned i = 0; i < width; ++i) {
      unsigned int idx = j * width + i;
      if (bits == 24)  // RGB24
      {
        fwrite((char *)&raw_img[idx * 3 + 2], sizeof(char),
               sizeof(unsigned char), pFile);
        fwrite((char *)&raw_img[idx * 3 + 1], sizeof(char),
               sizeof(unsigned char), pFile);
        fwrite((char *)&raw_img[idx * 3 + 0], sizeof(char),
               sizeof(unsigned char), pFile);
      } else if (bits == 8)  // Gray8
      {
        fwrite((char *)&raw_img[idx], sizeof(char), sizeof(unsigned char),
               pFile);
      }
    }
    for (size_t i = 0; i < alig; ++i)  // 4-byte alignment
    {
      fwrite("", sizeof(char), sizeof(unsigned char), pFile);
    }
  }
  fclose(pFile);
}

void read_from_bitmap(const char *name, unsigned char **raw_img,
                      unsigned int *width, unsigned int *height,
                      unsigned short *bits) {
  if (!(name && raw_img && width && height && bits)) {
    perror("Error read_from_bitmap.");
    return;
  }
  struct BitmapFileHeader file_h;
  struct BitmapInfoHeader info_h;
  FILE *pFile = NULL;
  fopen_s(&pFile, name, "rb+");
  if (!pFile) {
    perror("Error opening file.");
    return;
  }
  fread((char *)&file_h, sizeof(char), sizeof(file_h), pFile);
  fread((char *)&info_h, sizeof(char), sizeof(info_h), pFile);
  *width = info_h.biWidth;
  *height = info_h.biHeight;
  *bits = info_h.biBitCount;
  size_t ImgSize = ((size_t)*width) * ((size_t)*height) * 3;
  *raw_img = (unsigned char *)calloc(ImgSize, sizeof(unsigned char));
  fseek(pFile, file_h.bfOffBits, SEEK_SET);
  size_t alig = ((info_h.biWidth * info_h.biBitCount / 8) * 3) % 4;
  for (int j = *height - 1; j >= 0; --j) {
    for (unsigned i = 0; i < *width; ++i) {
      unsigned int idx = j * (*width) + i;
      if (*bits == 24) {
        fread((char *)&(*raw_img)[idx * 3 + 2], sizeof(char),
              sizeof(unsigned char), pFile);
        fread((char *)&(*raw_img)[idx * 3 + 1], sizeof(char),
              sizeof(unsigned char), pFile);
        fread((char *)&(*raw_img)[idx * 3 + 0], sizeof(char),
              sizeof(unsigned char), pFile);
      } else if (*bits == 8) {
        fread((char *)&(*raw_img)[idx], sizeof(char), sizeof(unsigned char),
              pFile);
      }
    }
    fseek(pFile, (long)alig, SEEK_CUR);
  }
  fclose(pFile);
}

void bitmap_write(const bitmap_t *_this, const char *name) {
  const bitmap_t *p = _this;
  write_to_bitmap(name, p->data, p->width, p->height, p->bits);
}

void bitmap_read(const bitmap_t *_this, const char *name) {
  const bitmap_t *p = _this;
  read_from_bitmap(name, (unsigned char **)&p->data, (unsigned int *)&p->width,
                   (unsigned int *)&p->height, (unsigned short *)&p->bits);
}
