
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

	std::string command=binaryFile+" "+cmdLineArg;
	// Starting the process
	PROCESS_INFORMATION processInfo;
	STARTUPINFOA startupInfo; 
	ZeroMemory(&startupInfo,sizeof(startupInfo));
	startupInfo.cb = sizeof(startupInfo);
	startupInfo.hStdError = stdoutWriteHandle;
	startupInfo.hStdOutput = stdoutWriteHandle;
	startupInfo.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
	startupInfo.dwFlags |= STARTF_USESTDHANDLES;
	ok&=CreateProcessA(	NULL, // safe for CreateProcessA:
						const_cast<char*>(command.c_str()),
						NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL,
						workingDir.empty() ? 0x0 : workingDir.c_str(),
						&startupInfo, &processInfo);
	if (!ok) return false;
	handle=processInfo.hProcess;
	CloseHandle(stdoutWriteHandle);
	CloseHandle(processInfo.hThread);
	return true;
}

void Process::readPipe(std::ostream& out, bool print) const
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
			out << tBuf;
			if (print)
				std::cout << tBuf;
		}
		stdoutReadHandle=0x0;
	}		
}

int Process::waitForExit(bool print) const
{
	std::ostringstream strstr;
	readPipe(strstr, print);
	stdOutput=strstr.str();
	if (handle)
		WaitForSingleObject(handle, INFINITE);
	DWORD exitcode;
	GetExitCodeProcess(handle, &exitcode);		
	return exit_code=exitcode;
}

const std::string&  Process::getConsoleOutput() const
{
	return stdOutput;
}
