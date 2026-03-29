#ifndef STB_IMAGE_WRITE_IMPLEMENTATION
#ifndef STB_IMAGE_WRITE_H
#define STB_IMAGE_WRITE_H

#include <stdlib.h>

typedef unsigned char stbiw_uint8;

#ifdef __cplusplus
extern "C" {
#endif

int stbi_write_png(char const *filename, int w, int h, int comp, const void *data, int stride_in_bytes);
int stbi_write_bmp(char const *filename, int w, int h, int comp, const void *data);
int stbi_write_jpg(char const *filename, int x, int y, int comp, const void *data, int quality);

#ifdef __cplusplus
}
#endif

#endif  
#endif  

#ifdef STB_IMAGE_WRITE_IMPLEMENTATION

#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#ifndef STBIW_ASSERT
#include <assert.h>
#define STBIW_ASSERT(x) assert(x)
#endif

#define STBIW_UCHAR(x) (unsigned char) ((x) & 0xff)

static void stbiw__writefile(void *context, void *data, int size)
{
    FILE *f = (FILE*) context;
    fwrite(data, 1, size, f);
}

static int stbiw__outfile(void *context, int rgb_dir, int vdir, int x, int y, int comp, int expand_mono, void *data, int alpha, int pad, const char *fmt, ...)
{
    FILE *f = (FILE*) context;
    if (f == NULL) return 0;
    
    va_list v;
    va_start(v, fmt);
    vfprintf(f, fmt, v);
    va_end(v);
    return 1;
}

static int stbi__flip_vertically_on_write = 0;

int stbi_write_bmp(char const *filename, int x, int y, int comp, const void *data)
{
    FILE *f;
    int i, pad;
    unsigned char *d;
    
    if (data == NULL) return 0;
    
    f = fopen(filename, "wb");
    if (!f) return 0;
    
    pad = (-x*comp) & 3;
    
    fprintf(f, "BM");
    unsigned int file_size = 54 + (x*comp + pad) * y;
    fputc(file_size & 0xff, f);
    fputc((file_size >> 8) & 0xff, f);
    fputc((file_size >> 16) & 0xff, f);
    fputc((file_size >> 24) & 0xff, f);
    fputc(0, f); fputc(0, f);
    fputc(0, f); fputc(0, f);
    fputc(54, f); fputc(0, f); fputc(0, f); fputc(0, f);
    
    fputc(x & 0xff, f);
    fputc((x >> 8) & 0xff, f);
    fputc((x >> 16) & 0xff, f);
    fputc((x >> 24) & 0xff, f);
    fputc(y & 0xff, f);
    fputc((y >> 8) & 0xff, f);
    fputc((y >> 16) & 0xff, f);
    fputc((y >> 24) & 0xff, f);
    fputc(1, f); fputc(0, f);
    fputc(comp*8, f); fputc(0, f);
    fputc(0, f); fputc(0, f); fputc(0, f); fputc(0, f);
    fputc(0, f); fputc(0, f); fputc(0, f); fputc(0, f);
    fputc(0, f); fputc(0, f); fputc(0, f); fputc(0, f);
    fputc(0, f); fputc(0, f); fputc(0, f); fputc(0, f);
    
    d = (unsigned char *) data;
    if (comp == 4) {
        for (i = 0; i < y; ++i) {
            unsigned char *p = d + (y - 1 - i) * x * comp;
            for (int j = 0; j < x; ++j) {
                fputc(p[2], f);
                fputc(p[1], f);
                fputc(p[0], f);
                p += 4;
            }
            for (int k = 0; k < pad; ++k) fputc(0, f);
        }
    } else if (comp == 3) {
        for (i = 0; i < y; ++i) {
            unsigned char *p = d + (y - 1 - i) * x * comp;
            for (int j = 0; j < x; ++j) {
                fputc(p[2], f);
                fputc(p[1], f);
                fputc(p[0], f);
                p += 3;
            }
            for (int k = 0; k < pad; ++k) fputc(0, f);
        }
    } else {
        for (i = 0; i < y; ++i) {
            unsigned char *p = d + (y - 1 - i) * x;
            fwrite(p, 1, x, f);
            for (int k = 0; k < pad; ++k) fputc(0, f);
        }
    }
    
    fclose(f);
    return 1;
}

static unsigned int stbiw__crc32(unsigned char *buffer, int len)
{
    static unsigned int crc_table[256] = {
        0x00000000, 0x77073096, 0xEE0E612C, 0x990951BA, 0x076DC419, 0x706AF48F, 0xE963A535, 0x9E6495A3,
        0x0EDB8832, 0x79DCB8A4, 0xE0D5E91E, 0x97D2D988, 0x09B64C2B, 0x7EB17CBD, 0xE7B82D07, 0x90BF1D91,
        0x1DB71064, 0x6AB020F2, 0xF3B97148, 0x84BE41DE, 0x1ADAD47D, 0x6DDDE4EB, 0xF4D4B551, 0x83D385C7,
        0x136C9856, 0x646BA8C0, 0xFD62F97A, 0x8A65C9EC, 0x14015C4F, 0x63066CD9, 0xFA0F3D63, 0x8D080DF5,
        0x3B6E20C8, 0x4C69105E, 0xD56041E4, 0xA2677172, 0x3C03E4D1, 0x4B04D447, 0xD20D85FD, 0xA50AB56B,
        0x35B5A8FA, 0x42B2986C, 0xDBBBC9D6, 0xACBCF940, 0x32D86CE3, 0x45DF5C75, 0xDCD60DCF, 0xABD13D59,
        0x26D930AC, 0x51DE003A, 0xC8D75180, 0xBFD06116, 0x21B4F4B5, 0x56B3C423, 0xCFBA9599, 0xB8BDA50F,
        0x2802B89E, 0x5F058808, 0xC60CD9B2, 0xB10BE924, 0x2F6F7C87, 0x58684C11, 0xC1611DAB, 0xB6662D3D,
        0x76DC4190, 0x01DB7106, 0x98D220BC, 0xEFD5102A, 0x71B18589, 0x06B6B51F, 0x9FBFE4A5, 0xE8B8D433,
        0x7807C9A2, 0x0F00F934, 0x9609A88E, 0xE10E9818, 0x7F6A0DBB, 0x086D3D2D, 0x91646C97, 0xE6635C01,
        0x6B6B51F4, 0x1C6C6162, 0x856530D8, 0xF262004E, 0x6C0695ED, 0x1B01A57B, 0x8208F4C1, 0xF50FC457,
        0x65B0D9C6, 0x12B7E950, 0x8BBEB8EA, 0xFCB9887C, 0x62DD1DDF, 0x15DA2D49, 0x8CD37CF3, 0xFBD44C65,
        0x4DB26158, 0x3AB551CE, 0xA3BC0074, 0xD4BB30E2, 0x4ADFA541, 0x3DD895D7, 0xA4D1C46D, 0xD3D6F4FB,
        0x4369E96A, 0x346ED9FC, 0xAD678846, 0xDA60B8D0, 0x44042D73, 0x33031DE5, 0xAA0A4C5F, 0xDD0D7CC9,
        0x5005713C, 0x270241AA, 0xBE0B1010, 0xC90C2086, 0x5768B525, 0x206F85B3, 0xB966D409, 0xCE61E49F,
        0x5EDEF90E, 0x29D9C998, 0xB0D09822, 0xC7D7A8B4, 0x59B33D17, 0x2EB40D81, 0xB7BD5C3B, 0xC0BA6CAD,
        0xEDB88320, 0x9ABFB3B6, 0x03B6E20C, 0x74B1D29A, 0xEAD54739, 0x9DD277AF, 0x04DB2615, 0x73DC1683,
        0xE3630B12, 0x94643B84, 0x0D6D6A3E, 0x7A6A5AA8, 0xE40ECF0B, 0x9309FF9D, 0x0A00AE27, 0x7D079EB1,
        0xF00F9344, 0x8708A3D2, 0x1E01F268, 0x6906C2FE, 0xF762575D, 0x806567CB, 0x196C3671, 0x6E6B06E7,
        0xFED41B76, 0x89D32BE0, 0x10DA7A5A, 0x67DD4ACC, 0xF9B9DF6F, 0x8EBEEFF9, 0x17B7BE43, 0x60B08ED5,
        0xD6D6A3E8, 0xA1D1937E, 0x38D8C2C4, 0x4FDFF252, 0xD1BB67F1, 0xA6BC5767, 0x3FB506DD, 0x48B2364B,
        0xD80D2BDA, 0xAF0A1B4C, 0x36034AF6, 0x41047A60, 0xDF60EFC3, 0xA867DF55, 0x316E8EEF, 0x4669BE79,
        0xCB61B38C, 0xBC66831A, 0x256FD2A0, 0x5268E236, 0xCC0C7795, 0xBB0B4703, 0x220216B9, 0x5505262F,
        0xC5BA3BBE, 0xB2BD0B28, 0x2BB45A92, 0x5CB36A04, 0xC2D7FFA7, 0xB5D0CF31, 0x2CD99E8B, 0x5BDEAE1D,
        0x9B64C2B0, 0xEC63F226, 0x756AA39C, 0x026D930A, 0x9C0906A9, 0xEB0E363F, 0x72076785, 0x05005713,
        0x95BF4A82, 0xE2B87A14, 0x7BB12BAE, 0x0CB61B38, 0x92D28E9B, 0xE5D5BE0D, 0x7CDCEFB7, 0x0BDBDF21,
        0x86D3D2D4, 0xF1D4E242, 0x68DDB3F8, 0x1FDA836E, 0x81BE16CD, 0xF6B9265B, 0x6FB077E1, 0x18B74777,
        0x88085AE6, 0xFF0F6A70, 0x66063BCA, 0x11010B5C, 0x8F659EFF, 0xF862AE69, 0x616BFFD3, 0x166CCF45,
        0xA00AE278, 0xD70DD2EE, 0x4E048354, 0x3903B3C2, 0xA7672661, 0xD06016F7, 0x4969474D, 0x3E6E77DB,
        0xAED16A4A, 0xD9D65ADC, 0x40DF0B66, 0x37D83BF0, 0xA9BCAE53, 0xDEBB9EC5, 0x47B2CF7F, 0x30B5FFE9,
        0xBDBDF21C, 0xCABAC28A, 0x53B39330, 0x24B4A3A6, 0xBAD03605, 0xCDD706B3, 0x54DE5729, 0x23D967BF,
        0xB3667A2E, 0xC4614AB8, 0x5D681B02, 0x2A6F2B94, 0xB40BBE37, 0xC30C8EA1, 0x5A05DF1B, 0x2D02EF8D
    };
    unsigned int crc = ~0u;
    int i;
    for (i=0; i < len; ++i)
        crc = (crc >> 8) ^ crc_table[buffer[i] ^ (crc & 0xff)];
    return ~crc;
}

static unsigned int stbiw__zhash(unsigned char *data)
{
    unsigned int hash = data[0] + (data[1] << 8) + (data[2] << 16);
    hash ^= hash << 3;
    hash += hash >> 5;
    hash ^= hash << 4;
    hash += hash >> 17;
    hash ^= hash << 25;
    hash += hash >> 6;
    return hash;
}

#define stbiw__wpng4(o,a,b,c,d) ((o)[0]=STBIW_UCHAR(a),(o)[1]=STBIW_UCHAR(b),(o)[2]=STBIW_UCHAR(c),(o)[3]=STBIW_UCHAR(d),(o)+=4)
#define stbiw__wp32(data,v) stbiw__wpng4(data, (v)>>24,(v)>>16,(v)>>8,(v));
#define stbiw__wptag(data,s) stbiw__wpng4(data, s[0],s[1],s[2],s[3])

static void stbiw__encode_png_line(unsigned char *pixels, int stride_bytes, int width, int height, int len, int comp, int filter_type, unsigned char *line_buffer)
{
    static int mapping[] = { 0,0,1,2,3 };
    static int first_row_filter[] = { 0,1,0,5,6 };
    int i;
    int sign = stride_bytes < 0 ? -1 : 1;
    int v;
    unsigned char *p = pixels + ((height-1) * abs(stride_bytes));
    int signed_stride = stride_bytes * sign;
    
    if (filter_type == 0) {
        for (i=0; i < len; ++i)
            line_buffer[i] = p[i];
        return;
    }
    
    for (i=0; i < comp; ++i)
        line_buffer[i] = p[i];
    
    for (i=comp; i < len; ++i) {
        v = p[i] - p[i-comp];
        line_buffer[i] = (unsigned char) v;
    }
}

int stbi_write_png(char const *filename, int x, int y, int comp, const void *data, int stride_bytes)
{
    FILE *f;
    int len;
    unsigned int crc;
    unsigned char *d;
    unsigned char *pixels;
    unsigned char *out;
    unsigned char *o;
    unsigned char *filt;
    unsigned char *zlib;
    unsigned char *line_buffer;
    int j, zlen;
    
    if (data == NULL) return 0;
    
    f = fopen(filename, "wb");
    if (!f) return 0;
    
    fprintf(f, "\211PNG\r\n\032\n");
    
    len = x * comp;
    filt = (unsigned char *) malloc(len + 1);
    line_buffer = (unsigned char *) malloc(len + 1);
    
    unsigned int ihdr_len = 13;
    fputc(0, f); fputc(0, f); fputc(0, f); fputc(13, f);
    fprintf(f, "IHDR");
    fputc((x >> 24) & 0xff, f);
    fputc((x >> 16) & 0xff, f);
    fputc((x >> 8) & 0xff, f);
    fputc(x & 0xff, f);
    fputc((y >> 24) & 0xff, f);
    fputc((y >> 16) & 0xff, f);
    fputc((y >> 8) & 0xff, f);
    fputc(y & 0xff, f);
    fputc(8, f);
    if (comp == 1) fputc(0, f);
    else if (comp == 2) fputc(4, f);
    else if (comp == 3) fputc(2, f);
    else fputc(6, f);
    fputc(0, f);
    fputc(0, f);
    fputc(0, f);
    
    crc = stbiw__crc32((unsigned char*)"IHDR", 4);
    unsigned char ihdr_data[13] = {
        (unsigned char)((x >> 24) & 0xff), (unsigned char)((x >> 16) & 0xff),
        (unsigned char)((x >> 8) & 0xff), (unsigned char)(x & 0xff),
        (unsigned char)((y >> 24) & 0xff), (unsigned char)((y >> 16) & 0xff),
        (unsigned char)((y >> 8) & 0xff), (unsigned char)(y & 0xff),
        8, (unsigned char)(comp == 1 ? 0 : (comp == 2 ? 4 : (comp == 3 ? 2 : 6))),
        0, 0, 0
    };
    crc = stbiw__crc32(ihdr_data, 13);
    fputc((crc >> 24) & 0xff, f);
    fputc((crc >> 16) & 0xff, f);
    fputc((crc >> 8) & 0xff, f);
    fputc(crc & 0xff, f);
    
    zlen = (len + 1) * y + 1024;
    zlib = (unsigned char *) malloc(zlen);
    
    int zlib_pos = 0;
    zlib[zlib_pos++] = 0x78;
    zlib[zlib_pos++] = 0x01;
    
    unsigned int bit_buf = 0;
    int bit_count = 0;
    
    pixels = (unsigned char *) data;
    if (stride_bytes == 0) stride_bytes = x * comp;
    
    for (j = 0; j < y; ++j) {
        unsigned char *p = pixels + j * stride_bytes;
        int i;
        
        filt[0] = 0;
        for (i = 0; i < len; ++i) {
            filt[i+1] = p[i];
        }
        
        for (i = 0; i <= len; ++i) {
            zlib[zlib_pos++] = filt[i];
            if (zlib_pos >= zlen - 1024) {
                zlen *= 2;
                zlib = (unsigned char *) realloc(zlib, zlen);
            }
        }
    }
    
    unsigned int adler = 1;
    for (j = 0; j < y; ++j) {
        unsigned char *p = pixels + j * stride_bytes;
        int i;
        adler = (adler + p[0]) % 65521;
        adler = (adler * 31 + 1) % 65521;
        for (i = 0; i < len; ++i) {
            adler = (adler + p[i]) % 65521;
            adler = (adler * 31 + adler) % 65521;
        }
    }
    
    zlib[zlib_pos++] = (adler >> 24) & 0xff;
    zlib[zlib_pos++] = (adler >> 16) & 0xff;
    zlib[zlib_pos++] = (adler >> 8) & 0xff;
    zlib[zlib_pos++] = adler & 0xff;
    
    unsigned int idat_len = zlib_pos;
    fputc((idat_len >> 24) & 0xff, f);
    fputc((idat_len >> 16) & 0xff, f);
    fputc((idat_len >> 8) & 0xff, f);
    fputc(idat_len & 0xff, f);
    fprintf(f, "IDAT");
    fwrite(zlib, 1, zlib_pos, f);
    
    crc = stbiw__crc32(zlib, zlib_pos);
    fputc((crc >> 24) & 0xff, f);
    fputc((crc >> 16) & 0xff, f);
    fputc((crc >> 8) & 0xff, f);
    fputc(crc & 0xff, f);
    
    fputc(0, f); fputc(0, f); fputc(0, f); fputc(0, f);
    fprintf(f, "IEND");
    crc = stbiw__crc32((unsigned char*)"IEND", 4);
    fputc((crc >> 24) & 0xff, f);
    fputc((crc >> 16) & 0xff, f);
    fputc((crc >> 8) & 0xff, f);
    fputc(crc & 0xff, f);
    
    free(filt);
    free(line_buffer);
    free(zlib);
    fclose(f);
    
    return 1;
}

int stbi_write_jpg(char const *filename, int x, int y, int comp, const void *data, int quality)
{
    (void)quality;
    return stbi_write_bmp(filename, x, y, comp, data);
}

#endif  
