#include "GetSet/GetSetSpecial.hxx"
#include "GetSet/GetSetXML.h"
#include "GetSet/Factory.h"
#include "GetSet/Configurator.hxx"

#include "GetSetGui/GetSetGui.h"

#include <iostream>

/// Debugging a tree:
class DebugTree : public GetSetInternal::Access {
public:
	DebugTree(GetSetDictionary& dictionary=GetSetDictionary::global()) : Access(dictionary) {}
	void print() {
		std::string tree;
		assemblePlot(getRoot()->getSection(),tree);
		std::cout << tree;
	}
private:
	typedef std::map<std::string,GetSetInternal::GetSetNode*> PropertyByName;
	void assemblePlot(const PropertyByName& here, std::string& out, std::string indent="")
	{
		for (PropertyByName::const_iterator it=here.begin();it!=here.end();++it)
		{
			GetSetInternal::GetSetSection* s=dynamic_cast<GetSetInternal::GetSetSection*>(it->second); 
			if (s)
			{
				out+=indent+" +-"+it->first+"\n";
				assemblePlot(s->getSection(),out,indent+" | ");
			}
			else
				out+=indent+" +-"+it->first+"\n";
		}
	}
};

///
class Vehicle {
public:
};

///
class Car {
public:
};

///
class PickUp : public Car {

public:

};

void gui(const std::string& section, const std::string& key)
{
	std::cout << section << " - " << key << std::endl;
}

int main(int argc, char **argv)
{
	char *paths[]={"Bla","Bla/Blubb","Bla/Schwupp"};
	for (int i=0;i<3;i++)
	{
		GetSetPath p(paths[i]);
		p.key<int>("a");
		p.key<std::string>("b");
		p.key<bool>("c")=false;
		GetSetGui::File(p.getPath("Input Image"),p.getDictionary()).setExtensions("Image Files (*.png *.jpg);;All Files (*.*)");
	}


	GetSetGui::File("Input/Image Files").setExtensions("Image Files (*.png *.jpg);;All Files (*)").setMultiple(true)="in0.jpg;in1.jpg";
	std::vector<std::string> files=GetSet<std::vector<std::string> >("Input/Image Files"); // vector contains "in0.jpg" and "in1.jpg"

	GetSetGui::File("Output/Table File").setExtensions("Comma Seperated Values (*.csv)").setCreateNew(true)="out.csv";
	std::string file=GetSet<>("Output/Table File");

	GetSetGui::Button("Process/Start")="Text On Button";

	GetSetGui::Slider("Mask/Threshold").setMin(0.0).setMax(1.0)=0.5;
	double t=GetSet<double>("Mask/Threshold");

	GetSetGui::Enum("Alphabet/Letter").setChoices("A;B;C;D")="B";
	int indexOfB=GetSet<int>("Alphabet/Letter");

	std::vector<std::string> codecs;
	codecs.push_back("Advanced...");
	codecs.push_back("H.264 (MPEG-4 Part 10)");
	codecs.push_back("DivX Media Format (DMF)");
	GetSetGui::Enum("Video Output/Codec").setChoices(codecs)=0; // Defaults to "Advanced..."
	GetSet<>("Video Output/Codec")="H.264 (MPEG-4 Part 10)";
	int index=GetSet<int>("Video Output/Codec"); // return 1 (index of H.264)
	std::cout << "Codec used = " << GetSet<>("Video Output/Codec").getString() << std::endl; // prints "Codec used = H.264 ...


	GetSetIO::save(GetSetIO::XmlFile("out.xml"));
	
	DebugTree().print();

	GetSetIO::save(GetSetIO::TxtFileDescription("out.txt"));
	
	GetSetHandler callback(gui);

	return GetSetGui::runQtApp("Test",argc,argv);
}
