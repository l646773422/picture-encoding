#include <stdint.h>
#include <stdlib.h> 
#include <memory.h>

#define BITS_SIZE 16
#define QUANTIZATION_TABLE_SIZE  64 // 8*8 block
#define BLOCK_COLUMN 8
#define BLOCK_ROW 8
#define BLOCK_PIXELS 64
#define PI 3.14159265358979323846

static const unsigned char zigzag[64] =
{
    0,   1,  5,  6, 14, 15, 27, 28,
    2,   4,  7, 13, 16, 26, 29, 42,
    3,   8, 12, 17, 25, 30, 41, 43,
    9,  11, 18, 24, 31, 40, 44, 53,
    10, 19, 23, 32, 39, 45, 52, 54,
    20, 22, 33, 38, 46, 51, 55, 60,
    21, 34, 37, 47, 50, 56, 59, 61,
    35, 36, 48, 49, 57, 58, 62, 63
};

typedef void Void;
typedef uint8_t pix;
typedef uint8_t byte;

typedef uint32_t bs_t;

typedef struct stream
{
    int      shift;   // bit position in the cache
    uint32_t cache;   // bit cache
    bs_t     *buf;    // current position
    bs_t     *origin; // initial position
}stream;

typedef enum coef_type{
    DC=0, AC, DEFAULT
}coef_type;

//typedef struct huffman_table{
//    uint8_t tabel_idx;
//    coef_type type;
//    uint8_t  *bits;
//    uint8_t  *shift_arr;  // array, elements like [0, L1, L1+L2, L1+L2+L3, ...]
//    uint8_t  *huff_size;
//    uint8_t  *huff_code;
//    size_t last_key;
//}huffman_table;

typedef struct bit_value {
    // ������AC����DC��huffman table���ݣ���valueһ�����Ҳ��д16bit
    int length;
    int code;
}bit_value;

//typedef bit_stream huffman_table;

typedef struct quantization_table{
    uint8_t coefs[8*8];
}quantization_table;

typedef struct frame_header{
    uint8_t hf_tables; // in baseline, only support 4 huffman tables.
    bit_value DC_luma_table[12];
    bit_value DC_chroma_table[12];
    bit_value AC_luma_table[256];
    bit_value AC_chroma_table[256];

    //uint8_t qz_tables;
    quantization_table luma_quantization_table;
    quantization_table chroma_quantization_table;
}frame_header;

static Void init_bit_stream(stream* bs)
{
    memset(bs, 0, sizeof(stream));
    bs->origin = (bs_t*)malloc(sizeof(bs_t) * 512);
    memset(bs->origin, 0, sizeof(bs_t) * 512);

    bs->shift = 32;
    bs->buf = bs->origin;
}

static Void clear_bit_stream(stream* bs)
{
    bs->buf = bs->origin;
    bs->cache = 0;
    bs->shift = 0;
}

static Void destory_bit_stream(stream* bs)
{
    free(bs->origin);
    memset(bs, 0, sizeof(stream));
}

static Void flush_stream(stream* bs)
{

}