//
//  Library: GetSet
//  c++ library for load/saving *typed* and *named* properties and automatic GUI.
//  
//  Copyright (c) by Andre Aichert (aaichert@gmail.com)
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

#ifndef __GetSetInternal_h
#define __GetSetInternal_h

#include <map>
#include <set>
#include <vector>
#include <array>

#include "StringConversion.hxx"
#include "StringType.hxx"

////////////////////////////////////////////////////////////////////////////////////////////////////
// GetSet internal -- Please refer to GetSet<T> in GetSet.hxx to use this framework.
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace GetSetGui {
	/// Pre-declaration: User's view of a GetSetInternal::Section
	class Section;
};

namespace GetSetInternal {

	/// Pre-declaration: the root Section.
	class Dictionary;
	/// Pre-declaration: a Section.
	class Section;

	/// Variant interface class. Super class of all nodes in the property tree.
	class Node {
		friend struct StringRepresentation; //< Only i/o shall be able to access private members.
	protected:
		Node(Section& _section, const std::string& _name);
		std::map<std::string,std::string> attributes;
		/// Signals sent to dictionaries when values change.
		void signalCreate();
	public:
		/// Dictionary where this node resides. Root of the property tree.
		Dictionary& dictionary;
		// Absolute path of the section, where this key resides.
		const std::string super_section;
		// Name of this node.
		const std::string name;

		// The absolute path of this node in the dictionary. (slow)
		std::string path() const;

		// Section containing this node. Returns dictionary if no such section exists.
		Section& super() const;

		/// Set value of this node by string. (Does not apply to Sections)
		virtual void        setString(const std::string& new_value)       = 0;
		/// Get value of this node as string.
		virtual std::string getString()                             const = 0;
		/// Get the type of this node as string.
		virtual std::string getType()                               const = 0;

		/// Read-access to all attributes.
		const std::map<std::string,std::string>& getAttributes() const;

		/// Get value of an attribute. If attrib is not defined, return empty string.
		template <typename T=std::string>
		T getAttribute(const std::string attrib) const {
			auto it=attributes.find(attrib);
			return stringTo<T>(it==attributes.end()?"":it->second);
		}

		/// Set an attribute of this Node.
		template <typename T=std::string>
		void setAttribute(const std::string attrib, const T& value); 

		/// Signals sent to dictionaries when values change.
		void signalChange();

	};

	/// Templated Variant specialization. Leaves of the property tree.
	template <typename T>
	class Key : public Node {
	public:
		Key(Section& _section, const std::string& _name, const T& default_value=default_value<T>())
			: Node(_section, _name), value(default_value) {}
		virtual void        setString(const std::string& v)       { value=stringTo<T>(v); signalChange(); }
		virtual std::string getString()                     const { return toString(value); }
		virtual void        setValue (const T& v)                 { value=v; signalChange(); }
		virtual const T&    getValue ()                     const { return value; }
		virtual std::string getType  ()                     const { return typeName<T>(); }
	private:
		T value;
	};

	/// This function is defined in GetSet.hxx, because there are local types defined that have to be available internally.
	/// Create a new Node of type (unless that type is a default type or unknown). The Node is not added to the Section (yet).
	Node* createSpecialNode(Section& section, const std::string& key, const std::string& type);

	/// This is a (sub-)Section that can holds other Nodes.
	class Section : public Node
	{
		friend struct StringRepresentation; //< Only i/o shall be able to access private members.
	public:
		Section(Section& super, const std::string& _name);
		virtual ~Section();

		//
		// Sections contain several child nodes associated with a name.
		//

		/// A mapping from a string to a variant property data
		typedef std::map<std::string,Node*> NodesByName;
		/// Direct READ-ONLY access. Only needed to walk the tree (which is rarely neccessary).
		const NodesByName& getChildren() const;

		/// Walk the tree to find an existing node. Returns null if not existing.
		Node* nodeAt(const std::string& relative_path) const;

		/// Walk the tree to create path to a section.
		/// Destroys everything in path's way. Returns new or existing section at path
		Section& createSection(const std::string& relative_path);

		/// Remove a child from the tree. See also: nodeAt(...)
		void removeNode(const std::string& relative_path);

		/// Delete all children.
		void clear();

		/// Insert a new node into this section.
		void insertNode(Node& new_node);

		//
		// Node implementation
		//

		// We are a "Section" holding more properties
		virtual std::string getType() const;
		// Silently ignore calls to set (does not apply to this special case)
		virtual void setString(const std::string& new_value);
		// Obtain a short string with the types of our children (not really our value...)
		virtual std::string getString() const;

	protected:
		Section(const Section&);

		/// This is where the properties reside
		NodesByName children;

		/// Walk the tree to find an existing node. Returns null if not existing.
		Node* nodeAt(const std::vector<std::string>& path, int i=0) const;

		/// Walk the tree to create path to a section.
		/// Destroys everything in path's way. Returns new or existing section at paths
		Section& createSection(const std::vector<std::string>& path, int i=0);
	
		/// Create a new property of specified type. If type is not known, std::string is assumed.
		inline GetSetInternal::Node& createNode(const std::string& relative_path, const std::string& type)
		{
			// relative path consists of "super_section"/"key"
			auto path=stringToVector<>(relative_path,'/',true);
			std::string key=path.back();
			path.pop_back();
			Section &section=createSection(path);
			// Special GetSet types first (Button, Slider etc. are not defined in GetSetInternal but in GetSetGui.)
			Node * new_node=createSpecialNode(section,key,type);
			if (new_node) return *new_node;
			// This (ugly) code craetes a Key from a string for c-types, std::string and std::vectors of these
			if (type=="vector<string>") new_node=new Key<std::vector<std::string> >(section,key);
			else if (type=="Section") new_node=new Section(section,key);
			#define _DEFINE_TYPE(X) else if (type==#X) new_node=new Key<X>(section,key);
			#include "BaseTypes.hxx"
			#undef _DEFINE_TYPE
			#define _DEFINE_TYPE(X) else if (type=="vector<"#X">") new_node=new Key<std::vector<X> >(section,key);
			#include "BaseTypes.hxx"
			#undef _DEFINE_TYPE
			// For unknown types, std::string is assumed.
			if (!new_node) new_node=new Key<std::string>(section,key);
			section.insertNode(*new_node);
			return *new_node;
		}

	};
	
	/// The root of a propetry tree. Access only via GetSet&lt;BasicType&gt;, see also: GetSet.hxx
	class Dictionary : public GetSetInternal::Section
	{
		// Nodes and sections are allowed access, so they can insert themselves and inform observers
		friend class GetSetInternal::Node;    //< For signal Change and Destroy, as well as Attrib
		friend class GetSetInternal::Section; //< For signal Create
	public:
		/// Allow instantiation of GetSetDictionaries (not copyable)
		Dictionary();
		virtual ~Dictionary();

		/// Signals sent by nodes to their dictionaries
		enum Signal { Create, Destroy, Change, Attrib };

		/// Signal/Observer implementation Observers are notified when something changes.
		struct Observer {
			friend class Dictionary;
			Observer(const Dictionary& dict);
			void attachTo(const Dictionary*);
			virtual ~Observer();
			virtual void notify(const GetSetInternal::Node& node, Signal signal)=0;
		private:
			const Dictionary* dictionary;
		};

		/// Access to the global Dictionary, which is used whenever no Dictionary is explicitly specified.
		static Dictionary& global();

	protected:
		/// Signals sent to this class from its friend Nodes
		void signal(const GetSetInternal::Node& node, Signal signal);
		
		/// This is where the observers reside
		mutable std::set<Observer*> registered_observers;

		/// Not copy-able
		Dictionary(const Dictionary&);

	};

	/// A simple representation of all the information in a property (sub-)tree
	struct StringRepresentation {
		typedef std::map<std::string, std::string> MapStrStr;
		typedef std::map<std::string, std::map<std::string, std::string> > MapStrMapStrStr;
		std::map<std::string, std::map<std::string, std::string> > contents;

		StringRepresentation() {}
		StringRepresentation(const Section& section, const std::string& path_prefix="") { retreive(section,path_prefix); }

		/// Retreive information in this object from a Section or Dictionary. path_prefix is used for recursion and empty by default.
		void retreive(const Section& section, const std::string& path_prefix="")
		{
			const auto& keys=section.getChildren();
			for (auto it=keys.begin();it!=keys.end();++it)
			{
				std::string path=path_prefix.empty()?it->first:path_prefix+"/"+it->first;
				auto& attribs=contents[path];
				attribs=it->second->attributes;
				// Add an attribute for Type
				attribs["Type"]=it->second->getType();
				Section* section=dynamic_cast<Section*>(it->second);
				// And add an attribute for value
				if (section) retreive(*section, path);
				else attribs["Value"]=it->second->getString();
			}
		}

		/// Create Section from information in this object
		void restore(Section& section) const
		{
			for (auto it=contents.begin();it!=contents.end();++it)
			{
				// Get path and type
				const std::string& path(it->first);
				std::string type;
				auto type_it=it->second.find("Type");
				// Silently ignore keys, for which the type is missing.
				if (type_it!=it->second.end()) type=type_it->second;
				Node *node=0x0;
				// Create new node (section or key)
				if (type=="Section")
					node=&section.createSection(path);
				else
				{
					node=section.nodeAt(path);
					bool force_type=!type.empty() && type!="string";
					if (!node || force_type) node=&section.createNode(path,type);
				}
				for (auto ait=it->second.begin();ait!=it->second.end();++ait)
				{
					const std::string& attrib=ait->first;
					if (attrib=="Value") node->setString(ait->second);
					else if (attrib=="Type") continue;
					else node->attributes[attrib]=ait->second;
				}
			}
		}
	};


} // namespace GetSetInternal


#include <functional>

/// A class which calls a function to handle change signals from GetSet (eg. GUI input)
class GetSetHandler : public GetSetInternal::Dictionary::Observer
{
public:
	void ignoreNotifications(bool ignore);
	GetSetHandler(std::function<void(const GetSetInternal::Node&)> change, const GetSetInternal::Dictionary& subject = GetSetInternal::Dictionary::global());
protected:
	bool ignore_notify;
	virtual void notify(const GetSetInternal::Node& node, GetSetInternal::Dictionary::Signal signal);
	std::function<void(const GetSetInternal::Node&)> change_handler_node;
};

// Implementation of Node::setAttribute
namespace GetSetInternal {
	template <typename T>
	void Node::setAttribute(const std::string attrib, const T& value) {
		// Set new value
		attributes[attrib]=toString(value);
		// If, however, the value is empty or default, remove attrib altogether.
		if (value==stringTo<T>("") || attributes[attrib].empty())
			attributes.erase(attributes.find(attrib));
		// Let the observer know that we have changed attributes
		dictionary.signal(*this, Dictionary::Signal::Attrib);
	}
} // namespace GetSetInternal

#endif // __GetSetInternal_h
