

#ifndef OBJECT_H_INCLUDED
#define OBJECT_H_INCLUDED
#include <string>
#include <typeinfo>

class Environment;
class Object
{
public:    
   Object(){}
   virtual Object* onCreate(CAP::_Node* OBJ_Data){}
   virtual void onUpdate(Environment *env){}
};

#endif //OBJECT_H_INCLUDED
