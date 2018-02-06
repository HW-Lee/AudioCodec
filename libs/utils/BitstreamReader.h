#ifndef __BITSTREAMREADER_H__
#define __BITSTREAMREADER_H__

#include <iostream>
#include <fstream>

using namespace std;

class BitstreamReader {
public:
    ~BitstreamReader();
    BitstreamReader(const char* path) : BitstreamReader::BitstreamReader(new ifstream(path, ios::in|ios::binary|ios::ate)) {};
    BitstreamReader(string path) : BitstreamReader::BitstreamReader(path.c_str()) {};
    BitstreamReader(ifstream* ifs);

    void read(char* dst, int size);
    streamsize get_streamsize() { return this->size; }
    streamsize get_offset() { return this->offset; }


private:
    ifstream* file;
    streamsize size;
    streamsize offset;
};

#endif
