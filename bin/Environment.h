#ifndef ENV_INCLUDED
#define ENV_INCLUDED

#include "TileMapLoader.h"
#include "Resources/objects/Base_Object.h"

#include <algorithm>
#include <unordered_map>
#include <functional>

class Environment{
      std::vector<Object*> ALL_OBJECTS;
   public :
   
   Environment(){ALL_OBJECTS.reserve(128);}
   void add_OBJ(CAP::_Node * NewObject);

   template <typename T>
   std::vector<T*> getObjects()
   {  
      std::string TypeName= typeid(T).name();
      std::vector<T*> newVEC;

      for(Object * item : ALL_OBJECTS) 
         if(TypeName==typeid(*item).name())
         { T* newObject = static_cast<T*>(item);
           newVEC.push_back(newObject);
           }
      return newVEC; 
   }

   void update()
   {
      for(Object * Curret_Object:ALL_OBJECTS)
         Curret_Object->onUpdate(this);
   }
    
    
};

#include "Resources/objects/Objects_Header.h"

void Environment::add_OBJ(CAP::_Node * Object_Data)
{    
     std::string CLASS  =  Object_Data->AttributesList["class"].toString();
     Object*     Parent =  Class_Constr[CLASS];
     ALL_OBJECTS.push_back(Parent->onCreate(Object_Data));
}

#endif 

