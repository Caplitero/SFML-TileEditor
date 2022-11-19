#pragma once
#include "TileMapLoader.h"
#include <iostream>
#include <vector>
#include <functional>



class _Object
{
	

    public:
		virtual void Init(CAP::_Node* Data ) 
		{ std::cout << "empty Init"; }
		std::function<void(CAP::_Node*Data )> get_Init()
		{
			return [&](CAP::_Node* Data ) {Init(Data);};
		};
		virtual std::string get_Class()
		{
			return "_Object";
		}
		_Object(){}
	
};


class Walls :public _Object
{
	virtual void Init(CAP::_Node* Data )
	{
		std::cout << "";
		
	}
public:

	int x ;
	int y ;

	virtual std::string get_Class()
	{
		return "wall";
	}

	Walls()
	{
		x = y = 0;
	}
	Walls(int _x, int _y)
	{
		x = _x; y = _y;
	}


};


class Enemy :public _Object
{
	
	virtual void Init(CAP::_Node* Data);

public:
	
	float x;
	float y;
	virtual std::string get_Class()
	{
		return "enemy";
	}
	virtual int get_Id()
	{
		return 2;
		
	}

	Enemy()
	{
		x = y = 0;
	}
	Enemy(int _x, int _y)
	{   
		x = _x; y = _y;
	}


};


static class ALLOBJECTS {
public:
	std::vector<Enemy> ENEMIES;
}GIVEMMEDATA;

inline void Enemy::Init(CAP::_Node * Data)
{
	std::cout << "new enemy is created";
	float nx = atof(Data->find_keyValue("x").c_str());
	float ny = atof(Data->find_keyValue("y").c_str());
	Enemy NewN(nx, ny);
	GIVEMMEDATA.ENEMIES.push_back(NewN);
}


class All_objects_Class
{
    

	std::vector < _Object* > DataSheet =
	{
		dynamic_cast<_Object*>(new Walls()),
		dynamic_cast<_Object*>(new Enemy())
	};
	
	void init_OBJ_vec();
public:
	
	All_objects_Class() { init_OBJ_vec();}
	void add_newItem(std::string Class, CAP::_Node* Child);
};
