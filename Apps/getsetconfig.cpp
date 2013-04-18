#include <iostream>

#include <GetSet/GetSet.hxx>
#include <GetSet/GetSetIO.h>
#include <GetSet/GetSetDictionary.h>

#define HAS_RUN_COMMAND

#ifdef HAS_RUN_COMMAND
// Only needed for "run" verb
#include "AutoGUI/Process.h"
Process child;
// atexit hack to prevent child from living onger than parent
void kill_child()
{
	child.kill();
}
#endif

std::string replace(const std::string& in, GetSetDictionary& config=GetSetDictionary::global())
{
	auto pos=in.find("GetSet[");
	if (pos==std::string::npos) return in;
	std::string ret=in.substr(0,pos);
	while (pos!=std::string::npos)
	{
		auto occ=in.find("]",pos);
		if (pos==std::string::npos)
			return ""; // syntax error
		std::string key=in.substr(pos+7,occ-pos-7);
		ret+=GetSet<>(key,config);
		pos=occ+1;
		occ=in.find("GetSet[",pos);
		if (occ==std::string::npos)
			ret+=in.substr(pos,in.length()-pos);
		else
			ret+=in.substr(pos,occ-pos);
		pos=occ;
	}
	return ret;
}

int main(int argc, char ** argv)
{
	if (argc<2)
	{
		std::cout <<
			"Usage:\n"
			"   config <verb> <arguments>\n"
			"where <verb> is one of \"get\", \"set\", \"del\", \"copy\", \"run\" and \"prep\"\n"
			"and <arguments> depends on choice of verb try:\n"
			"to get additional information for a specific verb.\n"
			"   config <verb>\n"
			;
		return 1;
	}
	std::string verb=argv[1];

	////////////////////////////////////////////////////////////////////////////////////////////////////
	if (verb=="get")
	{
		if (argc!=4)
		{
			std::cout <<
				"Usage:\n"
				"   config get <file> <key>\n"
				"Prints the value of the specified key, if found in file.\n"
				;
			return 1;
		}
		std::string file=argv[2];
		std::string key=argv[3];
		if (!GetSetIO::load<GetSetIO::IniFile>(file))
		{
			std::cerr << "Failed to load " << file << "!\n";
			return 2;
		}
		std::string value=GetSet<>(key);
		std::cout << value << std::endl;
		return 0;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////
	if (verb=="set")
	{
		if (argc!=5)
		{
			std::cout <<
				"Usage:\n"
				"   config set <file> <key> <value>\n"
				"Set the value of the specified key in file.\n"
				;
			return 1;
		}
		std::string file=argv[2];
		std::string key=argv[3];
		std::string value=argv[4];
		if (!GetSetIO::load<GetSetIO::IniFile>(file))
		{
			std::cerr << "Failed to load " << file << "!\n";
			return 2;
		}
		GetSet<>(key.c_str())=value;
		if (!GetSetIO::save<GetSetIO::IniFile>(file))
		{
			std::cerr << "Failed to save the file! (read-only?)\n";
			return 2;
		}
		return 0;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////
	if (verb=="prep")
	{
		if (argc!=5)
		{
			std::cout <<
				"Usage:\n"
				"   config prep <config.ini> <file.in> <file.out>\n"
				"Load all key/value pairs from config.ini and replace every occurence of\n"
				"GetSet[key] with the corresponding value. Example:\n"
				"   config set config.ini Network/IP 192.168.12.34\n"
				"   config set config.ini \"Network/Number of Pings\" 3\n"
				"   config prep ping_me.getset ping_me.bat\n"
				"Where the contents of ping_me.getset would be replaced from\n"
				"   ping GetSet[Network/IP] -n GetSet[Network/Number of Pings]\n"
				"    - to -\n"
				"   ping 192.168.12.34 -n 3\n"
				;
			return 1;
		}
		std::string ini_file=argv[2];
		std::string input_file=argv[3];
		std::string output_file=argv[4];
		if (!GetSetIO::load<GetSetIO::IniFile>(ini_file))
		{
			std::cerr << "Failed to load " << ini_file << "!\n";
			return 2;
		}
		std::string contents=replace(fileReadString(input_file));
		if (!fileWriteString(output_file,contents))
		{
			std::cerr << "Failed to save output file! (read-only?)\n";
			return 2;
		}
		return 0;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////
	if (verb=="run")
	{
#ifdef HAS_RUN_COMMAND
		if (argc!=4)
		{
			std::cout <<
				"Usage:\n"
				"   config run <config.ini> \"commands containing GetSet[key]-style properties\"\n"
				;
			return 1;
		}
		std::string ini_file=argv[2];
		std::string command=argv[3];
		if (!GetSetIO::load<GetSetIO::IniFile>(ini_file))
		{
			std::cerr << "Failed to load " << ini_file << "!\n";
			return 2;
		}
		command=replace(command);
		if (command.empty())
		{
			std::cerr << "Failed to process command!\n";
			return 2;
		}
		atexit(kill_child);
		child=Process(command);
		child.run();
		return child.waitForExit(true);
#elif
		if (argc!=3)
		{
			std::cout << "The run verb has not been built into this binary. (are you on a POSIX system?)\n";
			return 2;
		}
#endif 
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////
	if (verb=="del")
	{
		GetSetDictionary dict;
		if (argc!=4)
		{
			std::cout <<
				"Usage:\n"
				"   config del <config.ini> <file.in> <file.out>\n"
				;
			return 1;
		}
		std::string file=argv[2];
		std::string key=argv[3];
		if (!GetSetIO::load<GetSetIO::IniFile>(file,dict))
		{
			std::cerr << "Failed to load " << file << "!\n";
			return 2;
		}

		dict.remove(key);

		if (!GetSetIO::save<GetSetIO::IniFile>(file,dict))
		{
			std::cerr << "Failed to save the file! (read-only?)\n";
			return 2;
		}
		return 0;
	}

	std::cout << "Unknown verb:" << verb << std::endl;
	return 1;
}
