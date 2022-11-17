#include "H_Objects_.h"

long int HashConversion(std::string Class)
{
	long int t_size = 1e3 + 7;
	int hash_value = 37;
	long long int hash = 0;
	for (int i = 0; i < Class.length(); i++)
		hash = hash * hash_value + Class[i];
	
	return hash % t_size;
}

void All_objects_Class::init_OBJ_vec()
{
	std::vector < std::tuple<std::string, std::function<void(CAP::_Node* Data)>> > HashingTable;
	HashingTable.resize(1024);
	for (auto& T : Obj_List)
		HashingTable[HashConversion(std::get<0>(T))] = T;
	Obj_List = HashingTable;

}

void All_objects_Class::add_newItem(std::string Class,CAP::_Node* Child)
{
	
	auto lambda =std::get<1>(Obj_List[HashConversion(Class)]);
	if (lambda != nullptr)
	{
		lambda(Child);
		
	}
}
