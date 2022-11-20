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

	Enemy()
	{
		x = y = 0;
	}
	Enemy(int _x, int _y)
	{   
		x = _x; y = _y;
	}


};


static class Environment_Init
{
	std::vector < _Object* > DataSheet=
		{
		dynamic_cast<_Object*>(new Walls()),
		dynamic_cast<_Object*>(new Enemy()) };

public:
	
	Environment_Init() { }
	
	void add_newItem(CAP::_Node* Child)
	{
		std::string CLASS = Child->find_keyValue("class");
		if (atoi(CLASS.c_str()) < DataSheet.size() && !CLASS.empty())
		{
			_Object* Item = DataSheet[atoi(CLASS.c_str())];
			auto lambda = Item->get_Init();
            lambda(Child);

		}
	}

    ~Environment_Init()
	{
		for (auto Item : DataSheet)
			delete(Item);
	}
};

static class All_Objects_Vec {
public:
	std::vector<Enemy> ENEMIES;
}Env_data;


inline void Enemy::Init(CAP::_Node* Data)
{
	float nx = atof(Data->find_keyValue("x").c_str());
	float ny = atof(Data->find_keyValue("y").c_str());
	Enemy NewN(nx, ny);
	Env_data.ENEMIES.push_back(NewN);
	
}

