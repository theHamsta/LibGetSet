#include "Process.h"

#include <sstream>
#include <iostream>

// The implementation of Process is platform dependent.

#ifdef __APPLE__
	// 2do
#endif

#ifdef __linux__

Process::Process(const std::string& binary_file)
    : binaryFile(binary_file)
    , exit_code(-1)
    , child(0x0)

{
}

Process::~Process()
{
    if (child)
        delete child;
    child=0x0;
}

bool Process::run()
{
    kill();
    std::string command=std::string("cd \"")+workingDir+"\"; " + binaryFile+" "+cmdLineArg;
    child=new redi::ipstream(command);
    if (!child->is_open()) return false;
    return true;
}

bool Process::isRunning() const
{
    return child && child->is_open();
}

bool Process::kill()
{
    if (child)
    {
        child->rdbuf()->kill();
        delete child;
        child=0x0;
    }
    return true;
}

int Process::waitForExit(bool print) const
{
    if (!child) return -1;
    std::ostringstream strstr; // translated from windows version. dumb implementation
    readPipe(strstr, print);
    stdOutput=strstr.str();
    child->close();
    return child->rdbuf()->status();
}

void Process::readPipe(std::ostream& out, bool print) const
{
    std::string str;
    while (child && (*child >> str))
    {
        if (print)
            std::cout << str << std::endl;
        out << str << std::endl;
    }
}

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
