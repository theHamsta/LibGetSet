#include <iostream>

#include <GetSet/GetSet.hxx>
#include <GetSet/GetSetIO.h>
#include <GetSet/GetSetDictionary.h>

// #define HAS_RUN_VERB

#ifdef HAS_RUN_VERB
// Only needed for "run" verb
#include "AutoGUI/Process.h"
Process child;
// atexit hack to prevent child from living longer than parent
void kill_child()
{
	child.kill();
}
#endif

// Helper function for the "copy" verb
void copy(const std::string& prefix, const GetSetInternal::Section::PropertyByName& sin, const GetSetSection& dout)
{
	for (auto it=sin.begin();it!=sin.end();++it)
	{
		using GetSetInternal::Section;
		Section *s=dynamic_cast<Section*>(it->second);
		std::string p=prefix.empty()?it->first:prefix+"/"+it->first;
		if (s)
		{
			// recursively copy subsections	
			auto sin2=s->getSection();
			copy(p,sin2,dout);
		}
		else
		{
			// copy a single node
			GetSet<>(p,dout)=it->second->getString();
		}
	}
}


std::string replace(const std::string& in, const GetSetSection& config=GetSetDictionary::global())
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
			"where <verb> is one of \"get\", \"set\", \"del\", \"copy\", \"echo\""
#ifdef HAS_RUN_VERB
			", \"run\""
#endif
			" and \"prep\"\n"
			"and <arguments> depends on choice of verb.\n"
			"to get additional information for a specific verb try:\n"
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
			// Fix 2013-07-09: It's okay if the file does not exist yet.
			//std::cerr << "Failed to load " << file << "!\n";
			//return 2;
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
	if (verb=="copy")
	{
		if (argc!=5 && argc!=6)
		{
			std::cout <<
				"Usage:\n"
				"   config copy <file> <path> <file> [destination]\n"
				"Copies a key or a section from one file to another\n"
				;
			return 1;
		}
		std::string file_in=argv[2];
		std::string path=argv[3];
		std::string file_out=argv[4];
		GetSetDictionary din,dout;
		if (!GetSetIO::load<GetSetIO::IniFile>(file_in,din))
		{
			std::cerr << "Failed to load " << file_in << "!\n";
			return 2;
		}
		if (!GetSetIO::load<GetSetIO::IniFile>(file_out,dout))
		{
			std::cerr << "Failed to load " << file_out << "!\n";
			return 2;
		}

		std::string prefix=path;
		if (argc==6)
			prefix=argv[5];

		auto *sin=&(din.getSection());
		auto *sout=&(dout.getSection());
		
		std::vector<std::string> p=stringToVector<std::string>(path,'/');
		for (int i=0;i<(int)p.size()&&sin;i++)
		{
			auto it=sin->find(p[i]);
			if (it!=sin->end())
			{
				using GetSetInternal::Section;
				Section *s=dynamic_cast<Section*>(it->second);
				if (!s) sin=0x0;
				else sin=&(s->getSection());
			}
			else sin=0x0;
		}

		if (!sin)
			// If the path was invalid or points to a leaf, try to set a single property directly
			GetSet<>(prefix,dout)=GetSet<>(path,din).getString();
		else
			// Copy the entire subsection
			copy(prefix,*sin,dout);

		if (!GetSetIO::save<GetSetIO::IniFile>(file_out,dout))
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
				"GetSet[key] with the corresponding value.\n"
				"Example:\n"
				"   ping GetSet[Network/IP] -n GetSet[Network/Number of Pings]\n"
				"    - becomes -\n"
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
	if (verb=="echo")
	{
		if (argc!=4)
		{
			std::cout <<
				"Usage:\n"
				"   config echo <config.ini> \"string containing GetSet[key]-style properties\"\n"
				"See also: config prep\n"
				;
			return 1;
		}
		std::string ini_file=argv[2];
		if (!GetSetIO::load<GetSetIO::IniFile>(ini_file))
		{
			std::cerr << "Failed to load " << ini_file << "!\n";
			return 2;
		}
		std::string str=argv[3];
		std::cout << replace(str) << std::endl;
		return 0;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////
	if (verb=="run")
	{
#ifdef HAS_RUN_VERB
		if (argc!=4)
		{
			std::cout <<
				"Usage:\n"
				"   config run <config.ini> \"commands containing GetSet[key]-style properties\"\n"
				"See also: config prep\n"
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
