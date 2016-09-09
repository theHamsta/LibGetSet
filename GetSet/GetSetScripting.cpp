
#include "GetSetScripting.h"
#include "GetSetDictionary.h"

#include "GetSet.hxx"
#include "GetSetIO.h"

//
// GetSetScriptParser
//

GetSetScriptParser::GetSetScriptParser(GetSetDictionary& _subject)
	: subject(_subject)
	, user_input(0x0)
	, parse_error_occured(false)
{}

GetSetScriptParser& GetSetScriptParser::global()
{
	static GetSetScriptParser* instance;
	if (!instance)
		instance=new GetSetScriptParser(GetSetDictionary::global());
	return *instance;
}

bool GetSetScriptParser::good() const
{
	return !parse_error_occured;
}

void GetSetScriptParser::prompt()
{
	printErr("GetSet scripting language \nhttps://sourceforge.net/p/getset/");
	printErr("To leave prompt type:\n   set key Prompt value false");
	GetSet<bool>("Prompt")=true;
	while (GetSet<bool>("Prompt")) {
		std::string command=input();
		parse(command);
	}
}

bool GetSetScriptParser::parse(const std::string& commands, const std::string& scriptname)
{
	parse_error_occured=false;
	parse_commands(commands, scriptname);
	return !parse_error_occured;
}

void GetSetScriptParser::force_stop()
{
	parse_error("User interaction","Forced stop.");
}

void GetSetScriptParser::addOutputCallback(void *identifier,  void (*callback)(const std::string&, void*))
{
	if (!callback && user_out.find(identifier)!=user_out.end())
		user_out.erase(user_out.find(identifier));
	else user_out[identifier]=callback;
}

void GetSetScriptParser::addErrorCallback(void *identifier,  void (*callback)(const std::string&, void*))
{
	if (!callback && user_err.find(identifier)!=user_err.end())
		user_err.erase(user_err.find(identifier));
	else user_err[identifier]=callback;
}

void GetSetScriptParser::printOut(const std::string& text)
{
	if (user_out.empty())
		std::cout << text << std::endl;
	else
		for (auto it=user_out.begin();it!=user_out.end();++it)
			if (it->second) it->second(text, it->first);
}

void GetSetScriptParser::printErr(const std::string& text)
{
	// The identifier 0x0 is user as a special callback which only forces a GUI update
	// It is set by GetSetApplication by default.
	if (user_err.empty()||(user_err.size()==1 && user_err.find(0x0)!=user_err.end()))
	{
		if (!text.empty() && text.front()!='@')
			std::cerr << text << std::endl;
	}
	else
		for (auto it=user_err.begin();it!=user_err.end();++it)
			if (it->second) it->second(text, it->first);
}

std::string GetSetScriptParser::input()
{
	if (user_input) return user_input();
	else {
		std::cout << ">> ";
		std::string line;
		std::getline(std::cin,line);
		if (line.empty())
		{
			std::cout << "Multiline mode. Next empty line parses all lines at once.\n";
			line=input();
			std::string next_line=line;
			while (!next_line.empty()) {
				std::getline(std::cin,next_line);
				line=line+"\n"+next_line;
			}
		}
		return line;
	}
}

std::string GetSetScriptParser::synopsis(const std::string& command, bool with_example)
{
	static std::map<std::string,std::string> help,examples;
	if (help.empty())
	{
		help    ["who"]     +="   who\n";
		help    ["exit"]    +="   exit <value:int>\n";
		help    ["set"]     +="   set var <varname> to <value>\n";
		help    ["set"]     +="   set key <key> to <value>\n";
		help    ["set"]     +="   set trigger <key>\n";
		examples["set"]     +="   set key \"Personal/Last Name\" to var user_name\n";
		help    ["discard"] +="   discard key <key>\n";
		help    ["discard"] +="   discard {var|function} <varname>\n";
		examples["discard"] +="   discard key \"Personal/Last Name\"";
		help    ["define"]  +="   define {var|function} <varname:function> ... endfunction\n";
		examples["define"]  +="   define function greetings\n";
		examples["define"]  +="      echo value Hallo!\n";
		examples["define"]  +="   enddefine\n";
		help    ["call"]    +="   call function <varname:function>\n";
		examples["call"]    +="   call function greetings\n";
		help    ["with"]    +="   with section <key:section>\n";
		examples["with"]    +="   with section \"Personal\"\n";
		examples["with"]    +="   set key \"First Name\" to value \"John\"\n";
		examples["with"]    +="   with section \"\"\n";
		help    ["if"]      +="   if  [not] <op> <value> to <value>\n";
		help    ["if"]      +="   <op>:=strequal or numequal (test for string or numeric equality)\n";
		help    ["if"]      +="   if  [not] <op> <value> than <value>\n";
		help    ["if"]      +="   <op>:=gequal lequal greater less (compare two numeric values)\n";
		examples["if"]      +="   if not strequal key \"Personal/Last Name\" to value \"John\"\n";
		examples["if"]      +="      echo value \"Name is not John\"\n";
		examples["if"]      +="   endif\n";
		help    ["while"]   +="   while <varname> ... endwhile\n";
		examples["while"]   +="   while active\n";
		examples["while"]   +="      call do_something\n";
		examples["while"]   +="   endwhile\n";
		help    ["for"]     +="   for each var <varname> in <value:list> ... endfor\n";
		help    ["for"]     +="   <value:list> is a semicolon separated list of strings\n";
		help    ["for"]     +="   for each var <varname> from <value> to <value> step <value> ... endfor\n";
		examples["for"]     +="   for each var i from value 5 to value 9\n";
		examples["for"]     +="      echo var i\n";
		examples["for"]     +="   endfor\n";
		help    ["file"]    +="   file ini {load|save} <filename>\n";
		help    ["file"]    +="   file ini remove key <key> from <value:filename>\n";
		help    ["file"]    +="   file ini set key <key> in <value:filename> to <value>\n";
		help    ["file"]    +="   file ini get var <varname> from key <key> in <value:filename>\n";
		help    ["file"]    +="   file run <value:filename>\n";
		examples["file"]    +="   file ini save value \"./file.ini\"\n";
		examples["file"]    +="   file run script.getset\n";
		help    ["input"]   +="   input var <varname>\n";
		examples["input"]   +="   echo value \"What's your name?\"\n";
		examples["input"]   +="   input user_name\n";
		help    ["echo"]    +="   echo <value> [and <value>]*\n";
		examples["echo"]    +="   echo key \"Personal/First Name\"\n";
		examples["echo"]    +="   echo value \"Hello \" and var user_name and value \"!\"\n";
		examples["echo"]    +="   echo var \"Personal/Last Name\"\n";
		help    ["eval"]    +="   eval var <varname> from <value> [{plus|minus|times|over} <value>]+\n";
		examples["eval"]    +="   eval var i from var i times value \"2\" plus value 1 times value 0.5\n";
		examples["eval"]    +="   (careful: left-to-right evaluation ((i*2)+1)*0.5 )\n";
		help    ["concat"]  +="   concat var <varname> from <value> [and <value>]+\n";
		examples["concat"]  +="   concat var c from var a and var b\n";
	}
	std::string help_message;
	if (command.empty() || help.find(command)==help.end())
	{
		help_message="Try: help <command>\ncommands:";
		for (auto it=help.begin();it!=help.end();++it)
			help_message=help_message+" "+it->first;
		help_message=help_message+"\n";
	}
	else
	{
		help_message="Synopsis:\n";
		help_message=help_message+help[command];
		if (with_example && examples.find(command)!=examples.end())
			help_message=help_message+"Example:\n"+examples[command];
	}
	return help_message;	
}

std::string GetSetScriptParser::state()
{
	std::string ret;
	for (auto it=variables.begin();it!=variables.end();++it)
	{
		if (it->second.find_first_of('\n')!=std::string::npos) // FIXME multiline ??
		{
			ret=ret+"function "+it->first+"\n";
			ret=ret+it->second+"\n";
			ret=ret+"endfunction\n";
		}
		else ret=ret+ "set var " + it->first + " value \"" + it->second + "\"\n";
	}
	return ret;
}

void GetSetScriptParser::parse_commands(const std::string& commands, const std::string& file_or_function_name)
{
	std::string current_location;
	std::istringstream script(commands);
	while (!parse_error_occured && !script.eof())
	{
		std::string command;
		script >> std::ws;
		current_location=location(script);
		printErr(std::string("@")+file_or_function_name+" "+current_location);
		script >> command;
		if (command.empty() || command[0]=='#')
		{
			rest_of_line(script); // ignore rest of line
			continue;
		}
		else if (command == "help") parse_help(script);
		else if (command == "call") parse_call(script);
		else if (command == "concat") parse_concat(script);
		else if (command == "define") parse_define(script);
		else if (command == "discard") parse_discard(script);
		else if (command == "echo") parse_echo(script);
		else if (command == "eval") parse_eval(script);
		else if (command == "exit") parse_exit(script);
		else if (command == "file") parse_file(script);
		else if (command == "for") parse_for(script);
		else if (command == "if") parse_if(script);
		else if (command == "input") parse_input(script);
		else if (command == "set") parse_set(script);
		else if (command == "while") parse_while(script);
		else if (command == "who") parse_who(script);
		else if (command == "with") parse_with(script);
		else parse_error(command,"Unknown command.");
	}
	if (parse_error_occured)
		printErr(std::string("   called from ") + file_or_function_name + " @" + current_location);
	else
		printErr(std::string("@")+file_or_function_name+" success.");
}

void GetSetScriptParser::parse_error(const std::string& fn_name, const std::string& why)
{
	printErr(fn_name+" - "+why);
	std::istringstream str(fn_name);
	parse_error_occured=true;
}

std::string GetSetScriptParser::location(std::istream& script)
{
	auto pos=script.tellg();
	if (pos<0) return "end of file";
	script.seekg(0,script.beg);
	std::string commands;
	getline(script,commands,'\0');
	script.seekg(pos);
	int line_number=(int)std::count(commands.begin(),commands.begin()+pos,'\n') ;
	std::string location=std::string("line: ")+toString(line_number)+ " ch:" +toString(pos);
	return location;
}

bool GetSetScriptParser::get_token_string(std::istream& script, std::string& token)
{
	if (!script||script.eof()) return false;
	script >> std::ws;
	if (script.peek()=='\"')
	{
		script.get();
		getline(script, token, '\"');
	}
	else
		script >> token;
	return true;
}

bool GetSetScriptParser::expect_token_string(std::istream& script, const std::string& fn_name, std::string& token)
{
	if (!get_token_string(script, token))
		parse_error(fn_name, "Expected: <string>");
	return !parse_error_occured;
}

bool GetSetScriptParser::expect_token_key(std::istream& script, const std::string& fn_name, std::string& token)
{
	if (!get_token_string(script, token))
		parse_error(fn_name, "Expected: <key>");
	else token=section_prefix+token;
	return !parse_error_occured;
}

bool GetSetScriptParser::expect_token_value(std::istream& script, const std::string& fn_name, std::string& token, bool numeric)
{
	std::string value;
	int type=expect_keyword(script, "expect_token_value", "value;key;var");
	if (type<0 || !get_token_string(script, value)) {
		parse_error(fn_name, numeric?"Expected: <numeric value>":"Expected: <value>");
		return false;
	}
	if      (type==0) token=value; 
	else if (type==1) token=GetSet<>(section_prefix+value);
	else if (type==2) token=variables[value];
	return !parse_error_occured;
}

bool GetSetScriptParser::expect_token_value(std::istream& script, const std::string& fn_name, double& token)
{
	std::string value;
	expect_token_value(script,fn_name,value,true);
	token=stringTo<double>(value);
	return !parse_error_occured;
}



//
// Commands: call concat define discard echo eval exit file for if input set while who with
//

void GetSetScriptParser::parse_help(std::istream& script)
{
	auto line=rest_of_line(script);
	std::string command_name;
	get_token_string(line, command_name);
	expect_end_of_line(line,"help");
	printErr(synopsis(command_name,true));
}

void GetSetScriptParser::parse_call(std::istream& script)
{
	auto line=rest_of_line(script);
	if (expect_keyword(line,"call","var;function")<0) return;
	std::string varname;
	expect_token_string(line,"call",varname);
	if (!expect_end_of_line(line,"call")) return;
	if (variables.find(varname)==variables.end())
		parse_error("call", varname + " variable undefined.");
	else
		parse_commands(variables[varname], std::string("function ")+varname);
}

void GetSetScriptParser::parse_concat(std::istream& script)
{
	auto line=rest_of_line(script);
	// Parse: var <varname> from ...
	if (expect_keyword(line,"concat","var")<0) return;
	std::string varname;
	if (!expect_token_string(line,"file",varname)) return;
	if (expect_keyword(line,"concat","from")<0) return;
	// Parse: <numeric value> op <numeric value> ...
	std::string value, rhs;
	if (!expect_token_value(line,"concat (lhs)",value)) return;
	while (!line.eof() && !parse_error_occured)
	{
		int op=expect_keyword(line,"concat","and");
		if (op<0 || !expect_token_value(line,"concat (rhs)",rhs)) return;
		// Concatenation
		value=value+rhs;
		line >> std::ws;
	}
	if (!parse_error_occured)
		variables[varname]=toString(value);
}

void GetSetScriptParser::parse_define(std::istream& script)
{
	auto line=rest_of_line(script);
	if (expect_keyword(line,"define","var;function")<0) return;
	std::string varname;
	expect_token_string(line,"define",varname);
	expect_end_of_line(line,"define");
	auto pos_start=script.tellg();
	std::string block_start=location(script);
	std::string function_block=get_block(script,"define");
	std::string thisblock=std::string("# function ")+ varname + " (at " + block_start + " to " + location(script)+")";
	if (!parse_error_occured)
		variables[varname]=thisblock+"\n"+function_block;
}

void GetSetScriptParser::parse_discard(std::istream& script)
{
	auto line=rest_of_line(script);
	int type=expect_keyword(line,"discard","key;var;function");
	std::string var_or_key_name;
	if (!expect_token_string(line,"discard",var_or_key_name)) return;
	if (!expect_end_of_line(line,"discard")) return;
	if (type<0) return;
	else if (type==0)
		GetSetDictionary::global().remove(var_or_key_name);
	else
	{
		auto it=variables.find(var_or_key_name);
		if (it!=variables.end())
			variables.erase(it);
	}
}

void GetSetScriptParser::parse_echo(std::istream& script)
{
	auto line=rest_of_line(script);
	std::string value;
	if (!expect_token_value(line,"echo",value)) return;
	while (line && !line.eof())
	{
		std::string and;
		get_token_string(line,and);
		if (and=="and")
		{
			std::string next_value;
			if (!expect_token_value(line,"echo",value)) return;
			value=value+next_value;
		}
		else if (!and.empty())
			parse_error("echo",std::string("Expected \"and\" but found " + and));
	}
	if (!expect_end_of_line(line,"echo")) return;
	printOut(value);
}

void GetSetScriptParser::parse_eval(std::istream& script)
{
	auto line=rest_of_line(script);
	// Parse: var <varname> as ...
	if (expect_keyword(line,"eval","var")<0) return;
	std::string varname;
	if (!expect_token_string(line,"file",varname)) return;
	if (expect_keyword(line,"eval","from")<0) return;
	// Parse: <numeric value> op <numeric value> ...
	double value, rhs;
	if (!expect_token_value(line,"eval (lhs)",value)) return;
	while (!line.eof() && !parse_error_occured)
	{
		int op=expect_keyword(line,"eval","plus;minus;times;over");
		if (op<0 || !expect_token_value(line,"eval (rhs)",rhs)) return;
		// Calculation
		if (op==0)      value+=rhs;
		else if (op==1) value-=rhs;
		else if (op==2) value*=rhs;
		else if (op==3) value/=rhs;
		line >> std::ws;
	}
	if (!parse_error_occured)
		variables[varname]=toString(value);
}

void GetSetScriptParser::parse_exit(std::istream& script)
{
	auto line=rest_of_line(script);
	std::string optional_exit_code;
	get_token_string(line, optional_exit_code);
	expect_end_of_line(line,"exit");
	exit(stringTo<int>(optional_exit_code));
}

void GetSetScriptParser::parse_if(std::istream& script)
{
	auto line=rest_of_line(script);
	bool result;
	bool negate=false;
	const std::string comparators="strequal;numequal;gequal;lequal;greater;less";
	int comparator=expect_keyword(line,"if",comparators+";not");
	negate=comparator==6; // not followed by comparator
	if (negate) comparator=expect_keyword(line,"if",comparators);
	if (comparator<0) return;
	if (comparator==0) // strequal
	{
		std::string lhs, rhs;
		expect_token_value(line,"if (lhs)",lhs);
		if (expect_keyword(line,"if","to")<0) return;
		expect_token_value(line,"if (rhs)",rhs);
		result = lhs == rhs;
	}
	else
	{
		double lhs, rhs;
		expect_token_value(line,"if (lhs)",lhs);
		if (comparator==1 && expect_keyword(line,"if","to")<0) return;
		if (comparator!=1 && expect_keyword(line,"if","than")<0) return;
		expect_token_value(line,"if (rhs)",rhs);
		if      (comparator==1) result = lhs==rhs;
		else if (comparator==2) result = lhs>=rhs;
		else if (comparator==3) result = lhs<=rhs;
		else if (comparator==4) result = lhs>rhs;
		else if (comparator==5) result = lhs<rhs;
	}
	if (negate) result=!result;
	expect_end_of_line(line,"if");
	auto pos_start=script.tellg();
	std::string block_start=location(script);
	std::string if_block=get_block(script,"if");
	auto pos_end=script.tellg();
	std::string thisblock=std::string("if block (at ") + block_start + " + " + toString(pos_end-pos_start)+")";
	if (!parse_error_occured && result)
		parse_commands(if_block, thisblock );
}

void GetSetScriptParser::parse_file(std::istream& script)
{
	auto line=rest_of_line(script);
	std::string file;
	int what=expect_keyword(line,"file","ini;run");
	if (what==1) // run
	{
		if (!expect_token_string(line,"file",file)) return;
		if (!expect_end_of_line(line,"file")) return;
		std::string commands=fileReadString(file);
		if (commands.empty()) parse_error("file","File not found or file empty.");
		else parse_commands(commands, std::string("file \"")+file+"\"");
	}
	else // ini
	{
		int action=expect_keyword(line,"file","load;save;get;set;remove");
		if (action<0) return;
		if (action==0||action==1) // load/save
		{
			if (!expect_token_value(line,"file",file)) return;
			if (!expect_end_of_line(line,"file")) return;
			if (action==0) GetSetIO::load<GetSetIO::IniFile>(file);		
			else           GetSetIO::save<GetSetIO::IniFile>(file);	
		}
		if (action==2) // get
		{
			//  file ini get var <varname> from key <key> in <value:filename>\n";
			if (expect_keyword(line,"file","var")<0) return;
			std::string varname;
			if (!expect_token_string(line,"file",varname)) return;
			if (expect_keyword(line,"file","from")<0) return;
			if (expect_keyword(line,"file","key")<0) return;
			std::string keyname;
			if (!expect_token_string(line,"file",keyname)) return;
			if (expect_keyword(line,"file","in")<0) return;
			std::string ini_file;
			if (!expect_token_string(line,"file",ini_file)) return;
			if (!expect_end_of_line(line,"file")) return;
			// Implementation:
			GetSetDictionary file_contents;
			if (!GetSetIO::load<GetSetIO::IniFile>(ini_file,file_contents)) {
				parse_error("file", ini_file + " could not be loaded.");
				return;
			}
			std::string value=GetSet<>(keyname,file_contents);
			variables[varname]=value;
		}
		else if (action==3) // set
		{
			//  file ini set key <key> in <value:filename> to <value>\n";
			if (expect_keyword(line,"file","key")<0) return;
			std::string keyname;
			if (!expect_token_string(line,"file",keyname)) return;
			if (expect_keyword(line,"file","in")<0) return;
			std::string ini_file;
			if (!expect_token_string(line,"file",ini_file)) return;
			if (expect_keyword(line,"file","to")<0) return;
			std::string value;
			if (!expect_token_value(line,"file",value)) return;
			if (!expect_end_of_line(line,"file")) return;
			// Implementation:
			GetSetDictionary file_contents;
			if (!GetSetIO::load<GetSetIO::IniFile>(ini_file,file_contents)) {
				// if file does not exist, we may still want to set some values.
			}
			GetSet<>(keyname,file_contents)=value;
			if (!GetSetIO::save<GetSetIO::IniFile>(ini_file,file_contents))
				parse_error("file", ini_file + " could not be saved.");
		}
		else // remove
		{
			if (expect_keyword(line,"file","key")<0) return;
			std::string keyname;
			if (!expect_token_string(line,"file",keyname)) return;
			if (expect_keyword(line,"file","from")<0) return;
			std::string ini_file;
			if (!expect_token_string(line,"file",ini_file)) return;
			if (!expect_end_of_line(line,"file")) return;
			// Implementation:
			GetSetDictionary file_contents;
			if (!GetSetIO::load<GetSetIO::IniFile>(ini_file,file_contents)) {
				parse_error("file", ini_file + " could not be loaded.");
				return;
			}
			file_contents.remove(keyname);
			if (!GetSetIO::save<GetSetIO::IniFile>(ini_file,file_contents))
				parse_error("file", ini_file + " could not be saved.");
		}
	}
}

void GetSetScriptParser::parse_for(std::istream& script)
{
	auto line=rest_of_line(script);
	if (expect_keyword(line,"for","each")<0) return;
	if (expect_keyword(line,"for","var")<0) return;
	std::string varname;
	if (!expect_token_string(line,"for",varname)) return;
	int op=expect_keyword(line,"for","in;from");
	std::vector<std::string> values;
	if (op<0) return;
	else if (op==0)
	{
		std::string semicolon_separated_values;
		if (!expect_token_value(line,"for",semicolon_separated_values)) return;
		values=stringToVector<std::string>(semicolon_separated_values,';',true);
	}
	else if(op==1)
	{
		double a,b,step;
		if (!expect_token_value(line,"for (from)",a)) return;
		if (expect_keyword(line,"for","to")<0) return;
		if (!expect_token_value(line,"for (to)",b)) return;
		if (expect_keyword(line,"for","step")<0) return;
		if (!expect_token_value(line,"for (step)",step)) return;
		if (a<b) for (double d=a;d<=b;d+=step) values.push_back(toString(d));
		else     for (double d=a;d>=b;d-=step) values.push_back(toString(d));
	}
	expect_end_of_line(line,"for");
	auto pos_start=script.tellg();
	std::string block_start=location(script);
	std::string foreach_block=get_block(script,"for");
	auto pos_end=script.tellg();
	std::string thisblock=std::string("for block (at ") + block_start + " + " + toString(pos_end-pos_start)+")";
	for (auto value=values.begin();value!=values.end();++value)
	{
		if (parse_error_occured) break;
		variables[varname]=*value;
		parse_commands(foreach_block, thisblock);
	}
}

void GetSetScriptParser::parse_input(std::istream& script)
{
	auto line=rest_of_line(script);
	if (expect_keyword(line,"input","var")<0) return;
	std::string varname;
	if (!expect_token_string(line,"input",varname)) return;
	if (!expect_end_of_line(line,"input")) return;
	variables[varname]=input();
}

void GetSetScriptParser::parse_set(std::istream& script)
{
	auto line=rest_of_line(script);
	int type=expect_keyword(line,"set","var;key;trigger");
	if (type<0) return;
	else if (type==0) // set var to <value>
	{
		std::string varname, value;
		if (!expect_token_string(line,"set",varname)) return;
		if (expect_keyword(line,"set","to")<0) return;
		if (!expect_token_value(line,"set",variables[varname])) return;
		expect_end_of_line(line,"set");
	}
	else
	{
		std::string key,value;
		if (!expect_token_key(line,"set",key)) return;
		if (type==2) // trigger an action
			GetSetGui::Button(key,subject).trigger();
		else // set key to <value>
		{
			if (expect_keyword(line,"set","to")<0) return;
			expect_token_value(line,"set",value);
		}
		expect_end_of_line(line,"set");
		if (!parse_error_occured && type==1)
			GetSet<>(key,subject)=value;
	}
}

void GetSetScriptParser::parse_while(std::istream& script)
{
	auto line=rest_of_line(script);
	if (expect_keyword(line,"set","var")<0) return;
	std::string varname;
	expect_token_string(line,"while",varname);
	if (!expect_end_of_line(line,"while")) return;
	auto pos_start=script.tellg();
	std::string block_start=location(script);
	std::string while_block=get_block(script,"while");
	auto pos_end=script.tellg();
	std::string thisblock=std::string("while block (at ") + block_start + " + " + toString(pos_end-pos_start)+")";
	while (!parse_error_occured && stringTo<bool>(variables[varname]))
		parse_commands(while_block, thisblock );
}

void GetSetScriptParser::parse_who(std::istream& script)
{
	auto line=rest_of_line(script);
	if (!expect_end_of_line(line,"who")) return;
	std::vector<std::string> varnames;
	for (auto it=variables.begin();it!=variables.end();++it)
		varnames.push_back(it->first);
	printOut(vectorToString(varnames,"\n"));
}

void GetSetScriptParser::parse_with(std::istream& script)
{
	auto line=rest_of_line(script);
	if (expect_keyword(line,"with","section")<0) return;
	expect_token_string(line,"with",section_prefix);
	expect_end_of_line(line,"with");
	if (!section_prefix.empty() && section_prefix.back()!='/')
		section_prefix.push_back('/');
}

//
// Commands end
//

std::stringstream GetSetScriptParser::rest_of_line(std::istream& script)
{
	std::string line;
	getline(script, line, '\n');
	return std::stringstream(line);
}

bool GetSetScriptParser::expect_end_of_line(std::istream& script, const std::string& fn_name)
{
	script >> std::ws;
	if (!script.eof()) parse_error(fn_name,"Expected end of line.");
	return !parse_error_occured;
}

int GetSetScriptParser::expect_keyword(std::istream& script, const std::string& fn_name, const std::string& keywords)
{
	std::string keyword;
	script >> keyword;
	int index=-1;
	std::vector<std::string> c=stringToVector<std::string>(keywords,';');
	for (int i=0;i<(int)c.size();i++)
		if (c[i]==keyword) index=i;
	if (index<0) parse_error(fn_name,std::string("Expected ") + keywords + " but found "+(keyword.empty()?"nothing.":keyword));
	return index;
}

std::string GetSetScriptParser::get_block(std::istream& script, const std::string& block_name)
{
	std::string end_block=std::string("end")+block_name;
	std::string block;
	bool block_found=false;
	int stack_depth=1;
	while (script && !script.eof())
	{
		std::string line,command;
		script >> std::ws;
		getline(script, line, '\n');
		std::istringstream line_str(line);
		line_str >> command;
		if (command==block_name)
			stack_depth++;
		else if (command.length()>=end_block.size() && command.substr(0,end_block.size())==end_block )
		{
			expect_end_of_line(line_str,std::string("After ")+end_block);
			stack_depth--;
			if (stack_depth==0) break;
		}
		if (!line.empty() && line.front()!='#') block+=line+"\n";
	}
	if (stack_depth!=0)
	{
		parse_error(end_block,"Unexpected end of file.");
		return "";
	}
	return block;
}
