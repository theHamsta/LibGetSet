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

#include "StringUtil.hxx"

class GetSetDictionary;

#include "TypeString.hxx"

//
// GetSet internal -- Please refer to GetSet<T> in GetSet.hxx to use this framework.
//

namespace GetSetInternal {

	class Section;

	/// Variant interface class. Super class of all nodes in the property tree.
	class Node {
	public:
		virtual std::string getType() const = 0;
		virtual void setString(const std::string& new_value) = 0;
		virtual std::string getString() const = 0;
		virtual ~Node() {}
		std::map<std::string,std::string> attributes;
	};

	/// Templated Variant specialization. Leaves of the property tree.
	template <typename T>
	class GetSetKey : public Node
	{
	public:
		T value;

		GetSetKey() {reset(value);}

		virtual std::string getType() const {
			return typeName<T>();
		}

		virtual void setString(const std::string& new_value) {
			value=stringTo<T>(new_value);
		}

		virtual std::string getString() const {
			return toString(value);
		}
	};

	/// This function is defined in GetSet.hxx, because there are local types defined that have to be available internally.
	Node* createSpecial(const std::string& type);

	/// Way to expose select privates in GetSetDictionary. Definitions can only occur in GetSetDictionary.hxx!
	/// The idea is, that instead of having a member of GetSetDictionary, a class derives from GetSetInternal::Access.
	class Access
	{
	public:
		/// Create a new property in dictionary at path with specified type provided as string.
        static Node& createProperty(GetSetDictionary& dictionary, const std::string& path, const std::string& type)
        {
            // Special GetSet types first
            GetSetInternal::Node * node=createSpecial(type);
            if (!node)
            {
                // This (ugly) code craetes a GetSetKey from a string for c-types, std::string and std::vectors of these
                if (type=="string") node=new GetSetKey<std::string>();
                else if (type=="vector<string>") node=new GetSetKey<std::vector<std::string> >();
                #define _DEFINE_TYPE(X) else if (type==#X) node=new GetSetKey<X>();
                #include "BaseTypes.hxx"
                #undef _DEFINE_TYPE
                #define _DEFINE_TYPE(X) else if (type=="vector<"#X">") node=new GetSetKey<std::vector<X> >();
                #include "BaseTypes.hxx"
                #undef _DEFINE_TYPE
            }
            // For unkown types we just use std::string because it can hold /any/ value
            if (!node) node=new GetSetKey<std::string>();
            Access(dictionary).setProperty(path,node);
            return *node;
        }

		/// Allow access to getProperty
		Node* getProperty(const std::string& path);

	protected:

		GetSetDictionary& dictionary;
		Access(GetSetDictionary& d);

		/// Allow access to setProperty
		void setProperty(const std::string& path, Node* p);

		/// GetSetKeyType must inherit from Node
		template <typename GetSetKeyType>
        Node& declare(const std::string& path, bool forceType) const;

		/// Notify all observers of a change event
		void signalChange(const std::string& section, const std::string& key);
		/// Notify all observers of a creation event
		void signalCreate(const std::string& section, const std::string& key);
		/// Notify all observers of a destruction event
		void signalDestroy(const std::string& section, const std::string& key);
		/// Notify all observers of a change in attributes
		void signalUpdateAttrib(const std::string& section, const std::string& key);
    };


	/// An interface for file access. See Also: namespace GetSetIO
	class InputOutput
	{
	public:
		/// Flexible c-tor for string streams, stdio, file streams etc.
        InputOutput(std::istream&, std::ostream&);

    // protected: // FIXME gcc does not like this gui despite old friendship

        // In a way, this class extends the functionality of GetSetDictionary.
		// Currently I use friendship to express this. But this is not a good design.
		// Still, nothing inside this class should be accessible to the user.
		friend class Section;
		friend class GetSetDictionary;
		
		/// Store value of a key
		virtual void store(const std::string& section, const std::string& key, GetSetInternal::Node* value);
		/// Retreive all values that are available and store them in dictionary
		virtual void retreiveAll(GetSetDictionary& dictionary);

        virtual void write() const = 0;
        virtual void read() = 0;

		/// reference to the stream used for input. Usually istr==*pFile or some std::istringstream
		std::istream&	istr;
		/// reference to the stream used for output. Usually ostr==*pFile or ostr==std::cout
		std::ostream&	ostr;

		typedef std::map<std::string, std::string> MapStrStr;
		typedef std::map<std::string, std::map<std::string, std::string> > MapStrMapStrStr;
		std::map<std::string, std::map<std::string, std::string> > contents;

	};

	/// This is a (sub-)Section that can hold other Keys. It is an inner node of the property tree.
	class Section : public Node, public Access
	{
		/// GetSetInternal::Access and GetSetSection classes should have access, so they can declare(...) Sections
		friend class GetSetInternal::Access;

	public:
		/// A mapping from a string to a variant property data
		typedef std::map<std::string,Node*> PropertyByName;
		/// Direct READ-ONLY access. Only needed to walk the tree (which is rarely neccessary).
		const PropertyByName& getSection() const;

	protected:
		/// This is where the properties reside
		PropertyByName properties;
		/// Path to this Section 
		std::string absolutePath;

		/// Not copyable
		Section(const Section&);

		/// Not publicly constructible
		Section(const std::string& path, GetSetDictionary& dict);

		/// Store values in a InputOutput object
		void store(InputOutput& file) const;		

		/// Destroy all properties held by this object
		virtual ~Section();

		// Node implementation

		// We are a "Section" holding more properties
		virtual std::string getType() const {return "Section";}
		// Silently ignore calls to set (does not apply to this special case)
		virtual void setString(const std::string& new_value) {}
		// Obtain a short string with the types of our children (not really our value...)
		virtual std::string getString() const;

		/// Replace or create a property at path in the tree
		void setProperty(const std::vector<std::string>& path, Node* prop, int i);

		/// Check if there is a property at path. If so, Return it eles return null.
		Node* getProperty(const std::vector<std::string>& path, int i) const;
	};

}// namespace GetSetInternal


#endif // __GetSetInternal_h

#include "GetSetDictionary.h"

#ifdef __GetSetDictionary_h
#ifndef __GetSet_Access_Declare
#define __GetSet_Access_Declare

namespace GetSetInternal
{
    template <typename GetSetKeyType>
    Node& Access::declare(const std::string& path, bool forceType) const
    {
        std::vector<std::string> pv=stringToVector<std::string>(path,'/');
        Node* d=dictionary.getProperty(pv,0);
        // Check if GetSetKeyType at path exists or if another Node exsist and we don't forceType
        if (d && (!forceType || dynamic_cast<GetSetKeyType*>(d)))
            return *d;
        std::string value="";
        // d exists but is of wrong type and we want to forceType
        if (d) value=d->getString();
        // Create a new GetSetKeyType at path
        GetSetKeyType* p=new GetSetKeyType();
        p->setString(value);
        dictionary.setProperty(pv,p,0);
        return *p;
    }
}

#endif // __GetSet_def_declare
#endif // __GetSetDictionary_h
