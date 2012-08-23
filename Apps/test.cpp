#include "GetSet/GetSet.hxx"
#include "GetSet/GetSetCmdLine.h"
#include "GetSet/GetSetFile.h"
#include "GetSet/GetSetXML.h"

#include "GetSetGui\GetSetGui.h"

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

	

//	GetSetProperies.load(GetSetIO::XmlFile("out.xml"));
	
	DebugTree().print();

	GetSetDictionary::global().save(GetSetIO::TxtFileDescription("out.txt"));
	
	GetSetHandler callback(gui);

	return GetSetGui::runQtApp("Test",argc,argv);
}
