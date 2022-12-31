#ifndef ENV_INCLUDED
#define ENV_INCLUDED

#include "TileMapLoader.h"
#include "Resources/objects/Base_Object.h"

#include <memory>
#include <cstring>
#include <map>
#include <algorithm>
#include <functional>



// Initialize classes used to create objects
template<class U>
std::pair<std::string,std::shared_ptr<Object>> add_Class()
{
   std::shared_ptr<Object> ptr(new U);
   ptr->__init__();
   return std::make_pair(ptr.get()->get_Class(),ptr);
}

class Environment{
      std::vector<std::shared_ptr<Object>> ALL_OBJECTS;
      
   public :
   
   Environment(){ALL_OBJECTS.reserve(1000001);}
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
      for(auto item : ALL_OBJECTS)
        item->onUpdate(this);
      
   }
    
    
};

#include "Resources/objects/Objects_Header.h"

void Environment::add_OBJ(CAP::_Node * Object_Data)
{    
     std::string CLASS  =  Object_Data->AttributesList["class"].toString();
     Object*     Parent =  Class_Constr[CLASS].get(); 
     std::shared_ptr<Object> Child(Parent->clone()); 
     Child->onCreate(Object_Data);

     ALL_OBJECTS.emplace_back(Child);
     
}

#endif 

