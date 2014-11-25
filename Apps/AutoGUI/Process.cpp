#include "Process.h"

#include <sstream>
#include <iostream>

// The implementation of Process is platform dependent.

#ifdef __APPLE__
	// 2do
#endif

#ifdef __linux__
	#include "Process_posix.cpp"
#endif

#ifdef _WIN32
	#include "Process_win32.cpp"
#endif

const std::string&  Process::getConsoleOutput() const
{
	return stdOutput;
}

const std::string&  Process::getBinaryFilePath() const
{
	return binaryFile;
}

const std::string&  Process::getWorkingDirectory() const
{
	return workingDir;
}

const std::string&  Process::getCommanLineArgs() const
{
	return cmdLineArg;
}

Process& Process::setWorkingDirectory(const std::string& pwd)
{
	workingDir=pwd;
	return *this;
}

Process& Process::setCommanLineArgs(const std::string& arg)
{
	cmdLineArg=arg;
	return *this;
}
