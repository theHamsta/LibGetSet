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

#ifndef __GetSetScripting_h
#define __GetSetScripting_h

#include "GetSetDictionary.h"

#include <sstream>

/// Store the current state and record all changes with the possibility of playing them back.
// class GetSetScriptRecorder TODO
/// This is where a detailed log of all events will be stored
//	std::ofstream file;

/// Parse a script line-by-line. Language is not context free. See ScriptSyntax.txt for more info.
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

	/// Get help for script language
	std::string synopsis(const std::string& command="", bool with_example=false);

	/// Check to see whether errors have occured
	bool good() const
	{
		return !parse_error_occured;
	}

protected:
	/// Only this dictionary will be affectd by this parser
	GetSetDictionary& subject;
	
	/// Map of temporary variables and command functions (i.e. subroutines)
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
	/// Token: <string>
	bool expect_token_string(std::istream& script, const std::string& fn_name, std::string& token);
	/// Token: <key>
	bool expect_token_key(std::istream& script, const std::string& fn_name, std::string& token);
	/// Token: <value> or <numeric value> if numeric is set
	bool expect_token_value(std::istream& script, const std::string& fn_name, std::string& token, bool numeric=false);
	/// Token: <numeric value>
	bool expect_token_value(std::istream& script, const std::string& fn_name, double& token);

	//
	// Commands: set function call with if while for file input echo
	//

	/// help [<command>]
	void parse_help(std::istream& script);
	/// set [key <key>|var <varname>] <value>
	void parse_set(std::istream& script);
	/// function <varname> ... endfunction
	void parse_function(std::istream& script);
	/// call <varname>
	void parse_call(std::istream& script);
	/// with <section> ... endwith, where section has same format as <key>
	void parse_with(std::istream& script);
	/// if [not] {strequal|numequal|gequal|lequal|greater|less} <value> <value> ... endif
	void parse_if(std::istream& script);
	/// while <boolean value> ... endwhile
	void parse_while(std::istream& script);
	/// EITHER for each <varname> in <';' delimited strings> ... endfor
	/// OR     for each <varname> from <numeric value> to <numeric value> step <numeric value> ... endfor
	void parse_for(std::istream& script);
	/// file {run|save ini|load ini} <file>, where file is a string
	void parse_file(std::istream& script);
	/// input
	void parse_input(std::istream& script);
	/// echo
	void parse_echo(std::istream& script);
	/// eval (avoid double calculations: GetSet works with strings internally)
	void parse_eval(std::istream& script);
	
	/// Advance to the next line
	std::stringstream rest_of_line(std::istream& script);

	/// Make sure end of line or end of file is reached
	bool expect_end_of_line(std::istream& script, const std::string& fn_name);

	/// Make sure we encounter a certain set of keywords (semicolon separated)
	int expect_keyword(std::istream& script, const std::string& fn_name, const std::string& keywords);

	/// Access everything from current location to line starting with end_block
	std::string get_block(std::istream& script, const std::string& end_block);
};

#endif // __GetSetScripting_h
