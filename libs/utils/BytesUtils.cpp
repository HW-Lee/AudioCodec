#ifndef __BYTESUTILS_CPP__
#define __BYTESUTILS_CPP__

#include "BytesUtils.h"

void BytesUtils::switchEndianness(char* ptr, int size)
{
    for (int i = 0, j = size-1; i < j; i++, j--) {
        char tmp = ptr[i];
        ptr[i] = ptr[j];
        ptr[j] = tmp;
    }
}

#endif
