

#ifndef OBJECT_H_INCLUDED
#define OBJECT_H_INCLUDED


class _Object
{
public:
	_Object() {

	}
	static void onCreate(Environment* Data,
		CAP::_Node* new_Object) {}
	virtual void onUpdate() {}
    std::function<void(Environment* Data,
		CAP::_Node* new_Object)> getConstructor()
	{
		return [&](Environment* Data,
			CAP::_Node* new_Object) {onCreate(Data,new_Object); };
	}
	virtual ~_Object() {}
};

#endif //OBJECT_H_INCLUDED
