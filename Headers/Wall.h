#ifndef WALL_H_INCLUDED
#define WALL_H_INCLUDED
#include "objects/_Object.h"

 class Wall : public _Object
{
public:
	int _x, _y = 0;
	Wall(int x, int y) {
		_x = x; _y = y;
	}
	Wall()
	{
		_x = 0; _y = 0;
	}
	static void onCreate(Environment* Data, CAP::_Node* new_Object)
	{
		int x = atof(new_Object->find_keyValue("x").c_str());
		int y = atof(new_Object->find_keyValue("x").c_str());
		Data->AllWalls.push_back(new Wall(x, y));
	}
};

#endif // WALL_H_INCLUDED 
