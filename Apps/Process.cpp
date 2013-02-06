#include "Process.h"

#ifdef __APPLE__
	// 2do
#endif

#ifdef __linux__
	// 2do
#endif

#ifdef _WIN32
	#include <windows.h>
	Process::Process(const std::string& binary_file)
		: binaryFile(binary_file)
		, exit_code(-1)
		, stdoutReadHandle(0x0)
		, handle(0x0)
	{}

	Process::~Process()
	{
		if (handle)
			CloseHandle(handle);
		handle=0x0;
	}
	
	// Simple implementation w/o pipes
	//bool Process::run()
	//{
	//	if (handle) return false;
	//	STARTUPINFO si;
	//	PROCESS_INFORMATION pi;
	//	ZeroMemory(&si, sizeof(si));
	//	ZeroMemory(&pi, sizeof(pi));
	//	if(!CreateProcessA(	binaryFile.c_str(),
	//						const_cast<char*>(cmdLineArg.c_str()),
	//						NULL, NULL, FALSE, 0, NULL,
	//						workingDir.c_str(),
	//						&si,&pi)) 
	//	{
	//		handle=0x0;
	//		return false;
	//	}
	//	CloseHandle( pi.hThread );
	//	handle=pi.hProcess;
	//	return true;
	//}
	//int Process::waitForExit()
	//{
	//	if (!handle) return -1;
	//	unsigned long exit_code;
	//	WaitForSingleObject(handle, INFINITE);
	//	GetExitCodeProcess(handle, &exit_code);
	//	CloseHandle(handle);
	//	handle=0x0;
	//	return (int)exit_code;
	//}	

	bool Process::isRunning() const
	{
		if (handle)
		{
			DWORD exitcode;
			GetExitCodeProcess(handle, &exitcode);
			if (exitcode!=STILL_ACTIVE)
			{
				exit_code=exitcode;
				CloseHandle(handle);
				handle=0x0;
				// never blocks because process is already dead:
				waitForExit();
				return false;
			}
			return true;
		}
		return false;
	}

	bool Process::kill()
	{
		if (!isRunning()) return true;
		return 0!=TerminateProcess(handle,1);
	}

	bool Process::run()
	{
		// Make sure we are good to go
		if (isRunning()) return false;
		stdOutput.clear();
		HANDLE stdoutWriteHandle = NULL;

		// Creating a Pipe for stdout
		SECURITY_ATTRIBUTES saAttr; 
		ZeroMemory(&saAttr,sizeof(saAttr));
		saAttr.nLength = sizeof(SECURITY_ATTRIBUTES); 
		saAttr.bInheritHandle = TRUE; 
		saAttr.lpSecurityDescriptor = NULL; 
		BOOL ok=1;
		ok&=CreatePipe(&stdoutReadHandle, &stdoutWriteHandle, &saAttr, 5000);
		ok&=SetHandleInformation(stdoutReadHandle, HANDLE_FLAG_INHERIT, 0);
		if (!ok) return false;

		// We need to make a copy of the command line arguments.
		// This is required by the Windows API.
		// But since we are already at it, we append a space at the front
		// Now, cmdLineArg can start with a character (otherwise would have to start with whitespace)
		int l=(int)cmdLineArg.length();
		char *cmdl_arg_tmp=new char[l+2];
		for (int i=0;i<l;i++)
			cmdl_arg_tmp[1+i]=cmdLineArg[i];
		cmdl_arg_tmp[0]=' ';
		cmdl_arg_tmp[l+1]=0;

		// Starting the process
		PROCESS_INFORMATION processInfo;
		STARTUPINFOA startupInfo; 
		ZeroMemory(&startupInfo,sizeof(startupInfo));
		startupInfo.cb = sizeof(startupInfo);
		startupInfo.hStdError = stdoutWriteHandle;
		startupInfo.hStdOutput = stdoutWriteHandle;
		startupInfo.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
		startupInfo.dwFlags |= STARTF_USESTDHANDLES;
		ok&=CreateProcessA(	binaryFile.c_str(),
							cmdl_arg_tmp,
							NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL,
							workingDir.empty() ? 0x0 : workingDir.c_str(),
							&startupInfo, &processInfo);

		// Finish up
		delete [] cmdl_arg_tmp;
		if (!ok) return false;
		handle=processInfo.hProcess;
		CloseHandle(stdoutWriteHandle);
		CloseHandle(processInfo.hThread);
		return true;
	}

	int Process::waitForExit() const
	{
		if (stdoutReadHandle)
		{
			// Reading the pipe
			stdOutput.clear();
			DWORD bytes_read;
			char tBuf[257];
			while (ReadFile(stdoutReadHandle, tBuf, 256, &bytes_read, NULL) && bytes_read > 0)
			{
				tBuf[bytes_read]=0;
				stdOutput+=tBuf;
			}
			stdoutReadHandle=0x0;
		}

		if (handle)
			WaitForSingleObject(handle, INFINITE);

		if (isRunning()) return -1;
		else return exit_code;
	}

	const std::string&  Process::getConsoleOutput() const
	{
		return stdOutput;
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
