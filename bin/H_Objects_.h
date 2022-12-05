
#include "TileMapLoader.h"
#include <iostream>
#include <vector>
#include <functional>


////Predefined classes/////
class _Object;
class Enemy;
class Wall;



 class Environment
{
	 static std::vector<std::function<void(Environment* Data,
		 CAP::_Node* new_Object)>> DataSheet;
public:

	/////Write your vectors here/////
	
       std::vector<Wall*> AllWalls;
	/*<----------------------------->*/

	void addNewData(CAP::_Node *new_Object)
	{
		std::string CLASS = new_Object->find_keyValue("class");
		if (atoi(CLASS.c_str()) < DataSheet.size() && !CLASS.empty())
		{
			DataSheet[atoi(CLASS.c_str())](this, new_Object);
		}
	}
    
	virtual ~Environment()
	{
	   
	}
};

/////Include all your OBJECTS/////
#include "Resources/objects/Wall.h"
#include "Resources/objects/_Object.h"
 //////////////////////////////////

 std::vector<std::function< void(Environment * Data, 
	 CAP::_Node * new_Object)>> Environment::DataSheet = {
	 ///Add your class Constructors here///
		Wall::onCreate
	 
	 //////////////////////////////////////
 };