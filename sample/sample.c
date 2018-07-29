/*
 * Sample
 *
 * This source file is the main sample showing how to use JCFB and which
 * capabilities the library has.
 */
#include <stdio.h>
#include <unistd.h>
#include "jcfb/jcfb.h"


int main(int argc, char** argv) {
    if (argc < 2) {
        fprintf(stderr, "usage: sample <image> \n");
        return 1;
    }

    if (jcfb_start() < 0) {
        fprintf(stderr, "Cannot start JCFB\n");
        return 1;
    }

    bitmap_t image;
    if (bitmap_load(&image, argv[1]) != 0) {
        return 1;
    }

    bitmap_t* buffer = jcfb_get_bitmap();
    bitmap_clear(buffer, 0x00000000);

    float ratio = 1.0;
    if (buffer->h < image.h || buffer->w < image.w) {
        float yratio = buffer->h / (float)image.h;
        float xratio = buffer->w / (float)image.w;
        if (image.w * yratio > buffer->w) {
            ratio = xratio;
        } else {
            ratio = yratio;
        }
    }
    int dw = image.w * ratio;
    int dh = image.h * ratio;
    int dx = (buffer->w - dw) / 2;
    int dy = (buffer->h - dh) / 2;
    bitmap_scaled_blit(buffer, &image, dx, dy, dw, dh);

    int exit = 0;
    while (!exit) {
        int key = 0;
        while ((key = jcfb_next_key()) >= 0) {
            printf("%d\n", key);
            if (key == 27) {
                exit = 1;
            }
        }
        jcfb_refresh(buffer);
        usleep(1000);
    }

    bitmap_wipe(&image);
    bitmap_wipe(buffer);
    free(buffer);

    jcfb_stop();
    printf("Gentle quit\n");
    return 0;
}
