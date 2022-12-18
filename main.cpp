

#include <iostream>
#include "bin\Environment.h"

class App
{
    Environment Env;
    CAP::TileMapFile Current_Map;
    std::string Resources_Path = "bin\\Resources\\";
    
   public:
   App(){}

   bool Load_Map()
    {  
        
       if(Current_Map.load(Resources_Path,"Map_example.tmx")==CAP::Successful)
       {
             for(auto & object : Current_Map.Data.ObjectLayers)
              for(auto item : object.Children)
               if(item->tag == "object"){
                Env.add_OBJ(item);}

            Env.update();
       }
       
    }

   ~App()
   {
    std::for_each (Class_Constr.begin(), Class_Constr.end(), [](auto item) -> void
     {delete item.second;});
    Class_Constr.clear();
     }
   
};

int main(int, char**) {

   App myapp;
   myapp.Load_Map();
    
}
 