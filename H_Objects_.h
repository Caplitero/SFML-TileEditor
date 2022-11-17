#pragma once
#include "TileMapLoader.h"
#include <iostream>
#include <vector>
#include <functional>

class Walls_class
{
public:
	float X; float Y;
	Walls_class(float x, float y)
	{
		X = x;
		Y = y;
	}
};

class All_objects_Class
{

	std::vector < std::tuple<std::string, std::function<void(CAP::_Node* Data)>> > Obj_List =
	{
		{"wall",[&](CAP::_Node* Data) {
			float X = atof(Data->find_keyValue("x").c_str());
	        float Y = atof(Data->find_keyValue("y").c_str());
			Walls_class car(X,Y);
	    All_Objects.push_back(car); 
	                                 }
		}

	};
   
	void init_OBJ_vec();
public:
	std::vector<Walls_class> All_Objects;
	All_objects_Class() { init_OBJ_vec();}
	void add_newItem(std::string Class, CAP::_Node* Child);
};

