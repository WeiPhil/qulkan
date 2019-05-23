#pragma once

#ifndef PNGWRITER_H
#define PNGWRITER_H

#include <png.h>

class PNGWriter {

  private:
    int width, height, channels;
    float *imageBuffer;

    // float *createMandelbrotImage(int width, int height, float xS, float yS, float rad, int maxIteration);

    // inline void setRGB(png_byte *ptr, float val) {
    //     int v = (int)(val * 767);
    //     if (v < 0)
    //         v = 0;
    //     if (v > 767)
    //         v = 767;
    //     int offset = v % 256;

    //     if (v < 256) {
    //         ptr[0] = 0;
    //         ptr[1] = 0;
    //         ptr[2] = offset;
    //     } else if (v < 512) {
    //         ptr[0] = 0;
    //         ptr[1] = offset;
    //         ptr[2] = 255 - offset;
    //     } else {
    //         ptr[0] = offset;
    //         ptr[1] = 255 - offset;
    //         ptr[2] = 0;
    //     }
    // }

  public:
    PNGWriter(int width, int height, int channels, float *imageBuffer);
    ~PNGWriter() { delete imageBuffer; };

    int writePNG(const char *filename, char *imageTitle);
};

#endif
