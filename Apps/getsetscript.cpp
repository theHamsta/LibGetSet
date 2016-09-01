#include <GetSet/GetSet.hxx>
#include <GetSet/GetSetScripting.h>

#include <iostream>
#include <string>


int main(int argc, char** argv)
{
	if (argc>2) {
		std::cerr << "Usage:\n   script [file.getset]\n";
		return 1;
	}
	if (argc==2)
		GetSetScriptParser::global().parse(std::string("file run ")+argv[1]);

	GetSetScriptParser::global().prompt();
	
	return 0;
}
