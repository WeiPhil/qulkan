#include <cmath>
#include <string>

#include "qulkan/logger.h"
#include "utils/pngwriter.h"
#include <iostream>

// float *PNGWriter::createMandelbrotImage(int width, int height, float xS, float yS, float rad, int maxIteration) {
//     float *buffer = new float[width * height];
//     if (buffer == NULL) {
//         Qulkan::Logger::Error("Could not create image buffer\n");
//         return NULL;
//     }

//     // Create Mandelbrot set image

//     int xPos, yPos;
//     float minMu = maxIteration;
//     float maxMu = 0;

//     for (yPos = 0; yPos < height; yPos++) {
//         float yP = (yS - rad) + (2.0f * rad / height) * yPos;

//         for (xPos = 0; xPos < width; xPos++) {
//             float xP = (xS - rad) + (2.0f * rad / width) * xPos;

//             int iteration = 0;
//             float x = 0;
//             float y = 0;

//             while (x * x + y * y <= 4 && iteration < maxIteration) {
//                 float tmp = x * x - y * y + xP;
//                 y = 2 * x * y + yP;
//                 x = tmp;
//                 iteration++;
//             }

//             if (iteration < maxIteration) {
//                 float modZ = sqrt(x * x + y * y);
//                 float mu = iteration - (log(log(modZ))) / log(2);
//                 if (mu > maxMu)
//                     maxMu = mu;
//                 if (mu < minMu)
//                     minMu = mu;
//                 buffer[yPos * width + xPos] = mu;
//             } else {
//                 buffer[yPos * width + xPos] = 0;
//             }
//         }
//     }

//     // Scale buffer values between 0 and 1
//     int count = width * height;
//     while (count) {
//         count--;
//         buffer[count] = (buffer[count] - minMu) / (maxMu - minMu);
//     }

//     return buffer;
// }

PNGWriter::PNGWriter(int width, int height, int channels, float *imageBuffer) : width(width), height(height), imageBuffer(imageBuffer), channels(channels) {

    // this->imageBuffer = createMandelbrotImage(width, height, -0.802f, -0.177f, 0.011f, 110);
}

int PNGWriter::writePNG(const char *filename, char *imageTitle) {
    std::cout << imageBuffer[0] << std::endl;
    int code = 0;
    FILE *fp = NULL;
    png_structp png_ptr = NULL;
    png_infop info_ptr = NULL;
    png_bytep row = NULL;

    // Open file for writing (binary mode)
    fp = fopen(filename, "wb");
    if (fp == NULL) {
        Qulkan::Logger::Error("Could not open file %s for writing\n", filename);
        code = 1;
        goto finalise;
    }

    // Initialize write structure
    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (png_ptr == NULL) {
        Qulkan::Logger::Error("Could not allocate write struct\n");
        code = 1;
        goto finalise;
    }

    // Initialize info structure
    info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == NULL) {
        Qulkan::Logger::Error("Could not allocate info struct\n");
        code = 1;
        goto finalise;
    }

    // Setup Exception handling
    if (setjmp(png_jmpbuf(png_ptr))) {
        Qulkan::Logger::Error("Error during png creation\n");
        code = 1;
        goto finalise;
    }

    png_init_io(png_ptr, fp);

    int type;
    type = channels == 3 ? PNG_COLOR_TYPE_RGB : PNG_COLOR_TYPE_RGB_ALPHA;

    assert(channels == 3 || channels == 4);

    // Write header (8 bit colour depth)
    png_set_IHDR(png_ptr, info_ptr, width, height, 8, type, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

    // Set title
    if (imageTitle != NULL) {
        png_text title_text;
        title_text.compression = PNG_TEXT_COMPRESSION_NONE;
        title_text.key = "Title";
        title_text.text = imageTitle;
        png_set_text(png_ptr, info_ptr, &title_text, 1);
    }

    png_write_info(png_ptr, info_ptr);

    // Allocate memory for one row (3/4 bytes per pixel - RGB/RGBA)
    row = (png_bytep)malloc(channels * width * sizeof(png_byte));

    // Write image data
    int x, y, c, idx;
    idx = 0;
    for (y = 0; y < height; y++) {
        for (x = 0; x < width; x++) {
            for (c = 0; c < channels; c++) {
                row[x + c] = imageBuffer[idx] * 255.0;
                idx++;
            }
        }
        png_write_row(png_ptr, row);
    }

    // End write
    png_write_end(png_ptr, NULL);

finalise:
    if (fp != NULL)
        fclose(fp);
    if (info_ptr != NULL)
        png_free_data(png_ptr, info_ptr, PNG_FREE_ALL, -1);
    if (png_ptr != NULL)
        png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
    if (row != NULL)
        free(row);

    Qulkan::Logger::Info("PNGWriter: Wrote png file to %s \n", filename);

    return code;
}
