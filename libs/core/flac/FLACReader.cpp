#ifndef __FLACREADER_CPP__
#define __FLACREADER_CPP__

#include "FLACReader.h"
#include "BytesUtils.h"
#include <cstring>
#include <stdio.h>

uint16_t FRAME_HEADER_BLOCK_SIZE_TABLE[] = {
    0x0000, 192, 576, 1152, 2304, 4608, FRAME_HEADER_BLOCK_SIZE_8_BIT,
    FRAME_HEADER_BLOCK_SIZE_16_BIT, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768
};

uint32_t FRAME_HEADER_SAMPLE_RATE_TABLE[] = {
    0x0000, 88200, 176400, 192000, 8000, 16000, 22050,
    24000, 32000, 44100, 48000, 96000, FRAME_HEADER_SAMPLE_RATE_8_BIT,
    FRAME_HEADER_SAMPLE_RATE_16_BIT, FRAME_HEADER_SAMPLE_RATE_16_BIT_10X, 0x000F
};

uint8_t FRAME_HEADER_BPS_TABLE[] = {
    0x00, 8, 12, 0x03, 16, 20, 24, 0x07
};

FLACReader::FLACReader(const char* path)
    : bsreader(new BitstreamReader(path))
{
    char marker[5] = {0};
    this->bsreader->read(marker, 4);
    cout << marker << endl;

    uint32_t meta_block_header;
    uint8_t is_last = 0;

    while (!is_last) {
        meta_block_t meta = {META_BLOCK_INVALID, 0, 0, 0};
        this->bsreader->read((char*) &meta_block_header, 4);
        is_last = this->_process_meta_block_header(meta_block_header, &meta);
        this->bsreader->read(meta.raw_data, meta.data_size);
        if (meta.data_type == META_BLOCK_STREAMINFO) {
            this->_process_block_stream_info(&meta);
        }
        this->meta_blocks.push_back(meta);
    }

    for (int i = 0; i < this->meta_blocks.size(); i++) {
        meta_block_t meta = this->meta_blocks[i];
        cout << (uint16_t) meta.data_type << endl;
        cout << (uint16_t) meta.data_size << endl;
        if (meta.data_type == META_BLOCK_STREAMINFO) {
            cout << "-------------------------------" << endl;
            block_stream_info_t* strinfo = (block_stream_info_t*) meta.extra;
            cout << "blocksize_min: " << (uint64_t) strinfo->blocksize_min << endl;
            cout << "blocksize_max: " << (uint64_t) strinfo->blocksize_max << endl;
            cout << "framesize_min: " << (uint64_t) strinfo->framesize_min << endl;
            cout << "framesize_max: " << (uint64_t) strinfo->framesize_max << endl;
            cout << "fs: " << (uint64_t) strinfo->fs << endl;
            cout << "nchannels: " << (uint64_t) strinfo->nchannels << endl;
            cout << "bits_per_sample: " << (uint64_t) strinfo->bits_per_sample << endl;
            cout << "nsamples_per_ch: " << (uint64_t) strinfo->nsamples_per_ch << endl;
        }
        cout << endl;
    }

    frame_header_t first = {0, 0, 0, CHANNEL_ASSIGN_1, 0, 0, 0};
    while (this->bsreader->get_offset() < this->bsreader->get_streamsize()) {
        frame_header_t frameheader = {0, 0, 0, CHANNEL_ASSIGN_1, 0, 0, 0};
        if (!this->_get_frame_header(&frameheader))
            break;
        if (first.sync_code == 0)
            first = frameheader;
        if (first.sync_code != frameheader.sync_code ||
            first.block_size != frameheader.block_size ||
            first.fs != frameheader.fs ||
            first.channel_assign != frameheader.channel_assign ||
            first.bits_per_sample != frameheader.bits_per_sample)
            continue;
        printf("frameheader.sync_code = 0x%x\n", frameheader.sync_code);
        printf("frameheader.block_size = %d\n", frameheader.block_size);
        printf("frameheader.fs = %d\n", frameheader.fs);
        printf("frameheader.channel_assign = 0x%x\n", frameheader.channel_assign);
        printf("frameheader.bits_per_sample = %d\n", frameheader.bits_per_sample);
        printf("frameheader.nidx = 0x%lx (%ld)\n", frameheader.nidx, frameheader.nidx);
        cout << "----------------------------------------------------" << endl;
    }
}

uint8_t FLACReader::_process_meta_block_header(uint32_t v, meta_block_t* block)
{
    BytesUtils::switchEndianness((char*) &v, 4);
    uint8_t is_last = (v >> 24) & 0x000000FF;
    block->data_type = (block_t) (is_last & 0x7F);
    block->data_size = v & 0x00FFFFFF;
    block->raw_data = new char[block->data_size];

    return is_last >> 7;
}

void FLACReader::_process_block_stream_info(meta_block_t* block)
{
    if (block->data_type != META_BLOCK_STREAMINFO)
        return;

    block->extra = new block_stream_info_t[1];
    block_stream_info_t* strinfo = (block_stream_info_t*) block->extra;

    char* ptr = block->raw_data;
    memcpy(&strinfo->blocksize_min, ptr, 2);
    ptr += 2;
    BytesUtils::switchEndianness((char*) &strinfo->blocksize_min, 2);
    memcpy(&strinfo->blocksize_max, ptr, 2);
    ptr += 2;
    BytesUtils::switchEndianness((char*) &strinfo->blocksize_max, 2);

    memcpy((char*) &strinfo->framesize_min + 1, ptr, 3);
    ptr += 3;
    BytesUtils::switchEndianness((char*) &strinfo->framesize_min, 4);
    memcpy((char*) &strinfo->framesize_max + 1, ptr, 3);
    ptr += 3;
    BytesUtils::switchEndianness((char*) &strinfo->framesize_max, 4);
    strinfo->framesize_min &= 0x00FFFFFF;
    strinfo->framesize_max &= 0x00FFFFFF;

    uint32_t v;
    memcpy(&v, ptr, 4);
    ptr += 4;
    BytesUtils::switchEndianness((char*) &v, 4);
    strinfo->fs = (v >> 12) & 0x000FFFFF;
    strinfo->nchannels = ((v & 0x00000E00) >> 9) + 1;
    strinfo->bits_per_sample = ((v & 0x000001F0) >> 4) + 1;

    memcpy((char*) &strinfo->nsamples_per_ch + 4, ptr, 4);
    ptr += 4;
    BytesUtils::switchEndianness((char*) &strinfo->nsamples_per_ch, 8);
    strinfo->nsamples_per_ch = (strinfo->nsamples_per_ch & 0x00000000FFFFFFFF) | (uint64_t) (v << 28);

    memcpy(&strinfo->md5_sign, ptr, 16);
}

bool FLACReader::_get_frame_header(frame_header_t* frameheader)
{
    bool ret = false;
    unsigned char c;
    while (this->bsreader->get_offset() < this->bsreader->get_streamsize()) {
        this->bsreader->read((char*) &c, 1);
        if (c == 0xFF) {
            frameheader->sync_code = 0xFF00;
            this->bsreader->read((char*) &c, 1);
            if ((c >> 3) == 0x1F) {
                frameheader->sync_code |= (c & 0x00FF);
                this->bsreader->read((char*) &frameheader->block_size, 1);
                frameheader->fs = frameheader->block_size & 0x0F;
                frameheader->block_size >>= 4;
                frameheader->fs = FRAME_HEADER_SAMPLE_RATE_TABLE[frameheader->fs];
                frameheader->block_size = FRAME_HEADER_BLOCK_SIZE_TABLE[frameheader->block_size];
                this->bsreader->read((char*) &c, 1);
                frameheader->channel_assign = (channel_assign_t) (c >> 4);
                frameheader->bits_per_sample = FRAME_HEADER_BPS_TABLE[(c >> 1) & 0x07];

                this->bsreader->read((char*) &c, 1);
                uint8_t count = 0;
                while (c & 0x80) {
                    c <<= 1;
                    count++;
                }
                c >>= count;

                uint8_t mask = ~(((uint8_t) 0xFF) << (7 - count));
                frameheader->nidx |= (mask & c);

                for (int i = 0; i < count-1; i++) {
                    frameheader->nidx <<= 6;
                    this->bsreader->read((char*) &c, 1);
                    frameheader->nidx |= (c & 0x3F);
                }

                if (frameheader->block_size == FRAME_HEADER_BLOCK_SIZE_8_BIT) {
                    this->bsreader->read((char*) &frameheader->block_size, 1);
                } else if (frameheader->block_size == FRAME_HEADER_BLOCK_SIZE_16_BIT) {
                    this->bsreader->read((char*) &frameheader->block_size, 2);
                    BytesUtils::switchEndianness((char*) &frameheader->block_size, 2);
                }

                if (frameheader->fs == FRAME_HEADER_SAMPLE_RATE_8_BIT) {
                    this->bsreader->read((char*) &frameheader->fs, 1);
                } else if (frameheader->fs == FRAME_HEADER_SAMPLE_RATE_16_BIT ||
                           frameheader->fs == FRAME_HEADER_SAMPLE_RATE_16_BIT_10X) {
                    bool is10x = (frameheader->fs == FRAME_HEADER_SAMPLE_RATE_16_BIT_10X);
                    this->bsreader->read((char*) &frameheader->fs, 2);
                    BytesUtils::switchEndianness((char*) &frameheader->fs, 2);

                    if (is10x)
                        frameheader->fs *= 10;
                }

                this->bsreader->read((char*) &frameheader->crc8, 1);

                return true;
            }
        }
    }

    return false;
}

#endif
