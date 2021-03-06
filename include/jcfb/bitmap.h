/*
 * Bitmap module
 *
 * Load, create and manipulate bitmaps.
 *
 * Every bliting function assumes given bitmaps have the same pixel
 * format, minus `bitmap_blit()`.
 */
#ifndef _jcfb_bitmap_h_
#define _jcfb_bitmap_h_


#include <stdbool.h>


#include "jcfb/pixel.h"


/*
 * Bitmap flags
 */
enum {
    /*
     * The bitmap owns its memory
     */
    BITMAP_FLAG_MEM_OWNER = 0x01,
};


/*
 * Bitmap structure
 */
typedef struct bitmap {
    int w, h;
    pixfmt_id_t fmt;
    pixel_t* mem;
    uint32_t flags;
} bitmap_t;


/*
 * Initialize a bitmap having the given dimensions and the PIXFMT_FB
 * pixel format.
 */
int bitmap_init(bitmap_t* bmp, int w, int h);


/*
 * Like `bitmap_init` but the bitmap doesn't own its own memory.
 */
int bitmap_init_from_memory(bitmap_t* bmp, int w, int h, void* mem);


/*
 * Initialize a bitmap having the given dimensions and the given pixel
 * format.
 */
int bitmap_init_ex(bitmap_t* bmp, pixfmt_id_t fmt, int w, int h);


/*
 * Wipe the bitmap's memory.
 */
void bitmap_wipe(bitmap_t* bmp);


/*
 * Returns the address of the given pixel.
 */
pixel_t* bitmap_pixel_addr(bitmap_t* bmp, int x, int y);


/*
 * Returns the given pixel.
 */
pixel_t bitmap_pixel(const bitmap_t* bmp, int x, int y);


/*
 * Returns the size of the bitmap allocated space in bytes.
 */
size_t bitmap_memsize(const bitmap_t* bmp);


/*
 * Put a RGBA32 pixel at the given coordinates of the bitmap.
 */
void bitmap_put_pixel(bitmap_t* bmp, int x, int y, pixel_t color);


/*
 * Put a RGBA32 pixel at the given coordinates of the bitmap.
 * The `color` is added to the bitmap pixel for additive blending
 */
void bitmap_put_pixel_blend_add(bitmap_t* bmp, int x, int y, pixel_t color);


/*
 * Clear every pixels of the given bitmap with the given RGBA32 color.
 */
void bitmap_clear(bitmap_t* bmp, pixel_t color);


/*
 * Returns the size needed to store a bitmap line, in bytes.
 */
size_t bitmap_line_size(const bitmap_t* bmp);


/*
 * Returns `true` if point (`x`, `y`) is in the given bitmap.
 */
bool bitmap_is_in(const bitmap_t* bmp, int x, int y);


/* Regular blits ----------------------------------------------------------- */
/*
 * Blit the `src` bitmap at the given position of `dst` bitmap.
 */
void bitmap_blit(bitmap_t* dst, const bitmap_t* src, int x, int y);


/*
 * Blit the `src` bitmap at the given position of `dst` bitmap, scaled
 * to dimensions (w, h).
 */
void bitmap_scaled_blit(bitmap_t* dst, const bitmap_t* src,
                        int x, int y, int w, int h);


/*
 * Blit the given region of the `src` bitmap to the given position of
 * `dst` bitmap, scaled to dimensions (dw, dh).
 */
void bitmap_scaled_region_blit(bitmap_t* dst, const bitmap_t* src,
                               int sx, int sy, int sw, int sh,
                               int dx, int dy, int dw, int dh);


/*
 * Blit the `src` bitmap at the given position of `dst` bitmap. Skip
 * masked pixels.
 */
void bitmap_masked_blit(bitmap_t* dst, const bitmap_t* src,
                        int x, int y);



/*
 * Like `bitmap_blit()`, but flip on the x-axis
 */
void bitmap_blit_hflip(bitmap_t* dst, const bitmap_t* src, int x, int y);


/*
 * Blit the `src` bitmap around the point `(cx, cy)` with a rotation of `a`
 * radiants.
 */
void bitmap_rotated_blit(bitmap_t* dst, const bitmap_t* src,
                         int cx, int cy, float a);


/* Additive blend blits ---------------------------------------------------- */
void bitmap_blit_blend_add(bitmap_t* dst, const bitmap_t* src, int x, int y);


void bitmap_scaled_blit_blend_add(bitmap_t* dst, const bitmap_t* src,
                                  int x, int y, int w, int h);


void bitmap_scaled_region_blit_blend_add(bitmap_t* dst, const bitmap_t* src,
                                         int sx, int sy, int sw, int sh,
                                         int dx, int dy, int dw, int dh);


void bitmap_masked_blit_blend_add(bitmap_t* dst, const bitmap_t* src,
                                  int x, int y);


void bitmap_blit_hflip_blend_add(bitmap_t* dst, const bitmap_t* src,
                                 int x, int y);


void bitmap_rotated_blit_blend_add(bitmap_t* dst, const bitmap_t* src,
                                   int cx, int cy, float a);


/* Masked blits ------------------------------------------------------------ */
void bitmap_blit_masked(bitmap_t* dst, const bitmap_t* src, int x, int y);


void bitmap_scaled_blit_masked(bitmap_t* dst, const bitmap_t* src,
                               int x, int y, int w, int h);


void bitmap_scaled_region_blit_masked(bitmap_t* dst, const bitmap_t* src,
                                      int sx, int sy, int sw, int sh,
                                      int dx, int dy, int dw, int dh);


void bitmap_masked_blit_masked(bitmap_t* dst, const bitmap_t* src,
                               int x, int y);


void bitmap_blit_hflip_masked(bitmap_t* dst, const bitmap_t* src,
                              int x, int y);


void bitmap_rotated_blit_masked(bitmap_t* dst, const bitmap_t* src,
                                int cx, int cy, float a);


/* ------------------------------------------------------------------------- */


#endif
