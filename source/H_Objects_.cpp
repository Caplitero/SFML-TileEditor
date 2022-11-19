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
	std::vector<_Object*> NewDataSheet;
	NewDataSheet.resize(1024);
	for (auto& T : DataSheet)
		NewDataSheet[HashConversion(T->get_Class())] = T;
		
	DataSheet = NewDataSheet;
	
	
}

void All_objects_Class::add_newItem(std::string Class,CAP::_Node* Child)
{
	_Object *Item = DataSheet[HashConversion(Class)];
	auto lambda = Item->get_Init();

	if (Item != nullptr )
	{
		lambda(Child);
		
	}
}



