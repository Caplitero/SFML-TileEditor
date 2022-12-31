

#ifndef OBJECT_H_INCLUDED
#define OBJECT_H_INCLUDED
#include <string>
#include <typeinfo>
#include <memory>



class Environment;
class Object
{
   std::string _Class_Name="\0";
public:    
   
   void __init__()
   { std::string ID = typeid(*this).name();
     uint8_t Iter= 0;
     while(isdigit(ID[Iter]))Iter++;
    _Class_Name=ID.substr(Iter); 
   }

   // Return the name of the class
   std::string get_Class(){return _Class_Name;}

    virtual void onCreate(CAP::_Node* OBJ_Data)=0;
    virtual void onUpdate(Environment *env)=0;
    virtual Object* clone() const {return nullptr;};

};

#define is_Cloneable Object* clone() const override {return new std::remove_const_t<std::remove_pointer_t<decltype(this)>>(*this); }

#endif //OBJECT_H_INCLUDED
