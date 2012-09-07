#ifndef _factory_h
#define _factory_h

#include "Initializer.h"
#include "TypeString.hxx"

#include <map>
#include <string>

namespace Factory {
	class Object {
	public:
		virtual ~Object() {}
	};

	Object* CallCreateMethod(const std::string& type);
	void Declare(const std::string& type, Factory::Object* (*create)());

	template <class Interface>
	Interface* Create(const std::string& type) {
		Factory::Object* obj=CallCreateMethod(type);
		Interface* intobj=dynamic_cast<Interface*>(obj);
		if (!intobj && obj) delete obj;
		return intobj;
	}

	template <class Type>
	Type* Create() {
		return Create<Type>(getTypeName<Type>());
	}

} // namespace Factory

#define DECLARE_TYPE(TYPE)												\
template<> inline std::string getTypeName<TYPE>() { return #TYPE; }		\
Factory::Object* Create##TYPE() {										\
	return new TYPE;													\
}																		\
INITIALIZER(Register##TYPE) {											\
	Factory::Declare(#TYPE,Create##TYPE);								\
}

#endif // _factory_h
