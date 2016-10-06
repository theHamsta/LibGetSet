//
//  Package: AutoGUI
//  Copyright (c) by André Aichert (aaichert@gmail.com)
//
// This a simple example program showing how a console application can be
// configured by a host process through an auto-generated GUI. This method
// is useful, whenever you need to focus on algorithm development and when
// you cannot or do not want to link to large GUI-libraries, such as Qt.
// 
// In addition, the host can be rewritten using different libraries without 
// re-building the client. The client can be run on systems where no GUI is
// available if you configure it directly through ini-files.
// 
// This package is part of (and depends on) the GetSet libraries.
// See also: http://sourceforge.net/projects/getset/
//
// Do whatever the * you want with this code.
//

#include <GetSetObjects/ObjectFactory.h>

/// Name
class Name : public GetSetObjects::Object {
	GETSET_DECLARE_CLASS(Name,Object)
public:
	std::string first;
	std::string last;

	void stateName()
	{
		std::cout << "My name is" << first << " " << last << std::endl;
	}

};

Name::Name(GetSetObjects::Configurator& config)
{
	first=config.declare<std::string>("First Name","Hans");
	last =config.declare<std::string>("Last Name","Mustermann");
	config.declareButton("State Name",stateName);
}

GETSET_REGISTER_CLASS(Name);

/// Command Line Application
int main(int argc, char **argv)
{
	GetSetObjects::

	return 0;
}
	
