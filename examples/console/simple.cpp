#include <GetSet/GetSet.hxx>
#include <GetSet/GetSetIO.h>

#include <iostream>

int main(int argc, char** argv)
{
	//// Define types and standard values

	GetSet<int>("Algorithm/Number of Iterations")=25;

	std::cout << GetSet<int>("Algorithm/Number of Iterations").getValue() << std::endl;


	GetSet<double>("Algorithm/Step Size")=0.123;
	GetSet<bool>("Algorithm/Verbose Output")=false;

	GetSetGui::File("Input/Source Image").setExtensions("Portable Network Graphics (*.png);;All Files (*)")="./file0.png";
	GetSetGui::File("Input/Target Image").setExtensions("Portable Network Graphics (*.png);;All Files (*)")="./file1.png";
	GetSetGui::Directory("Output/Output Dir")="./out/";
	GetSetGui::File("Output/Output Image").setExtensions("Portable Network Graphics (*.png);;All Files (*)").setCreateNew(true)="./file3.png";

	// (try to) read configuration file
	GetSetIO::load<GetSetIO::IniFile>("config.ini");

	// Access values though implicit casts:
	int n=GetSet<int>("Algorithm/Number of Iterations");

	// You can do GetSet<...>("X/Y/Z") instead of GetSet<...>("X/Y", "Z")
	double step=GetSet<double>("Algorithm/Step Size");

	// And implicit cast to bool also happens here:
	if (GetSet<bool>("Algorithm/Verbose Output"))
		std::cout << "Verbose Output enabled!\n";

	// Default template parameter is std::string
	std::string file0=GetSet<>("Input/Source Image");
	std::string file1=GetSet<>("Input/Target Image");

	//// Your simple algorithm would go here

	GetSetIO::TxtDetailed io;
	io.retreive();
	io.saveStream(std::cout);
	return 0;
}

