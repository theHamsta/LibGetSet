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

#include <sstream>
#include <map>

#include "GetSetInternal.h"

/// Parse a script line-by-line. Language is not context free. See ScriptSyntax.txt for more info.
class GetSetScriptParser : public GetSetInternal::Dictionary::Observer
{
public:
	GetSetScriptParser(GetSetInternal::Dictionary& _subject=GetSetInternal::Dictionary::global());

	/// Global instance
	static GetSetScriptParser& global();

	/// Check to see whether errors have occured
	bool good() const;

	/// User prompt
	void prompt();

	/// Parse the scrip provided by text
	bool parse(const std::string& commands, const std::string& scriptname="script");

	/// Force stop execution
	void force_stop();

	/// Add (or remove) a callback function for script output.
	void addOutputCallback(void *identifier,  void (*callback)(const std::string& text, void* identifier)=0x0);

	/// Add (or remove) a callback function for parse errors and other information
	void addErrorCallback(void *identifier,  void (*callback)(const std::string& text, void* identifier)=0x0);

	/// Get help for script language
	static std::string synopsis(const std::string& command="", bool with_example=false);

	/// Creates a script which stores all current variables
	std::string state();

	/// Access everything from current location to line starting with end_block
	std::string get_block(std::istream& script, const std::string& end_block);

	/// Figure out current location within a stringstream
	static std::string location(std::istream& script);

	/// Calls registered event handlers. Recursion is not allowed.
	void notify(const GetSetInternal::Node& node, GetSetInternal::Dictionary::Signal signal);

	/// Registered script functions called on signals
	std::map<std::string, std::string> event_handlers;

	/// Callback for user input
	std::string (*user_input)();

	/// Map of temporary variables and command functions (i.e. subroutines)
	std::map<std::string, std::string> variables;

protected:

	/// Callback for output (optional)
	std::map<void*, void (*)(const std::string&, void*)> user_out;

	/// Callback for output (optional)
	std::map<void*, void (*)(const std::string&, void*)> user_err;

	/// Get user input
	std::string input();

	/// Write user output
	void printOut(const std::string& text);

	/// Write user output
	void printErr(const std::string& text);

	/// Only this dictionary will be affectd by this parser
	GetSetInternal::Dictionary& subject;


	/// A section used as prefix for all keys ("with" command)
	std::string section_prefix;

	/// Set to true on syntax error
	bool parse_error_occured;
	
	/// Parse the scrip provided by text
	void parse_commands(const std::string& script, const std::string& file_or_function_name);

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
	// Commands: call concat define discard print eval exit file for if input set while who with
	//

	/// help [<command>]
	void parse_help(std::istream& script);
	/// call function <varname:function>
	void parse_call(std::istream& script);
	/// on [change|create|destroy] key <key> call function <varname:function>>
	void parse_on(std::istream& script);
	/// concat var <varname> from <value> [and <value>]+
	void parse_concat(std::istream& script);
	/// define function <varname> ... enddefine
	void parse_define(std::istream& script);
	/// discard [key <key>|var <varname>|function <varname:function>]
	void parse_discard(std::istream& script);
	/// print output <value> [and <value>]*
	/// print file <value> [append|replace] <value> [and <value>]*
	void parse_print(std::istream& script);
	/// eval var <varname> from <value> <op> <value> (avoid double calculations: GetSet works with strings internally)
	void parse_eval(std::istream& script);
	/// exit [<string>], where string is converted to int and used as exit code.
	void parse_exit(std::istream& script);
	/// file {run|save ini|load ini} <file>, where file is a string
	void parse_file(std::istream& script);
	/// EITHER for each <varname> in <';' delimited strings> ... endfor
	/// OR     for each <varname> from <numeric value> to <numeric value> step <numeric value> ... endfor
	void parse_for(std::istream& script);
	/// if [not] {strequal|numequal|gequal|lequal|greater|less} <value> <value> ... endif
	void parse_if(std::istream& script);
	/// input
	void parse_input(std::istream& script);
	/// set [key <key>|var <varname>] <value>
	void parse_set(std::istream& script);
	/// while <boolean value> ... endwhile
	void parse_while(std::istream& script);
	/// who
	void parse_who(std::istream& script);
	/// with <section> ... endwith, where section has same format as <key>
	void parse_with(std::istream& script);


	/// Make sure end of line or end of file is reached
	bool expect_end_of_line(std::istream& script, const std::string& fn_name);

	/// Make sure we encounter a certain set of keywords (semicolon separated)
	int expect_keyword(std::istream& script, const std::string& fn_name, const std::string& keywords);


};

#endif // __GetSetScripting_h
