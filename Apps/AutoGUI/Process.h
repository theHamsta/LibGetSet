#ifndef __Process_h
#define __Process_h

#include <string>

#ifdef __linux__
    #include "pstream.h"
#endif

/// An independent process running alongside the parent.
/// 
/// Example:
///   Process exe("c:/windows/system32/ping.exe");
///   if (exe.setCommanLineArgs("127.0.0.1").run())
///   {
///      std::cout << "Ping now is running! Waiting for results...\n";
///      int error_code=exe.waitForExit();
///      std::cout << "Error code: " << error_code << std::endl;
///      std::cout << exe.getConsoleOutput() << std::endl;
///   }
///   else
///      std::cerr << "Failed to run process!\n";
///
/// This impementatin does not supports piping into stdin.
///
class Process {
public:
	Process(const std::string& binary_file="");			//< Create a process from path to executable.
	~Process();											//< Process continues to live after d-tor.

	bool run();											//< Run process. Use waitForExit() if you need output or return code.
	bool isRunning() const;								//< Returns true if the process is still running.
	bool kill();										//< Kills teh process if it is still running.

	int waitForExit(bool print=false) const;			//< Block until process dies. Returns error code of process.
	void readPipe(std::ostream& out, bool print) const;	//< Blocking read on pipe until EOF and stream to out
	const std::string&  getConsoleOutput() const;		//< Contents of stdout after waitForExit()
	
	const std::string&  getBinaryFilePath() const;		//< Path to executable
	const std::string&  getWorkingDirectory() const;	//< Working directory
	const std::string&  getCommanLineArgs() const;		//< Command line arguments

	Process& setWorkingDirectory(const std::string&);	//< Set working directory used for the next call to run()
	Process& setCommanLineArgs(const std::string&);		//< Set command line arguments used for the next call to run()

protected:
	std::string binaryFile;
	std::string workingDir;
	std::string cmdLineArg;
	mutable std::string stdOutput;
	mutable int	exit_code;

#ifdef __linux__
    redi::ipstream *child;
#endif

#ifdef _WIN32
	mutable void* handle;
	mutable void* stdoutReadHandle;
#endif

};

#endif // __Process_h
