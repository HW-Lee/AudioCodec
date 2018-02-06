#ifndef __BITSTREAMREADER_CPP__
#define __BITSTREAMREADER_CPP__

#include "BitstreamReader.h"

BitstreamReader::BitstreamReader(ifstream* ifs)
    : file(ifs),
      offset(0)
{
    if (!this->file->is_open()) {
        cout << "error for opening the file." << endl;
        return;
    }

    this->size = this->file->tellg();
    this->file->seekg(0, this->file->beg);
}

void BitstreamReader::read(char* dst, int size)
{
    this->file->read(dst, size);
    this->offset += size;
}

#endif
