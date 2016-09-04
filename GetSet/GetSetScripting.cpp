
#include "GetSetScripting.h"
#include "GetSetDictionary.h"

#include "GetSet.hxx"
#include "GetSetIO.h"

GetSetScriptParser::GetSetScriptParser(GetSetDictionary& _subject)
	: subject(_subject)
	, user_input(0x0)
	, user_output(0x0)
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
	output("GetSet scripting language \nhttps://sourceforge.net/p/getset/");
	output("To leave prompt type:\n   set key Prompt value false");
	GetSet<bool>("Prompt")=true;
	while (GetSet<bool>("Prompt")) {
		std::string command=input();
		parse(command);
	}
}

bool GetSetScriptParser::parse(const std::string& commands)
{
	parse_error_occured=false;
	parse_commands(commands);
	return parse_error_occured;
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

void GetSetScriptParser::output(const std::string& text)
{
	if (user_output) return user_output(text);
	else std::cout << text << std::endl;
}

std::string GetSetScriptParser::synopsis(const std::string& command, bool with_example)
{
	static std::map<std::string,std::string> help,examples;
	if (help.empty())
	{
		help    ["who"]     +="   who\n";
		help    ["exit"]    +="   exit <value>\n";
		help    ["set"]     +="   set var <varname> <value>\n";
		help    ["set"]     +="   set key <key> <value>\n";
		help    ["set"]     +="   set trigger <key>\n";
		examples["set"]     +="   set key \"Personal/Last Name\" var user_name\n";
		help    ["function"]+="   function <varname> ... endfunction\n";
		examples["function"]+="   function greetings\n";
		examples["function"]+="      echo value Hallo!\n";
		examples["function"]+="   endfunction\n";
		help    ["call"]    +="   call <varname>\n";
		examples["call"]    +="   call greetings\n";
		help    ["with"]    +="   with <section>\n";
		examples["with"]    +="   with \"Personal\"\n";
		examples["with"]    +="   set key \"Last Name\" value \"John\"\n";
		examples["with"]    +="   with \"\"\n";
		help    ["if"]      +="   if <value> [not] <op> <value>\n";
		help    ["if"]      +="   <op>:=strequal numequal gequal lequal greater less\n";
		examples["if"]      +="   if key \"Personal/Last Name\" not strequal \"John\"\n";
		examples["if"]      +="      echo value \"Name is not John\"\n";
		examples["if"]      +="   endif\n";
		help    ["while"]   +="   while <varname> ... endwhile\n";
		examples["while"]   +="   while active\n";
		examples["while"]   +="      call do_something\n";
		examples["while"]   +="   endwhile\n";
		help    ["for"]     +="   for each <varname> in <list> ... endfor\n";
		help    ["for"]     +="   <list>:=<value> with semicolon separated strings\n";
		help    ["for"]     +="   for each <varname> from <value> to <value> step <value> ... endfor\n";
		examples["for"]     +="   for each i from value 5 to value 9\n";
		examples["for"]     +="      echo var i\n";
		examples["for"]     +="   endfor\n";
		help    ["file"]    +="   file {load|save} ini <filename>\n";
		help    ["file"]    +="   file run <filename>\n";
		examples["file"]    +="   file save ini \"./file.ini\"\n";
		examples["file"]    +="   file run script.getset\n";
		help    ["input"]   +="   input <varname>\n";
		examples["input"]   +="   echo value \"What's your name?\"\n";
		examples["input"]   +="   input user_name\n";
		help    ["echo"]    +="   echo <value>\n";
		examples["echo"]    +="   echo value \"Hello World\"\n";
		examples["echo"]    +="   echo key user_name\n";
		examples["echo"]    +="   echo var \"Personal/Last Name\"\n";
		help    ["eval"]    +="   eval <varname> as <value> {plus|minus|times|over} <value>\n";
		examples["eval"]    +="   eval i as var i plus value 1\n";
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
void GetSetScriptParser::parse_commands(const std::string& commands)
{
	std::istringstream script(commands);
	while (!parse_error_occured && !script.eof())
	{
		std::string command;
		script >> command;
		if (command.empty() || command[0]=='#')
		{
			rest_of_line(script); // ignore rest of line
			continue;
		}
		else if (command == "exit") parse_exit(script);
		else if (command == "help") parse_help(script);
		else if (command == "who") parse_who(script);
		else if (command == "set") parse_set(script);
		else if (command == "function") parse_function(script);
		else if (command == "call") parse_call(script);
		else if (command == "with") parse_with(script);
		else if (command == "if") parse_if(script);
		else if (command == "while") parse_while(script);
		else if (command == "for") parse_for(script);
		else if (command == "file") parse_file(script);
		else if (command == "input") parse_input(script);
		else if (command == "echo") parse_echo(script);
		else if (command == "eval") parse_eval(script);
		else parse_error(command,"Unknown command.");
	}
}

void GetSetScriptParser::parse_error(const std::string& fn_name, const std::string& why)
{
	output(std::string("GetSetScriptParser::")+fn_name+" - "+why);
	std::istringstream str(fn_name);
	parse_error_occured=true;
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

void GetSetScriptParser::parse_exit(std::istream& script)
{
	auto line=rest_of_line(script);
	std::string optional_exit_code;
	get_token_string(line, optional_exit_code);
	expect_end_of_line(line,"exit");
	exit(stringTo<int>(optional_exit_code));
}

void GetSetScriptParser::parse_help(std::istream& script)
{
	auto line=rest_of_line(script);
	std::string command_name;
	get_token_string(line, command_name);
	expect_end_of_line(line,"help");
	output(synopsis(command_name,true));
}

void GetSetScriptParser::parse_who(std::istream& script)
{
	auto line=rest_of_line(script);
	if (!expect_end_of_line(line,"who")) return;
	std::vector<std::string> varnames;
	for (auto it=variables.begin();it!=variables.end();++it)
		varnames.push_back(it->first);
	output(vectorToString(varnames,"\n"));
}

void GetSetScriptParser::parse_set(std::istream& script)
{
	auto line=rest_of_line(script);
	int type=expect_keyword(line,"set","var;key;trigger");
	if (type<0) return;
	else if (type==0)
	{
		std::string varname, value;
		if (!expect_token_string(line,"set",varname)) return;
		if (!expect_token_value(line,"set",variables[varname])) return;
		expect_end_of_line(line,"set");
	}
	else
	{
		std::string key,value;
		if (!expect_token_key(line,"set",key)) return;
		if (type==2) // trigger an action
			GetSetGui::Button(key,subject).trigger();
		else // set value
			expect_token_value(line,"set",value);
		expect_end_of_line(line,"set");
			if (!parse_error_occured && type==1)
				GetSet<>(key,subject)=value;
	}
}

void GetSetScriptParser::parse_function(std::istream& script)
{
	auto line=rest_of_line(script);
	std::string varname;
	expect_token_string(line,"function",varname);
	expect_end_of_line(line,"function");
	if (!parse_error_occured)
		variables[varname]=get_block(script,"function");
}

void GetSetScriptParser::parse_call(std::istream& script)
{
	auto line=rest_of_line(script);
	std::string varname;
	expect_token_string(line,"call",varname);
	if (!expect_end_of_line(line,"call")) return;
	if (variables.find(varname)==variables.end())
		parse_error("call", varname + " variable undefined.");
	else
		parse_commands(variables[varname]);
}

void GetSetScriptParser::parse_with(std::istream& script)
{
	auto line=rest_of_line(script);
	expect_token_string(line,"with",section_prefix);
	expect_end_of_line(line,"with");
	if (!section_prefix.empty() && section_prefix.back()!='/')
		section_prefix.push_back('/');
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
		expect_token_value(line,"if (rhs)",rhs);
		result = lhs == rhs;
	}
	else
	{
		double lhs, rhs;
		expect_token_value(line,"if (lhs)",lhs);
		expect_token_value(line,"if (rhs)",rhs);
		if      (comparator==1) result = lhs==rhs;
		else if (comparator==2) result = lhs>=rhs;
		else if (comparator==3) result = lhs<=rhs;
		else if (comparator==4) result = lhs>rhs;
		else if (comparator==5) result = lhs<rhs;
	}
	expect_end_of_line(line,"if");
	if (negate) result=!result;
	std::string if_block=get_block(script,"if");
	if (!parse_error_occured && result)
		parse_commands(if_block);
}

void GetSetScriptParser::parse_while(std::istream& script)
{
	auto line=rest_of_line(script);
	std::string varname;
	expect_token_string(line,"while",varname);
	if (!expect_end_of_line(line,"while")) return;
	std::string while_block=get_block(script,"while");
	while (!parse_error_occured && stringTo<bool>(variables[varname]))
		parse_commands(while_block);
}

void GetSetScriptParser::parse_for(std::istream& script)
{
	auto line=rest_of_line(script);
	if (expect_keyword(line,"for","each")<0) return;
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
	std::string foreach_block=get_block(script,"for");
	for (auto value=values.begin();value!=values.end();++value)
	{
		if (parse_error_occured) break;
		variables[varname]=*value;
		parse_commands(foreach_block);
	}
}

void GetSetScriptParser::parse_file(std::istream& script)
{
	auto line=rest_of_line(script);
	std::string file;
	int action=expect_keyword(line,"file","load;save;run");
	if (action<0) return;
	if (action==0||action==1) // load/save
	{
		expect_keyword(line,"file","ini");
		if (!expect_token_value(line,"file",file)) return;
		if (!expect_end_of_line(line,"file")) return;
		if (action==0) GetSetIO::load<GetSetIO::IniFile>(file);		
		else           GetSetIO::save<GetSetIO::IniFile>(file);	
	}
	else if (action==2) // run
	{
		if (!expect_token_string(line,"file",file)) return;
		if (!expect_end_of_line(line,"file")) return;
		std::string commands=fileReadString(file);
		if (commands.empty()) parse_error("file","File not found or file empty.");
		else parse_commands(commands);
	}
}

void GetSetScriptParser::parse_input(std::istream& script)
{
	auto line=rest_of_line(script);
	std::string varname;
	if (!expect_token_string(line,"input",varname)) return;
	if (!expect_end_of_line(line,"input")) return;
	variables[varname]=input();
}

void GetSetScriptParser::parse_echo(std::istream& script)
{
	auto line=rest_of_line(script);
	std::string value;
	if (!expect_token_value(line,"echo",value)) return;
	if (!expect_end_of_line(line,"echo")) return;
	output(value);
}

void GetSetScriptParser::parse_eval(std::istream& script)
{
	auto line=rest_of_line(script);
	// Parse: <varname> as ...
	std::string varname;
	if (!get_token_string(line,varname))
	{
		parse_error("eval","Failed to parse variable name.");
		return;
	}
	if (expect_keyword(line,"eval","as")<0) return;
	// Parse: <numeric value> op <numeric value> ...
	double lhs, rhs;
	if (!expect_token_value(line,"eval (lhs)",lhs)) return;
	int op=expect_keyword(line,"eval","plus;minus;times;over");
	if (op<0 || !expect_token_value(line,"eval (rhs)",rhs)) return;
	// Calculation
	if (op==0)      variables[varname]=toString(lhs+rhs);
	else if (op==1) variables[varname]=toString(lhs-rhs);
	else if (op==2) variables[varname]=toString(lhs*rhs);
	else if (op==3) variables[varname]=toString(lhs/rhs);
	expect_end_of_line(line,"eval");
}

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
		if (line.front()!='#') block+=line+"\n";
	}
	if (stack_depth!=0)
	{
		parse_error(end_block,"Unexpected end of file.");
		return "";
	}
	return block;
}
