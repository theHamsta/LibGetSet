#include "runProcess.h"

#ifdef __APPLE__
	// 2do
#endif

#ifdef __linux__
	// 2do
#endif

#ifdef _WIN32
	#include <windows.h>
	#include <vector>

#include <iostream> // dbg

	Process::Process(const std::string& binary_file)
		: binaryFile(binary_file)
		, handle(0x0)
	{}

	Process::~Process()
	{
		waitForExit();
	}
	
	bool Process::run(bool blocking)
	{
		waitForExit();
		STARTUPINFO si;
		PROCESS_INFORMATION pi;
		ZeroMemory( &si, sizeof(si) );
		ZeroMemory( &pi, sizeof(pi) );
		if(!CreateProcessA(	binaryFile.c_str(),
							const_cast<char*>(cmdLineArg.c_str()),
							NULL, NULL, FALSE, 0, NULL,
							workingDir.c_str(),
							&si,&pi)) 
		{
			std::cout << "Process::run - error " << GetLastError() << std::endl;
			handle=0x0;
			return false;
		}
		CloseHandle( pi.hThread );
		handle=pi.hProcess;
		if (blocking)
			return waitForExit()==0;
		return true;
	}
	
	int Process::waitForExit()
	{
		if (!handle) return -1;
		unsigned long exit_code;
		WaitForSingleObject(handle, INFINITE);
		GetExitCodeProcess(handle, &exit_code);
		return (int)exit_code;
	}

#endif

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

const std::string& Process::getConsoleOutput() const
{
	return consoleOut;
}

int Process::getExitCode() const
{
	return exitCode;
}



//	void ReadFromPipe(void) 
//
//// Read output from the child process's pipe for STDOUT
//// and write to the parent process's pipe for STDOUT. 
//// Stop when there is no more data. 
//{ 
//   DWORD dwRead, dwWritten; 
//   CHAR chBuf[BUFSIZE]; 
//   BOOL bSuccess = FALSE;
//   HANDLE hParentStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
//
//   for (;;) 
//   { 
//      bSuccess = ReadFile( g_hChildStd_OUT_Rd, chBuf, BUFSIZE, &dwRead, NULL);
//      if( ! bSuccess || dwRead == 0 ) break; 
//
//      bSuccess = WriteFile(hParentStdOut, chBuf, 
//                           dwRead, &dwWritten, NULL);
//      if (! bSuccess ) break; 
//   } 
//} 

