#ifndef SVPNG_INC_
#define SVPNG_INC_

#ifndef SVPNG_LINKAGE
#define SVPNG_LINKAGE
#endif

#ifndef SVPNG_OUTPUT
#include <stdio.h>
#define SVPNG_OUTPUT FILE *fp
#endif

#ifndef SVPNG_PUT
#define SVPNG_PUT(u) fputc(u, fp)
#endif

SVPNG_LINKAGE void svpng(SVPNG_OUTPUT, unsigned w, unsigned h, const unsigned char *img, int alpha)
{
    static const unsigned t[] = {0, 0x1db71064, 0x3b6e20c8, 0x26d930ac, 0x76dc4190, 0x6b6b51f4, 0x4db26158, 0x5005713c,
                                0xedb88320, 0xf00f9344, 0xd6d6a3e8, 0xcb61b38c, 0x9b64c2b0, 0x86d3d2d4, 0xa00ae278, 0xbdbdf21c};
    unsigned a = 1, b = 0, c, p = w * (alpha ? 4 : 3) + 1, x, y, i;
#define SVPNG_U8A(ua, l)    \
    for (i = 0; i < l; i++) \
        SVPNG_PUT((ua)[i]);
#define SVPNG_U32(u)                  \
    do                                \
    {                                 \
        SVPNG_PUT((u) >> 24);         \
        SVPNG_PUT(((u) >> 16) & 255); \
        SVPNG_PUT(((u) >> 8) & 255);  \
        SVPNG_PUT((u)&255);           \
    } while (0)
#define SVPNG_U8C(u)              \
    do                            \
    {                             \
        SVPNG_PUT(u);             \
        c ^= (u);                 \
        c = (c >> 4) ^ t[c & 15]; \
        c = (c >> 4) ^ t[c & 15]; \
    } while (0)
#define SVPNG_U8AC(ua, l)   \
    for (i = 0; i < l; i++) \
    SVPNG_U8C((ua)[i])
#define SVPNG_U16LC(u)               \
    do                               \
    {                                \
        SVPNG_U8C((u)&255);          \
        SVPNG_U8C(((u) >> 8) & 255); \
    } while (0)
#define SVPNG_U32C(u)                 \
    do                                \
    {                                 \
        SVPNG_U8C((u) >> 24);         \
        SVPNG_U8C(((u) >> 16) & 255); \
        SVPNG_U8C(((u) >> 8) & 255);  \
        SVPNG_U8C((u)&255);           \
    } while (0)
#define SVPNG_U8ADLER(u)       \
    do                         \
    {                          \
        SVPNG_U8C(u);          \
        a = (a + (u)) % 65521; \
        b = (b + a) % 65521;   \
    } while (0)
#define SVPNG_BEGIN(s, l) \
    do                    \
    {                     \
        SVPNG_U32(l);     \
        c = ~0U;          \
        SVPNG_U8AC(s, 4); \
    } while (0)
#define SVPNG_END() SVPNG_U32(~c)
    SVPNG_U8A("\x89PNG\r\n\32\n", 8);
    SVPNG_BEGIN("IHDR", 13);
    SVPNG_U32C(w);
    SVPNG_U32C(h);
    SVPNG_U8C(8);
    SVPNG_U8C(alpha ? 6 : 2);
    SVPNG_U8AC("\0\0\0", 3);
    SVPNG_END();
    SVPNG_BEGIN("IDAT", 2 + h * (5 + p) + 4);
    SVPNG_U8AC("\x78\1", 2);
    for (y = 0; y < h; y++)
    {
        SVPNG_U8C(y == h - 1);
        SVPNG_U16LC(p);
        SVPNG_U16LC(~p);
        SVPNG_U8ADLER(0);
        for (x = 0; x < p - 1; x++, img++)
            SVPNG_U8ADLER(*img);
    }
    SVPNG_U32C((b << 16) | a);
    SVPNG_END();
    SVPNG_BEGIN("IEND", 0);
    SVPNG_END();
}

#endif /* SVPNG_INC_ */
