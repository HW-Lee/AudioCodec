#ifndef __FLACREADER_H__
#define __FLACREADER_H__

#include "BitstreamReader.h"
#include <vector>

#define FRAME_HEADER_BLOCK_SIZE_8_BIT 0x0006
#define FRAME_HEADER_BLOCK_SIZE_16_BIT 0x0007

#define FRAME_HEADER_SAMPLE_RATE_8_BIT 0x000C
#define FRAME_HEADER_SAMPLE_RATE_16_BIT 0x000D
#define FRAME_HEADER_SAMPLE_RATE_16_BIT_10X 0x000E

typedef enum
{
    META_BLOCK_STREAMINFO = 0,
    META_BLOCK_PADDING = 1,
    META_BLOCK_APPLICATION = 2,
    META_BLOCK_SEEKTABLE = 3,
    META_BLOCK_VORBIS_COMMENT = 4,
    META_BLOCK_CUESHEET = 5,
    META_BLOCK_PICTURE = 6,
    META_BLOCK_RESERVED_MIN = 7,
    META_BLOCK_RESERVED_MAX = 126,
    META_BLOCK_INVALID = 127
} block_t;

typedef enum
{
    CHANNEL_ASSIGN_1,
    CHANNEL_ASSIGN_2,
    CHANNEL_ASSIGN_3,
    CHANNEL_ASSIGN_4,
    CHANNEL_ASSIGN_5,
    CHANNEL_ASSIGN_6,
    CHANNEL_ASSIGN_7,
    CHANNEL_ASSIGN_8,
    CHANNEL_ASSIGN_LS_STEREO,
    CHANNEL_ASSIGN_RS_STEREO,
    CHANNEL_ASSIGN_MS_STEREO
} channel_assign_t;

typedef struct
{
    block_t data_type;
    uint32_t data_size;
    char* raw_data;
    void* extra;
} meta_block_t;

typedef struct
{
    uint16_t blocksize_min;
    uint16_t blocksize_max;
    uint32_t framesize_min;
    uint32_t framesize_max;
    uint32_t fs;
    uint8_t  nchannels;
    uint8_t  bits_per_sample;
    uint64_t nsamples_per_ch;
    uint8_t  md5_sign[16];
} block_stream_info_t;

typedef struct
{
    uint16_t sync_code;
    uint16_t block_size;
    uint32_t fs;
    channel_assign_t channel_assign;
    uint8_t bits_per_sample;
    uint64_t nidx;
    uint8_t crc8;
} frame_header_t;

class FLACReader
{
public:
    FLACReader(const char* path);

private:
    BitstreamReader* bsreader;
    vector<meta_block_t> meta_blocks;

    uint8_t _process_meta_block_header(uint32_t v, meta_block_t* block);
    void _process_block_stream_info(meta_block_t* block);
    bool _get_frame_header(frame_header_t* frameheader);
};

#endif
