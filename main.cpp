#include <iostream>
#include "H_Objects_.h"

int main(int, char**) {
    CAP::TileMapFile mymap;
    mymap.load("","Ma-_Example.tmx");
    std::cout << "Hello, world!\n";
}
