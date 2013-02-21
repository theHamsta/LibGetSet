#include <GetSet/GetSet.hxx>
#include <GetSet/GetSetIO.h>

#include <iostream>

int main(int argc, char** argv)
{
	// Define types and standard values

	GetSet<int>("Algorithm","Number of Iterations")=25;
	GetSet<double>("Algorithm","Step Size")=0.123;
	GetSet<bool>("Algorithm","Verbose Output")=false;

	GetSet<std::string>("Input","Source Image")="./file0.png";
	GetSet<std::string>("Input","Target Image")="./file1.png";
	GetSet<std::string>("Input","Mask Image")="./mask.png";

	// (try to) read configuration file
	GetSetIO::load<GetSetIO::IniFile>("config.ini");

	// Access values though implicit casts:
	int n=GetSet<int>("Algorithm","Number of Iterations");

	// You can do GetSet<...>("X/Y/Z") instead of GetSet<...>("X/Y", "Z")
	double step=GetSet<double>("Algorithm/Step Size");

	// And implicit cast to bool also happens here:
	if (GetSet<bool>("Algorithm","Verbose Output"))
		std::cout << "Verbose Output enabled!\n";

	// Default template parameter is std::string
	std::string file0=GetSet<>("Input","Source Image");
	std::string file1=GetSet<>("Input","Target Image");
	std::string file2=GetSet<>("Input","Mask Image");

	// Your simple algorithm would go here

	return 0;
}

