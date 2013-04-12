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

#include <iostream>
#include <fstream>

#include "GetSetAutoGUI.hxx"
GETSET_GLOBAL_DICTIONARY

int main(int argc, char **argv)
{
	// ********************************************
	// This is where we define our parameters, standard values and descriptions if you feel like it.
	//

	GetSetGui::Enum("Executable/Exit Code")
		.setChoices("Success;Error 1;Error 2")
		.setDescription("The exit code returned by the program.")
		=0;
	
	GetSet<bool>("Executable/Print Command Line Arguments")
		.setDescription("Just displays what is supplied via argc/argv")
		=true;

	GetSetGui::File("Algorithm/Output File")
		.setExtensions("Text File (*.txt);;All Files (*)")
		.setCreateNew(true)
		.setDescription("Prints a stupid text to this file.")
		="hello.txt";

	GetSet<bool>("Algorithm/Count to 10^10")
		.setDescription("Take some time performing a stupid task and show progress.")
		=true;

	// ********************************************
	// This piece of code should look very similar in all "--xml"-style configurable command line apps
	//

	// User has to supply exactly one command line argument: either "--xml" or path to an ini-File
	if (!GetSetAutoGUI::handleCommandLine(argc,argv))
	{
		// This program does not offer a command line interface beyond --xml or ini-file.
		// You could however define a more complex command line interfac here instead of just printing an error message
		std::cerr	<< "Usage:\n   ExampleAutoGUI --xml\n   ExampleAutoGUI file.ini\n\n"
					<< "This is just an example program showing how a console application can be configured by a host process through an auto-generated GUI.\n"
					<< "It doesnt't do anything useful, really.\n"
					<< "See also: http://sourceforge.net/projects/getset/ \n\n";
		return 0;
	}

	// ********************************************
	// From here on it's just performing some example tasks
	// Your algorithm would go here
	//

	if (GetSet<bool>("Executable/Print Command Line Arguments"))
	{
		std::cout << "Printing command line args:\n";
		for (int i=0;i<argc;i++)
			std::cout << i << "\t- " << argv[i] << std::endl;
	}

	if (GetSet<bool>("Algorithm/Count to 10^10"))
	{
		// You can use "### <command> - <action>: <parameters> - <key>" syntax for some extra magic.
		// This example displays a progress bar
		GetSetAutoGUI::info("Counting...","Spend some time counting up to a few billion...");
		int n=1000;
		double c=-2;
		for (int a=0;a<n;a++)
		{
			for (int b=0;b<10000000;b++) c=-c-c*c;
			GetSetAutoGUI::progress("Counting...",a,n);
			std::cout << "c = " << c << std::endl;
		}
		GetSetAutoGUI::progress_hide("Counting...");
	}

	std::string filename=GetSet<>("Algorithm/Output File");
	std::cout << "Trying to print to file " << filename << std::endl;
	std::ofstream file(filename);
	if (!file.good())
		std::cout << "Failed.\n";
	else
		file << "Just a test.\n";

	int error=GetSet<int>("Executable/Exit Code");
	if (error!=0)
		GetSetAutoGUI::error("Simulated Error", "We pretend an error had occured: Error " + toString(error));
	return 0;
}
