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

#ifndef __GetSet_hxx
#define __GetSet_hxx

#include "GetSetInternal.h"

// Basic GetSet usage:
//   GetSet<bool>("Options/Checkbox")=true;
//   GetSetIO::load<GetSetIO::IniFile>("config.ini");
//   bool myBool=GetSet<bool>("Options/Checkbox");
// See also: Special GetSet types defined in namespace GetSetGui (below)

// A macro to reduce the amount of code needed to describe attributes.
#define GETSET_TAG(CLASS,TYPE,TAG)																\
	CLASS& set##TAG(const TYPE& value)       {node.setAttribute<TYPE>(#TAG,value);return *this;}		\
	TYPE   get##TAG()                  const {return node.getAttribute<TYPE>(#TAG);}

/// Access a section other than global dictionary. Destroys everything in the way of creating relative_path.
namespace GetSetGui {
	class Section
	{
		GetSetInternal::Section& node;
	public:
		Section(GetSetInternal::Section& _section=GetSetInternal::Dictionary::global()) : node(_section) {}
		Section(const std::string& relative_path, Section& super_section=Section());

		/// Create new Key or replace an already existing one if it is a string or forceType is set.
		/// Most of the time, just return a pointer to an existing node.
		template <typename GetSetKey>
		GetSetInternal::Node& declare(const std::string& absolute_path, bool forceType) const;
		
		/// Create nodes by type
		GetSetInternal::Node& Section::createNode(const std::string& key, const std::string& type);

		/// Implicit cast to GetSetInternal::Section&
		operator GetSetInternal::Section& () { return node; }

		/// Set a brief description for this Section.
		GETSET_TAG( Section, std::string, Description )
		/// Contents of this section will not be modifiable in GUI.
		GETSET_TAG( Section, bool, Disabled )
		/// Show contents of this section in a group box.
		GETSET_TAG( Section, bool, Grouped )
		/// Collapse group box.
		GETSET_TAG( Section, bool, Collapsed )
		/// This section will not be shown in GUI at all.
		GETSET_TAG( Section, bool, Hidden )
		/// Are contents of this section shown in a collapsible group box?
		GETSET_TAG( Section, bool, Collapsible )
	};
} // 


/// Syntactic sugar to access and change GetSet properties
template <typename BasicType=std::string>
class GetSet
{
public:
	/// Access a GetSet property by by absolute path to its key (or optionally a relative path to a key within the given section)
	GetSet(const std::string& key, const GetSetGui::Section& section = GetSetGui::Section());

	/// Set the value of a GetSet property (same as: assigment operator)
	GetSet<BasicType>& setValue(const BasicType& v);
	/// Get the value of a GetSet property (same as: cast operator)
	const BasicType getValue() const;

	/// Set the value of this property from a string
	virtual GetSet<BasicType>& setString(const std::string& value);
	/// Get the value of the property as string
	virtual std::string getString() const;

	/// Set the value of a GetSet property directly via assignment operator
	inline GetSet<BasicType>& operator=(const BasicType& v);
	/// Cast operator directly to BasicType (behaves almost like a c++ variable of BasicType)
	inline operator BasicType() const;

	/// Set a brief description for this property.
	GETSET_TAG( GetSet<BasicType>, std::string, Description )

protected:
	/// Keep track of the associated property (not actually owned by this class)
	GetSetInternal::Node& node;

	/// Same as property, if the type is an exact match. Else it is null.
	GetSetInternal::Key<BasicType>* typed_node;

	/// c-tor for derived types
	GetSet(GetSetInternal::Node& _node);

};

// Special GetSet types:
//    Slider Enum Button StaticText ReadOnlyText Directory File
// All of these types specialize a basic GetSet<> class.
// For example GetSetGui::Slider is a GetSet<double> and can be used as such.
//
// Examples:
//
// File: (Open multiple files)
//    GetSetGui::File("Input/Image Files").setExtensions("Image Files (*.png *.jpg);;All Files (*)").setMultiple(true)="in0.jpg;in1.jpg";
//    std::vector<std::string> files=GetSet<std::vector<std::string> >("Input/Image Files"); // vector contains "in0.jpg" and "in1.jpg"
// File: (Single Output File)
//    GetSetGui::File("Output/Table File").setExtensions("Comma Seperated Values (*.csv)").setCreateNew(true)="out.csv";
//    std::string file=GetSet<>("Output/Table File");
// Button:
//    GetSetGui::Button("Process/Start")="Text On Button";
// Slider:
//    GetSetGui::Slider("Mask/Threshold").setMin(0.0).setMax(1.0)=0.5;
//    double t=GetSet<double>("Mask/Threshold");
// Enum: (Simple Example)
//    GetSetGui::Enum("Alphabet/Letter").setChoices("A;B;C;D")="B";
//    int indexOfB=GetSet<int>("Alphabet/Letter");
// Enum: (Define by std::vector<std::string> and use both features of GetSet<std::string> and GetSet<int>)
//    std::vector<std::string> codecs;
//    codecs.push_back("H.264 (.mp4)");
//    codecs.push_back("DivX (.dmf)");
//    GetSetGui::Enum("Video Output/Codec").setChoices(codecs)=1; // Defaults to "DivX (.dmf)"
//    or simply:
//    GetSetGui::Enum("Video Output/Codec").setChoices("H.264 (.mp4);DivX (.dmf)")=0; // Defaults to "H.264 (.mp4)"
//    GetSet<>("Video Output/Codec")="H.264 (MPEG-4 Part 10)";
//    int index=GetSet<int>("Video Output/Codec"); // return 0 or 1 (index of H.264 or DivX)
//    std::cout << "Codec used = " << GetSet<>("Video Output/Codec").getString() << std::endl;
//    // prints "Codec used = H.264 (.mp4)" or "DivX (.dmf)"
//
// Hint: to reduce the amount of code, you can use GetSetIO to load a full description of your parameters.
// Example: (GetSetIO::TxtFileDescription)
//    Key="Mask/Threshold" Type="double" Value="0.5"
//    or even
//    Key="Alphabet/Letter" Choices="A;B;C;D" Type="Enum" Value="B"
//    etc.

#include "GetSet_impl.hxx"

#endif // __GetSet_hxx
