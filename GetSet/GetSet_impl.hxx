//
//  Library: GetSet
//  c++ library for load/saving *typed* and *named* properties and automatic GUI.
//  
//  Copyright (c) by André Aichert (aaichert@gmail.com)
//    
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//  
//    http://www.apache.org/licenses/LICENSE-2.0
//    
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
//

#ifndef __GetSetSpecial_hxx
#define __GetSetSpecial_hxx

#include "GetSet.hxx"

// 
// GetSetGui::Section
// 

namespace GetSetGui {

	Section::Section(const std::string& relative_path, GetSetGui::Section super_section)
		: node(((GetSetInternal::Section&)super_section).createSection(relative_path))
	{}

	bool Section::isCollapsible() const { return node.getAttribute<bool>("Collapsed") || node.getAttribute<bool>("Collapsible") ;}
	
	Section& Section::setCollapsible(bool collapsible) { node.setAttribute<bool>("Collapsible",collapsible); if (!collapsible) node.setAttribute<bool>("Collapsed",false); return *this;}

	template <typename KeyType, typename BasicType>
	GetSetInternal::Node& Section::declare(const std::string& relative_path, bool forceType, const BasicType& default_value) const
	{
		// See if a node at absolute_path exists
		GetSetInternal::Node* new_node=node.nodeAt(relative_path);
		// Check for an exact match of key type
		KeyType* typed_node=dynamic_cast<KeyType*>(new_node);
		if (typed_node) return *typed_node;
		// if node exists and it is of type string, we would also like to forceType
		if	( (!forceType&&new_node&&new_node->getType()==typeName<std::string>())
			  ||(new_node&&new_node->getType()=="Section"))
				forceType=true;
		// Check if we need to create a new key
		if (!new_node || (!typed_node && forceType))
		{
			// Get path to super section and key name
			std::string path_to_super, key;
			// Split right
			std::string::size_type loc=relative_path.find_last_of("/");
			if (loc!=std::string::npos) {
				key=relative_path.substr(loc+1,std::string::npos);
				path_to_super=relative_path.substr(0,loc); // left
			}
			else key=relative_path;
			// Find super section and insert new node
			GetSetInternal::Section& super_section=node.createSection(path_to_super);
			new_node=new KeyType(super_section,key,default_value);
			super_section.insertNode(*new_node);
		}
		return *new_node;
	}

} // namespace GetSetGui

//
// GetSet<...>
//

template <typename BasicType>
GetSet<BasicType>::GetSet(const std::string& key, const GetSetGui::Section& section, const BasicType& default_value)
	: node(section.declare<GetSetInternal::Key<BasicType>, BasicType>(key,false,default_value))
	, typed_node(dynamic_cast<GetSetInternal::Key<BasicType>*>(&node))
{}

template <typename BasicType>
GetSet<BasicType>& GetSet<BasicType>::setValue(const BasicType& v)
{
	if (typed_node) typed_node->setValue(v);
	else node.setString(toString(v));
	return *this;
}

template <typename BasicType>
BasicType GetSet<BasicType>::getValue() const
{
	if (typed_node) return typed_node->getValue();
	else return stringTo<BasicType>(node.getString());
}

template <typename BasicType>
GetSet<BasicType>& GetSet<BasicType>::setString(const std::string& value)
{
	node.setString(value);
	return *this;
}

template <typename BasicType>
std::string GetSet<BasicType>::getString() const { return node.getString(); }

template <typename BasicType>
inline void GetSet<BasicType>::discard() { node.super().removeNode(node.name); }

template <typename BasicType>
inline GetSetGui::Section GetSet<BasicType>::supersection() const { return GetSetGui::Section(node.super_section, node.dictionary); }

template <typename BasicType>
inline GetSet<BasicType>& GetSet<BasicType>::operator=(const BasicType& v) { setValue(v); return *this; }

template <typename BasicType>
inline GetSet<BasicType>::operator BasicType() const { return getValue(); }

template <typename BasicType>
GetSet<BasicType>::GetSet(GetSetInternal::Node& _node) : node(_node), typed_node(0x0) {}

//
// DEFINITION OF SPECIAL TYPES Enum, Button, File, ect.
//

/// Specializations for GUI representations
#define GETSET_SPECIALIZATION(SPECIAL_TYPE,BASE_TYPE,CLASS_BODY,KEY_BODY)								\
	namespace GetSetInternal																			\
	{																									\
		class Key##SPECIAL_TYPE : public GetSetInternal::Key<BASE_TYPE>									\
		{																								\
		public:																							\
			Key##SPECIAL_TYPE(Section& _section, const std::string& _key,								\
							  const BASE_TYPE& dflt_val=default_value<BASE_TYPE>())						\
				: Key<BASE_TYPE>(_section,_key,dflt_val) {}												\
			virtual std::string getType() const { return #SPECIAL_TYPE; }								\
			KEY_BODY																					\
		};																								\
	}																									\
	namespace GetSetGui																					\
	{																									\
		class SPECIAL_TYPE : public GetSet<BASE_TYPE>													\
		{																								\
		public:																							\
			SPECIAL_TYPE(const std::string& k, const GetSetGui::Section& s = GetSetGui::Section(),		\
						 const BASE_TYPE& v=default_value<BASE_TYPE>())									\
				: GetSet<BASE_TYPE>(s.declare<GetSetInternal::Key##SPECIAL_TYPE,BASE_TYPE>(k,true,v))	\
			{																							\
				typed_node=static_cast<GetSetInternal::Key<BASE_TYPE>*>(&node);							\
			}																							\
			GetSet<BASE_TYPE>& operator=(const BASE_TYPE& v) { typed_node->setValue(v); return *this; }	\
			operator BASE_TYPE() const { return typed_node->getValue(); }								\
			CLASS_BODY																					\
		};																								\
	}
// end of GETSET_SPECIALIZATION

// Use pre-processor to define several specialized GetSetGui:: and GetInternal::Key types.
// Each of these classes had between 50 and 100 LOC if expanded and formatted.

#define GETSET_ENUM_CLASS_BODY																			\
	GETSET_TAG(Enum,std::vector<std::string>,Choices)													\
	Enum& setChoices(const std::string& c) { node.setAttribute<>("Choices",c); return *this; }			\
	inline GetSet<int>& operator=(const std::string& v) { node.setString(v); return *this; }			\
	inline operator std::string() const { return node.getString(); }

#define GETSET_ENUM_KEY_BODY																			\
	virtual std::string getString() const																\
	{																									\
		auto choices=getAttribute<std::vector<std::string> >("Choices");								\
		int index=getValue();																			\
		if (choices.empty()) toString(index);															\
		if (index<0||index>=(int)choices.size()) return toString(index); else return choices[index];	\
	}																									\
	virtual void setString(const std::string& in)														\
	{																									\
		auto choices=getAttribute<std::vector<std::string> >("Choices");								\
		if (choices.empty()) setValue(stringTo<int>(in));												\
		for (int i=0;i<(int)choices.size();i++) if (choices[i]==in) { setValue(i); return; }			\
		setValue(stringTo<int>(in));																	\
	}

#define GETSET_BUTTON_KEY_BODY																			\
	void (*callback)(const std::string& info, void* user_data);											\
	std::string	caller_info;																			\
	void*		caller_data;																			\

#define GETSET_BUTTON_CLASS_BODY																		\
	virtual GetSet<std::string>& setString(const std::string& in)										\
	{																									\
		GetSet<std::string>::setString(in);																\
		trigger();																						\
		return *this;																					\
	}																									\
	virtual GetSet<std::string>& setValue(const std::string& in)										\
	{																									\
		trigger(false);																					\
		node.setString(in);																				\
		return *this;																					\
	}																									\
	Button& setCallback(void (*c)(const std::string&, void*), const std::string& info, void* data)		\
	{																									\
		auto *exactlyTypedNode=dynamic_cast<GetSetInternal::KeyButton*>(&node);							\
		exactlyTypedNode->caller_info=info;																\
		exactlyTypedNode->caller_data=data;																\
		exactlyTypedNode->callback=exactlyTypedNode->caller_info.empty()?0x0:c;							\
		return *this;																					\
	}																									\
	void trigger(bool signal_change=true)																\
	{																									\
		auto *exactlyTypedNode=dynamic_cast<GetSetInternal::KeyButton*>(&node);							\
		if (!exactlyTypedNode->caller_info.empty())														\
			exactlyTypedNode->callback(																	\
				exactlyTypedNode->caller_info,															\
				exactlyTypedNode->caller_data);															\
		if (signal_change) node.signalChange();															\
	}

/// A pulldown menu with a number of choices.
GETSET_SPECIALIZATION(Enum,int,GETSET_ENUM_CLASS_BODY, GETSET_ENUM_KEY_BODY)

/// A GetSet&lt;double&gt; with additional range information, so that it could be represented as a slider
GETSET_SPECIALIZATION(Slider,double, GETSET_TAG(Slider,double,Min) GETSET_TAG(Slider,double,Max), )

/// A GetSet&lt;double&gt; with additional range information, so that it could be represented as a SpinBox
GETSET_SPECIALIZATION(RangedDouble,double, GETSET_TAG(RangedDouble,double,Min) GETSET_TAG(RangedDouble,double,Step) GETSET_TAG(RangedDouble,double,Max) GETSET_TAG_BOOL(RangedDouble,Periodic) , )

/// A GetSet&lt;int&gt; with additional range information, so that it could be represented as a SpinBox
GETSET_SPECIALIZATION(RangedInt,int, GETSET_TAG(RangedInt,int,Min) GETSET_TAG(RangedInt,int,Step) GETSET_TAG(RangedInt,int,Max) GETSET_TAG_BOOL(RangedInt,Periodic) , )

/// A button that creates a GetSet change event when pressed.
GETSET_SPECIALIZATION(Button,std::string,GETSET_BUTTON_CLASS_BODY, GETSET_BUTTON_KEY_BODY)

/// A static text with some information. StaticTexts are not included in ini-Files (user-info in GUI)
GETSET_SPECIALIZATION(StaticText,std::string, , )

/// An edit field, but read-only. Intended for output-values that the user can select and copy to clipboard.
GETSET_SPECIALIZATION(ReadOnlyText,std::string, , )

/// A directory
GETSET_SPECIALIZATION(Directory,std::string, , )

/// A file (or multiple semicolon seperated files). Extensions is a string such as "Images (*.png *.xpm *.jpg);;All files (*)"
GETSET_SPECIALIZATION(File,std::string, GETSET_TAG(File,std::string,Extensions) GETSET_TAG_BOOL(File, CreateNew) GETSET_TAG_BOOL(File, Multiple), )

#undef GETSET_ENUM_CLASS_BODY
#undef GETSET_ENUM_KEY_BODY
#undef GETSET_BUTTON_CLASS_BODY
#undef GETSET_BUTTON_KEY_BODY
#undef GETSET_SPECIALIZATION

// 
// createSpecialNode
// 

namespace GetSetInternal {
	/// Create a special property by string.
	Node* createSpecialNode(Section& section, const std::string& key, const std::string& type)
	{
		Node* node=0x0;
		#define GETSET_TYPE_STR(X) if (type==#X) node=new Key##X(section,key);
		// special types
		GETSET_TYPE_STR(Slider)
		GETSET_TYPE_STR(RangedDouble)
		GETSET_TYPE_STR(Enum)
		GETSET_TYPE_STR(Button)
		GETSET_TYPE_STR(StaticText)
		GETSET_TYPE_STR(ReadOnlyText)
		GETSET_TYPE_STR(Directory)
		GETSET_TYPE_STR(File)
		#undef GETSET_TYPE_STR
		// Not actually that special at all
		if (type=="RangedInt") node=new Key<int>(section,key);
		return node;
	}
}

#endif // __GetSetSpecial_hxx
