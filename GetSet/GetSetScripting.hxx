//
//  Library: GetSet
//  c++ library for load/saving *typed* and *named* properties and automatic GUI.
//  
//  Copyright (c) by André Aichert (aaichert@gmail.com)
//    
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//  
//    http://www.apache.org/licenses/LICENSE-2.0
//    
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
//

#ifndef __GetSetScripting_hxx
#define __GetSetScripting_hxx

#include "GetSet.hxx"
#include "GetSetIO.h"

#include <sstream>

/// Store the current state and record all changes with the possibility of playing them back.
// class GetSetScriptRecorder TODO
/// This is where a detailed log of all events will be stored
//	std::ofstream file;

/// Parse a script line-by-line. See ScriptSyntax.txt for more info.
class GetSetScriptParser
{
public:
	GetSetScriptParser(GetSetDictionary& _subject=GetSetDictionary::global());

	/// Callback for user input (optional)
	std::string (*user_input)();

	/// Callback for output (optional)
	void (*user_output)(const std::string&);

	/// Parse the scrip provided by text
	bool parse(const std::string& commands)
	{
		parse_error_occured=false;
		parse_commands(commands);
		return parse_error_occured;
	}

protected:
	/// Only this dictionary will be affectd by this parser
	GetSetDictionary& subject;
	
	/// Map of temporary variables and command lists (i.e. subroutines)
	std::map<std::string,std::string> variables;

	/// A section used as prefix for all keys ("with" command)
	std::string section_prefix;

	/// Set to true on syntax error
	bool parse_error_occured;
	
	/// Parse the scrip provided by text
	void parse_commands(const std::string& script);

	/// Report a parse error
	void parse_error(const std::string& where, const std::string& why);

	//
	// Tokens: <string> <varname> <value> <key> <section>
	//

	/// Token: <string> (or: <varname> <file> <key> and <section>)
	bool get_token_string(std::istream& script, std::string& token);
	/// Token: <key> and <section>
	bool get_token_key(std::istream& script, std::string& token);
	/// Token: <value>
	bool get_token_value(std::istream& script, std::string& token);

	//
	// Commands: var list call with if set trigger while foreach file input output
	//

	/// var <varname> <value>
	void parse_var(std::istream& script);
	/// list <section> ... endlist
	void parse_list(std::istream& script);
	/// call <varname>
	void parse_call(std::istream& script);
	/// with <section> ... endwith
	void parse_with(std::istream& script);
	/// with <section> ... endwith
	void parse_endwith(std::istream& script);
	/// if [not] {strequal|numequal|gequal|lequal|greater|less} <value> <value> ... endif
	void parse_if(std::istream& script);
	/// set <key> <value>
	void parse_set(std::istream& script);
	/// trigger <key> 
	void parse_trigger(std::istream& script);
	/// while <value> ... endwhile
	void parse_while(std::istream& script);
	/// foreach <varname> in value endforeach
	void parse_foreach(std::istream& script);
	// file [run|save ini|load ini] <file>
	void parse_file(std::istream& script);
	// input
	void parse_input(std::istream& script);
	// output
	void parse_output(std::istream& script);
	
	/// Advance to the next line
	std::stringstream rest_of_line(std::istream& script);

	/// Make sure end of line or end of file is reached
	void expect_end_of_line(std::istream& script, const std::string& fn_name);

	/// Access everything from current location to line starting with end_block
	std::string get_block(std::istream& script, const std::string& end_block);
};


//
// GetSetScripting Implementation
//

GetSetScriptParser::GetSetScriptParser(GetSetDictionary& _subject)
	: subject(_subject)
	, user_input(0x0)
	, user_output(0x0)
	, parse_error_occured(false)
{}

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
		else if (command == "var") parse_var(script);
		else if (command == "list") parse_list(script);
		else if (command == "call") parse_call(script);
		else if (command == "with") parse_with(script);
		else if (command == "endwith") parse_endwith(script);
		else if (command == "if") parse_if(script);
		else if (command == "set") parse_set(script);
		else if (command == "trigger") parse_trigger(script);
		else if (command == "while") parse_while(script);
		else if (command == "foreach") parse_foreach(script);
		else if (command == "file") parse_file(script);
		else if (command == "input") parse_input(script);
		else if (command == "output") parse_output(script);
		else parse_error(command,"Unknown command.");
	}
}

void GetSetScriptParser::parse_error(const std::string& where, const std::string& why)
{
	if (user_output) (*user_output)(where+": "+why);
	else std::cerr << "GetSetScriptParser::error - " << where << ": " << why << std::endl;
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

bool GetSetScriptParser::get_token_key(std::istream& script, std::string& token)
{
	if (!get_token_string(script, token))
		return false;
	token=section_prefix+token;
	return true;
}

bool GetSetScriptParser::get_token_value(std::istream& script, std::string& token)
{
	if (!script||script.eof()) return false;
	std::string type, value;
	script >> type;
	if (!get_token_string(script, value))
		return false;
	if (type=="value")
		token=value; 
	else if (type=="key")
		token=GetSet<>(section_prefix+value);
	else if (type=="var")
		token=variables[value];
	else
		return false;
	return true;
}


void GetSetScriptParser::parse_var(std::istream& script)
{
	auto line=rest_of_line(script);
	std::string varname, value;
	if (!get_token_string(line,varname))
		parse_error("parse_var","Failed to parse string.");
	variables[varname]="";
	while (get_token_value(line,value))
		variables[varname]+=value;
	expect_end_of_line(line,"parse_var");
}

void GetSetScriptParser::parse_list(std::istream& script)
{
	auto line=rest_of_line(script);
	std::string varname;
	if (!get_token_string(line,varname))
		parse_error("parse_list","Failed to parse variable name.");
	expect_end_of_line(line,"parse_list");
	variables[varname]=get_block(script,"endlist");
}

void GetSetScriptParser::parse_call(std::istream& script)
{
	auto line=rest_of_line(script);
	std::string varname;
	if (!get_token_string(line,varname))
		parse_error("parse_call","Failed to parse variable name.");
	expect_end_of_line(line,"parse_call");
	if (variables.find(varname)==variables.end())
		parse_error("parse_call", varname + " variable undefined.");
	else
		parse_commands(variables[varname]);
}

void GetSetScriptParser::parse_with(std::istream& script)
{
	auto line=rest_of_line(script);
	if (!get_token_string(line,section_prefix))
		parse_error("parse_var","Failed to parse section name.");
	if (!section_prefix.empty() && section_prefix.back()!='/')
		section_prefix.push_back('/');
	expect_end_of_line(line,"parse_with");
}

void GetSetScriptParser::parse_endwith(std::istream& script)
{
	auto line=rest_of_line(script);
	expect_end_of_line(line,"parse_endwith");
	section_prefix.clear();
}

void GetSetScriptParser::parse_if(std::istream& script)
{
	auto line=rest_of_line(script);
	bool result;
	bool negate=false;
	std::string comparator;
	line >> comparator;
	if (comparator=="not")
	{
		negate=true;
		line >> comparator;
	}
	std::string lhs, rhs;
	if (!get_token_value(line,lhs))
			parse_error("parse_if","Failed to parse first argument.");
	if (!get_token_value(line,rhs))
			parse_error("parse_if","Failed to parse second argument.");
	expect_end_of_line(line,"parse_if");
	// Comparison
	if (comparator=="strequal")
		result = lhs == rhs;
	else if (comparator=="numequal")
		result = stringTo<double>(lhs) == stringTo<double>(rhs);
	else if (comparator=="gequal")
		result = stringTo<double>(lhs) >= stringTo<double>(rhs);
	else if (comparator=="lequal")
		result = stringTo<double>(lhs) <= stringTo<double>(rhs);
	else if (comparator=="greater")
		result = stringTo<double>(lhs) > stringTo<double>(rhs);
	else if (comparator=="less")
		result = stringTo<double>(lhs) < stringTo<double>(rhs);
	else {
		parse_error("parse_if","Failed to parse comparator.");
		return;
	}
	if (negate) result=!result;
	std::string if_block=get_block(script,"endif");
	if (result) parse_commands(if_block);
}

void GetSetScriptParser::parse_set(std::istream& script)
{
	auto line=rest_of_line(script);
	std::string key,value;
	if (!get_token_key(line,key))
		parse_error("parse_set","Failed to parse key name.");
	if (!get_token_value(line,value))
		parse_error("parse_set","Failed to parse value.");
	expect_end_of_line(line,"parse_set");
	if (!parse_error_occured)
		GetSet<>(key,subject)=value;
}

void GetSetScriptParser::parse_trigger(std::istream& script)
{
	auto line=rest_of_line(script);
	std::string key;
	if (!get_token_key(line,key))
		parse_error("parse_trigger","Failed to parse key name.");
	expect_end_of_line(line,"parse_trigger");
	GetSet<>(key,subject)=GetSet<>(key,subject).getValue();
}

void GetSetScriptParser::parse_while(std::istream& script)
{
	auto line=rest_of_line(script);
	std::string varname;
	if (!get_token_string(line,varname))
		parse_error("parse_while","Failed to parse variable name.");
	expect_end_of_line(line,"parse_while");
	std::string while_block=get_block(script,"endwhile");
	while (stringTo<bool>(variables[varname]) && !parse_error_occured) parse_commands(while_block);
}

void GetSetScriptParser::parse_foreach(std::istream& script)
{
	auto line=rest_of_line(script);
	std::string varname;
	if (!get_token_string(line,varname))
		parse_error("parse_foreach","Failed to parse variable name.");
	std::string in;
	line >> in;
	if (in!="in")
	{
		parse_error("parse_foreach", std::string("Expected \"in\" but found " + in +"."));
		return;
	}
	std::string semicolon_seperated_values;
	if (!get_token_value(line,semicolon_seperated_values))
	{
		parse_error("parse_foreach","Failed to parse variable name.");
		return;
	}
	expect_end_of_line(line,"parse_foreach");
	std::vector<std::string> vector=stringToVector<std::string>(semicolon_seperated_values,';',true);
	std::string foreach_block=get_block(script,"endforeach");
	for (auto it=vector.begin();it!=vector.end();++it)
	{
		if (parse_error_occured) break;
		variables[varname]=*it;
		parse_commands(foreach_block);
	}
}

void GetSetScriptParser::parse_file(std::istream& script)
{
	auto line=rest_of_line(script);
	std::string action,type,file;
	line >> action;
	if (action=="load")
	{
		line >> type;
		if (!get_token_string(line,file))
			parse_error("parse_file","Failed to parse ini file name.");
		expect_end_of_line(line,"parse_file");
		if (type == "ini")
			GetSetIO::load<GetSetIO::IniFile>(file);
		else parse_error("parse_file",std::string("Unknown file type ")+action);
	}
	else if (action=="save")
	{
		line >> type;
		if (!get_token_string(line,file))
			parse_error("parse_file","Failed to parse ini file name.");
		expect_end_of_line(line,"parse_file");
		if (type == "ini")
			GetSetIO::save<GetSetIO::IniFile>(file);
		else parse_error("parse_file",std::string("Unknown file type ")+action);
	}
	else if (action=="run")
	{
		if (!get_token_string(line,file))
			parse_error("parse_file","Failed to parse script file name.");
		expect_end_of_line(line,"parse_file");
		std::string commands=fileReadString(file);
		if (commands.empty())
			parse_error("parse_file","File not found or file empty.");
		else
			parse_commands(commands);
	}
	else parse_error("parse_file",std::string("Unknown action ")+action);
}

void GetSetScriptParser::parse_input(std::istream& script)
{
	auto line=rest_of_line(script);
	std::string varname;
	if (!get_token_string(line,varname))
	{
		parse_error("parse_input","Failed to parse variable name.");
		return;
	}
	expect_end_of_line(line,"parse_input");
	if (user_input) variables[varname]=(*user_input)();
	else std::cin >> variables[varname];
}

void GetSetScriptParser::parse_output(std::istream& script)
{
	auto line=rest_of_line(script);
	std::string value;
	if (!get_token_value(line,value))
	{
		parse_error("parse_output","Failed to parse value.");
		return;
	}
	expect_end_of_line(line,"parse_output");
	if (user_output) (*user_output)(value);
	else std::cout << value << std::endl;
}


std::stringstream GetSetScriptParser::rest_of_line(std::istream& script)
{
	std::string line;
	getline(script, line, '\n');
	return std::stringstream(line);
}

void GetSetScriptParser::expect_end_of_line(std::istream& script, const std::string& fn_name)
{
	script >> std::ws;
	if (!script.eof())
		parse_error(fn_name,"Expected end of line.");
}

std::string GetSetScriptParser::get_block(std::istream& script, const std::string& end_block)
{
	std::string block;
	bool block_found=false;
	while (script && !script.eof())
	{
		std::string line,command;
		script >> std::ws;
		getline(script, line, '\n');
		std::istringstream line_str(line);
		line_str >> command;
		if (command.length()>=end_block.size() && command.substr(0,end_block.size())==end_block )
		{
			expect_end_of_line(line_str,std::string("After ")+end_block);
			block_found=true;
			break;
		}
		block+=line+"\n";
	}
	if (!block_found)
	{
		parse_error(end_block,"Unexpected end of file.");
		return "";
	}
	return block;
}

#endif // __GetSetScripting_hxx
