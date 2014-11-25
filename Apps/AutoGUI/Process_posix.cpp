

Process::Process(const std::string& binary_file)
	: binaryFile(binary_file)
	, exit_code(-1)
{

}

Process::~Process()
{

}

bool run()
{
	int aStdinPipe[2];
	int aStdoutPipe[2];
	int nChild;
	char nChar;
	int nResult;

	if (pipe(aStdinPipe) < 0)
	{
		std::cerr << "Failed allocating pipe for child input redirect");
		return -1;
	}
	
	if (pipe(aStdoutPipe) < 0)
	{
		close(aStdinPipe[PIPE_READ]);
		close(aStdinPipe[PIPE_WRITE]);
		std::cerr << "Failed allocating pipe for child output redirect");
		return -1;
	}

	nChild = fork();
	if (0 == nChild)
	{
		// child continues here

		// redirect stdin
		if (dup2(aStdinPipe[PIPE_READ], STDIN_FILENO) == -1)
		{
			std::cerr << "Failed redirecting stdin");
			return -1;
		}

		// redirect stdout
		if (dup2(aStdoutPipe[PIPE_WRITE], STDOUT_FILENO) == -1)
		{
			std::cerr << "Failed redirecting stdout");
			return -1;
		}

		// redirect stderr
		if (dup2(aStdoutPipe[PIPE_WRITE], STDERR_FILENO) == -1)
		{
			std::cerr << "Failed redirecting stderr");
			return -1;
		}

		// all these are for use by parent only
		close(aStdinPipe[PIPE_READ]);
		close(aStdinPipe[PIPE_WRITE]);
		close(aStdoutPipe[PIPE_READ]);
		close(aStdoutPipe[PIPE_WRITE]); 

		// run child process image
		// replace this with any exec* function find easier to use ("man exec")
		nResult = execve(szCommand, aArguments, aEnvironment);

		// if we get here at all, an error occurred, but we are in the child process, so just exit
		std::cerr << "Failed exec of the child process");
		exit(nResult);
	}
	else if (nChild > 0)
	{
		// parent continues here

		// close unused file descriptors, these are for child only
		close(aStdinPipe[PIPE_READ]);
		close(aStdoutPipe[PIPE_WRITE]); 

		// Include error check here
		if (NULL != szMessage)
			write(aStdinPipe[PIPE_WRITE], szMessage, strlen(szMessage));

		// Just a char by char read here, you can change it accordingly
		while (read(aStdoutPipe[PIPE_READ], &nChar, 1) == 1)
			write(STDOUT_FILENO, &nChar, 1);
		
		// done with these in this example program, you would normally keep these
		// open of course as long as you want to talk to the child
		close(aStdinPipe[PIPE_WRITE]);
		close(aStdoutPipe[PIPE_READ]);
	}
	else
	{
		// failed to create child
		close(aStdinPipe[PIPE_READ]);
		close(aStdinPipe[PIPE_WRITE]);
		close(aStdoutPipe[PIPE_READ]);
		close(aStdoutPipe[PIPE_WRITE]);
	}
		return nChild;
	}
}

bool isRunning() const
{

}

bool kill()
{

}

int waitForExit(bool print=false) const
{
	std::ostringstream strstr;
	readPipe(strstr, print);
	stdOutput=strstr.str();
	int returnStatus;
	waitpid(childPid, &returnStatus, 0);
	return returnStatus;
}

void readPipe(std::ostream& out, bool print) const
{
}

