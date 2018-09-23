#include <string.h>


#include "jcfb/bitmap.h"
#include "jcfb/util.h"


int bitmap_init(bitmap_t* bmp, int w, int h) {
    *bmp = (bitmap_t){
        .w = w,
        .h = h,
        .fmt = PIXFMT_FB,
        .mem = calloc(w * h, sizeof(pixel_t)),
        .flags = BITMAP_FLAG_MEM_OWNER,
    };
    if (!bmp->mem) {
        return -1;
    }
    return 0;
}


int bitmap_init_from_memory(bitmap_t* bmp, int w, int h, void* mem)
{
    *bmp = (bitmap_t){
        .w = w,
        .h = h,
        .fmt = PIXFMT_FB,
        .mem = mem,
        .flags = 0,
    };
    return 0;
}


int bitmap_init_ex(bitmap_t* bmp, pixfmt_id_t fmt, int w, int h) {
    *bmp = (bitmap_t){
        .w = w,
        .h = h,
        .fmt = fmt,
        .mem = calloc(w * h, sizeof(pixel_t)),
        .flags = BITMAP_FLAG_MEM_OWNER,
    };
    if (!bmp->mem) {
        return -1;
    }
    return 0;
}


void bitmap_wipe(bitmap_t* bmp) {
    if ((bmp->flags & BITMAP_FLAG_MEM_OWNER) && bmp->mem) {
        free(bmp->mem);
        bmp->mem = NULL;
    }
}


size_t bitmap_memsize(const bitmap_t* bmp) {
    return bmp->w * bmp->h * sizeof(pixel_t);
}


void bitmap_put_pixel(bitmap_t* bmp, int x, int y, pixel_t color) {
    bmp->mem[y * bmp->w + x] = pixel_to(bmp->fmt, color);
}


void bitmap_clear(bitmap_t* bmp, pixel_t color) {
    color = pixel_to(bmp->fmt, color);
    for (int y = 0; y < bmp->h; y++) {
        for (int x = 0; x < bmp->w; x++) {
            bmp->mem[y * bmp->w + x] = color;
        }
    }
}


static void _blit_row(bitmap_t* dst, const bitmap_t* src,
                      int dx, int dy, int sx, int sy)
{
    pixel_t* dest_addr = dst->mem + dy * dst->w + dx;
    pixel_t* src_addr = src->mem + sy * src->w + sx;
    size_t max_size = min(src->w - sx, dst->w - dx);
    memcpy(dest_addr, src_addr, max_size * sizeof(pixel_t));
}


static void _convert_row(bitmap_t* dst, int x, int y,
                         pixfmt_id_t pixfmt)
{
    for (int i = x; i < dst->w; i++) {
        pixel_t old_color = dst->mem[y * dst->w + i];
        pixel_t new_color = pixel_conv(pixfmt, dst->fmt, old_color);
        dst->mem[y * dst->w + i] = new_color;
    }
}


static void _fast_blit(bitmap_t* dst, const bitmap_t* src,
                       int x, int y)
{
    for (int sy = 0; sy < min(src->h, dst->h - y); sy++) {
        _blit_row(dst, src, x, y + sy, 0, sy);
    }
}


static void _slow_blit(bitmap_t* dst, const bitmap_t* src,
                       int x, int y)
{
    for (int sy = 0; sy < min(src->h, dst->h - y); sy++) {
        _blit_row(dst, src, x, y + sy, 0, sy);
        _convert_row(dst, x, y, src->fmt);
    }
}


void bitmap_blit(bitmap_t* dst, const bitmap_t* src, int x, int y) {
    if (dst->fmt == src->fmt) {
        _fast_blit(dst, src, x, y);
    } else {
        _slow_blit(dst, src, x, y);
    }
}


static void _blit_scaled_row(pixel_t* dst, const pixel_t* src,
                             int sw, int dw)
{
    // If scale is greater than 1, it means that the source is greater
    // than the dest, so we will miss some input pixels.
    // If scale is lower than 1, it means the the source is smaller
    // than the dest, so we will duplicates some pixels.
    float scale = sw / (float) dw;
    for (int dx = 0; dx < dw; dx++) {
        int sx = dx * scale;
        // TODO conversion ?
        dst[dx] = src[sx];
    }
}


void bitmap_scaled_blit(bitmap_t* dst, const bitmap_t* src,
                        int dx, int dy, int dw, int dh)
{
    int min_y = dy;
    int max_y = dy + dh;
    float y_ratio = src->h / (float)dh;
    for (; dy < min(max_y, dst->h); dy++) {
        int sy = min((dy - min_y) * y_ratio, src->h - 1);
        pixel_t* dst_addr = dst->mem + dy * dst->w + dx;
        pixel_t* src_addr = src->mem + sy * src->w;
        size_t max_size = min(dw, dst->w - dx);
        _blit_scaled_row(dst_addr, src_addr, src->w, max_size);
    }
}


void bitmap_masked_blit(bitmap_t* dst, const bitmap_t* src,
                        int x, int y)
{
    pixel_t mask = pixel_to(src->fmt, 0x00ff00ff);
    for (size_t sy = 0; sy < min(src->h, dst->h - y); sy++) {
        size_t dy = (y + sy) * dst->w;
        for (size_t sx = 0; sx < min(src->w, dst->w - x); sx++) {
            pixel_t p = src->mem[sy * src->w + sx];
            if (p == mask) {
                continue;
            }
            size_t dx = x + sx;
            dst->mem[dy + dx] = pixel_conv(src->fmt, dst->fmt, p);
        }
    }
}


pixel_t* bitmap_pixel_addr(bitmap_t* bmp, int x, int y) {
    return &bmp->mem[y * bmp->w + x];
}
