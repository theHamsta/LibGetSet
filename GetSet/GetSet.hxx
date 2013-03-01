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

#include "GetSetDictionary.h"

// Basic GetSet usage:
//   GetSet<bool>("Options/Checkbox")=true;
//   GetSetIO::load<GetSetIO::IniFile>("config.ini");
//   bool myBool=GetSet<bool>("Options/Checkbox");
// See also: Special GetSet types defined in namespace GetSetGui (below)

/// Syntactic sugar to access and change GetSet properties
template <typename BasicType=std::string>
class GetSet : public GetSetInternal::Access
{
public:
	/// Access a GetSet property by section and key (and optionally explicitly a dictionary)
	GetSet(const std::string& pathToSection, const std::string& k, GetSetDictionary& dict=GetSetDictionary::global());
	/// Access a GetSet property by the absolute path to its key (and optionally explicitly a dictionary)
	GetSet(const std::string& pathToKey, GetSetDictionary& dict=GetSetDictionary::global());


	/// Set the value of a GetSet property (same as: assigment operator)
	GetSet<BasicType>& setValue(const BasicType& v);
	/// Get the value of a GetSet property (same as: cast operator)
	const BasicType getValue() const;

	/// Set the value of this property from a string
	virtual void setString(const std::string& value);
	/// Get the value of the property as string
	virtual std::string getString() const;

	/// Set the value of a GetSet property directly via assignment operator
	inline void operator=(const BasicType& v) { setValue(v); }
	/// Cast operator directly to BasicType (behaves almost like a c++ variable of BasicType)
	inline operator BasicType() const { return getValue(); }

	/// Set a brief description for this property. Same as setAttribute("Description",...).
	GetSet<BasicType>& setDescription(const std::string& desc);
	/// Get a brief description for this property. Same as getAttribute("Description",...).
	std::string getDescription() const;

	/// Access attributes directly (eg. "Description", "CommandLineArg" etc.)
	GetSet<BasicType>& setAttribute(const std::string& attrib, const std::string& value);
	/// Access attributes directly
	std::string getAttribute(const std::string& attrib) const;

protected:
	/// The path to the section where this property resides
	std::string section;
	/// The name to the associated property in section
	std::string key;

	/// Keep track of the associated property (not actually owned by this class)
	GetSetInternal::GetSetNode* property;

	/// Same as property, if the type is an exact match. Else it is null.
	GetSetInternal::GetSetKey<BasicType>* typedProperty;

	/// c-tor for subclasses: initialize property from there.
	GetSet(GetSetDictionary& dict) : GetSetInternal::Access(dict) {}

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
//    codecs.push_back("Advanced...");
//    codecs.push_back("H.264 (MPEG-4 Part 10)");
//    codecs.push_back("DivX Media Format (DMF)");
//    GetSetGui::Enum("Video Output/Codec").setChoices(codecs)=0; // Defaults to "Advanced..."
//    GetSet<>("Video Output/Codec")="H.264 (MPEG-4 Part 10)";
//    int index=GetSet<int>("Video Output/Codec"); // return 1 (index of H.264)
//    std::cout << "Codec used = " << GetSet<>("Video Output/Codec").getString() << std::endl; // prints "Codec used = H.264 ...
//
// Hint: to reduce the amount of code, you can use GetSetIO to load a full description of your parameters.
// Example: (GetSetIO::TxtFileDescription)
//    Key="Alphabet/Letter" Choices="A;B;C;D" Type="Enum" Value="B"
// Example: (GetSetIO::XMLDescription)
//    <Section Name="Alphabet">
//      <Key Name="Letter" Choices="A;B;C;D" Type="Enum">B</Key>
//    </Section>

#include "GetSet_impl.hxx"

#endif // __GetSet_hxx
