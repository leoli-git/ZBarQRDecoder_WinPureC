/*------------------------------------------------------------------------
 *  Copyright 2023 (c) Leo CH Li <jafee201153@gmail.com>
 *------------------------------------------------------------------------*/

#ifndef __BITMAP_H__
#define __BITMAP_H__

#pragma pack(2)
struct BitmapFileHeader {
  unsigned short bfTybe;
  unsigned int bfSize;
  unsigned short bfReserved1;
  unsigned short bfReserved2;
  unsigned int bfOffBits;
};
struct BitmapInfoHeader {
  unsigned int biSize;
  unsigned int biWidth;
  unsigned int biHeight;
  unsigned short biPlanes;  // 1=defeaul, 0=custom
  unsigned short biBitCount;
  unsigned int biCompression;
  unsigned int biSizeImage;
  unsigned int biXPelsPerMeter;  // 72dpi=2835, 96dpi=3780
  unsigned int biYPelsPerMeter;  // 120dpi=4724, 300dpi=11811
  unsigned int biClrUsed;
  unsigned int biClrImportant;
};
#pragma pack()

typedef struct bitmap_t {
  unsigned int width, height;
  unsigned short bits;
  unsigned char *data;
} bitmap_t;

void bitmap_write(const bitmap_t *_this, const char *name);
void bitmap_read(const bitmap_t *_this, const char *name);

#endif /* __BITMAP_H__ */
