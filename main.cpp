#include <iostream>
#include "H_Objects_.h"

int main(int, char**) {
    CAP::Clock appClock;

    CAP::TileMapFile mymap;
    mymap.load("Resources\\","Map_Example.tmx");

    std::cout<<"Time : "<<appClock.elapsedTime()<<"ms";
}
