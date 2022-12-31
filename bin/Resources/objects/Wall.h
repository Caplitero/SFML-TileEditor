#ifndef WALL_H_INCLUDED
#define WALL_H_INCLUDED


class WALL: public Object
{
   public:
   is_Cloneable;
   int x=0;
   int y=0;
   void onCreate(CAP::_Node * OBJ_Data)
   { 
     
     x=OBJ_Data->AttributesList["x"].toInt();
     y=OBJ_Data->AttributesList["y"].toInt();
   }
   
   void onUpdate(Environment *env)
   {
    std::cout<<x<<y<<"\n";  
   }
   
};

#endif // WALL_H_INCLUDED 
