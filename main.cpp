#include <iostream>
#include "H_Objects_.h"

class App
{
    Environment Env;
    CAP::TileMapFile Current_Map;
    
    
   public:
   App(){}

   void Load_Map()
    {
       if(Current_Map.load("Resources\\","Map_example.tmx")==CAP::Successful)
       {
             for(auto & object : Current_Map.Data.ObjectLayers)
              for(auto item : object.Children)
               if(item->tag == "object") Env.addNewData(item);
       }
       if(Env.AllWalls.size())
        std::cout<<Env.AllWalls[0]->_x;
    }

   
};


int main(int, char**) {

   App myapp;
   myapp.Load_Map();
    
    
}
 