#include <iostream>
#include "FLACReader.h"

int main(int argc, char* argv[])
{
    FLACReader* f = new FLACReader(argv[1]);
    return 0;
}
