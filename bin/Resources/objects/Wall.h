#ifndef WALL_H_INCLUDED
#define WALL_H_INCLUDED


class WALL: public Object
{
   public:
   int x=0;int y=0;
   Object* onCreate(CAP::_Node * OBJ_Data)
   {
    WALL *newObject= new WALL;
    newObject->x= OBJ_Data->AttributesList["x"].toInt();
    newObject->y= OBJ_Data->AttributesList["y"].toInt();
    return newObject;
   }
   void onUpdate(Environment *env)
   {
   std::cout<<x<<y<<'\n';}
};

#endif // WALL_H_INCLUDED 
