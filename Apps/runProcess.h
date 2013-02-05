#ifndef __runProcess_h

#include <string>

/// An independent process running independently alongside the parent
class Process {
public:
	Process(const std::string& binary_file);			//< Create a process from path to executable
	~Process();											//< Careful: d-tor blocks if process is running
	
	bool run(bool blocking=true);						//< Run process and possibly block while process is running

	int waitForExit();									//< Block execution until process dies

	const std::string&  getBinaryFilePath() const;		//< Path to executable
	const std::string&  getWorkingDirectory() const;	//< Working Directory 
	const std::string&  getCommanLineArgs() const;		//< Working Directory

	Process& setWorkingDirectory(const std::string&);	//< Set working directory used for the next call to run(...)
	Process& setCommanLineArgs(const std::string&);		//< Set ´command line arguments used for the next call to run(...)

	const std::string&  getConsoleOutput() const;		//< Console output of the process
	int					getExitCode() const;			//< Exit code when the process died


private:
	std::string binaryFile;
	std::string workingDir;
	std::string cmdLineArg;
	std::string consoleOut;
	int			exitCode;
	void*		handle;			//< Used for windows API implementation
};

#endif // __runProcess_h
