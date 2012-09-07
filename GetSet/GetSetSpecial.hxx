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


/// Specializations for GUI representations
#define GETSET_SPECIALIZATION(SPECIAL_TYPE,BASE_TYPE,CLASS_BODY,KEY_BODY)									\
	namespace GetSetInternal																				\
	{																										\
		class GetSetKey##SPECIAL_TYPE : public GetSetKey<BASE_TYPE>											\
		{																									\
		public:																								\
			virtual std::string getType() const { return #SPECIAL_TYPE; }									\
			KEY_BODY																						\
		};																									\
	}																										\
	namespace GetSetGui																						\
	{																										\
		class SPECIAL_TYPE : public GetSet<BASE_TYPE>														\
		{																									\
		public:																								\
			SPECIAL_TYPE(const std::string& pathToKey, GetSetDictionary& d=GetSetDictionary::global())		\
				: GetSet<BASE_TYPE>(d)																		\
			{																								\
				section=pathToKey;																			\
				key=splitRight(section,"/\\");																\
				property=&declare<GetSetInternal::GetSetKey##SPECIAL_TYPE>(pathToKey,true);					\
				typedProperty=dynamic_cast<GetSetInternal::GetSetKey##SPECIAL_TYPE*>(property);				\
			}																								\
			SPECIAL_TYPE(const std::string& pathToSection, const std::string& k,							\
					GetSetDictionary& d=GetSetDictionary::global())											\
						: GetSet<BASE_TYPE>(d)																\
			{																								\
				section=pathToSection; key=k;std::string path=section.empty()?key:section+"/"+key;			\
				property=&declare<GetSetInternal::GetSetKey##SPECIAL_TYPE>(path,true);						\
				typedProperty=dynamic_cast<GetSetInternal::GetSetKey##SPECIAL_TYPE*>(property);				\
			}																								\
			void operator=(const BASE_TYPE& v) { setValue(v); }												\
			operator BASE_TYPE() const { return getValue(); }												\
			CLASS_BODY																						\
		};																									\
	}
// end of GETSET_SPECIALIZATION

#define GETSET_TAG(SPECIAL_TYPE,TYPE,TAG)																	\
	SPECIAL_TYPE& set##TAG(const TYPE& value) {property->attributes[#TAG]=toString(value);return *this;}	\
	TYPE get##TAG() const {return stringTo<TYPE>(property->attributes[#TAG]);}

// The Enum class is more complex, because it has features of both GetSet<std::string> and GetSet<int>
#define GETSET_ENUM_CLASS_BODY																				\
	GETSET_TAG(Enum,std::vector<std::string>,Choices)														\
	Enum& setChoices(const std::string& c) {property->attributes["Choices"]=c;return *this;}				\
	inline void operator=(const std::string& v) { setString(v); }											\
	inline operator std::string() const { return getString(); }

#define GETSET_ENUM_KEY_BODY																				\
	virtual std::string getString() const																	\
	{																										\
		std::map<std::string,std::string>::const_iterator it=attributes.find("Choices");					\
		if (it==attributes.end()) return toString(value);													\
		std::vector<std::string> c=stringTo<std::vector<std::string> >(it->second);							\
		if (value<0||value>=(int)c.size()) return toString(value); else return c[value];					\
	}																										\
	virtual void setString(const std::string& in)															\
	{																										\
		std::vector<std::string> c=stringTo<std::vector<std::string> >(attributes["Choices"]);				\
		for (int i=0;i<(int)c.size();i++)																	\
			if (c[i]==in) { value=i; return; }																\
		value=stringTo<int>(in);																			\
	}

/// A pulldown menu with a number of choices.
GETSET_SPECIALIZATION(Enum,int,GETSET_ENUM_CLASS_BODY, GETSET_ENUM_KEY_BODY)

/// A GetSet&lt;double&gt; with additional range information, so that it could be represented as a slider
GETSET_SPECIALIZATION(Slider,double, GETSET_TAG(Slider,double,Min) GETSET_TAG(Slider,double,Max), )

/// A button that creates a GetSet change event when pressed.
GETSET_SPECIALIZATION(Button,std::string, void action() {signalChange(section,key);}, )

/// A static text with some information. StaticTexts are not included in ini-Files (user-info in GUI)
GETSET_SPECIALIZATION(StaticText,std::string, , )

/// An edit field, but read-only. Intended for output-values that the user can select and copy to clipboard.
GETSET_SPECIALIZATION(ReadOnlyText,std::string, , )

/// A directory
GETSET_SPECIALIZATION(Directory,std::string, , )

/// A file (or multiple semicolon seperated files). Extensions is a string such as "Images (*.png *.xpm *.jpg);;All files (*)"
GETSET_SPECIALIZATION(File,std::string, GETSET_TAG(File,std::string,Extensions) GETSET_TAG(File,bool, CreateNew) GETSET_TAG(File,bool, Multiple), )

namespace GetSetInternal {
	/// Create a special property by string
	inline GetSetNode* createSpecial(const std::string& type)
	{
		GetSetNode* node=0x0;
		#define GETSET_TYPE_STR(X) if (type==#X) node=new GetSetKey##X();
		// special types
		GETSET_TYPE_STR(Slider)
		GETSET_TYPE_STR(Enum)
		GETSET_TYPE_STR(Button)
		GETSET_TYPE_STR(StaticText)
		GETSET_TYPE_STR(ReadOnlyText)
		GETSET_TYPE_STR(Directory)
		GETSET_TYPE_STR(File)
		#undef GETSET_TYPE_STR
		return node;
	}
}

#undef GETSET_ENUM_CLASS_BODY
#undef GETSET_ENUM_KEY_BODY
#undef GETSET_SPECIALIZATION
#undef GETSET_TAG

#endif // __GetSetSpecial_hxx
