/*
 *	$Id$
 *	Manager class interface
 *
 *	This file is part of OTAWA
 *	Copyright (c) 2004-07, IRIT UPS.
 * 
 *	OTAWA is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	OTAWA is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with OTAWA; if not, write to the Free Software 
 *	Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */
#ifndef ELM_OPTION_MANAGER_H
#define ELM_OPTION_MANAGER_H

#include <elm/genstruct/Vector.h>
#include <elm/option/Option.h>
#include <elm/util/Version.h>
#include <elm/util/MessageException.h>
#include <elm/genstruct/SortedBinMap.h>
#include <elm/util/VarArg.h>

namespace elm { namespace option {

// OptionException exception
class OptionException: public MessageException {
public:
	OptionException(const String& message);
};

// configuration tags
const int end			= 0,
		  program		= 1,	// const char *
		  version		= 2,	// Version *
		  author		= 3,	// const char *
		  copyright		= 4,	// const char *
		  description	= 5,	// const char *
		  help			= 5,	// alias for description
		  free_arg		= 6,	// const char *
		  cmd			= 7,	// const char *
		  short_cmd		= 8,	// char
		  long_cmd		= 9,	// const char *
		  def			= 10,	// option dependent
		  require		= 12,	// none
		  optional		= 13,	// none
		  arg_desc		= 14;	// const char *


// Manager
class Manager {
	friend class Option;
public:

	class Make {
		friend class Manager;
	public:
		inline Make(void) { }
		inline Make(cstring program, Version version = Version::ZERO): _program(program), _version(version) { }
		inline Make& author(cstring s) { _author = s; return *this; }
		inline Make& copyright(cstring s) { _copyright = s; return *this; }
		inline Make& description(cstring s) { _description = s; return *this; }
		inline Make& free_argument(cstring s) { _free_argument_description = s; return *this; }
	private:
		cstring _program;
		Version _version;
		cstring _author;
		cstring _copyright;
		cstring _description;
		cstring _free_argument_description;
	};

	typedef const char * const *argv_t;
	inline Manager(void) { }
	Manager(int tag, ...);
	Manager(const Make& maker);
	virtual ~Manager(void) { }
	void addOption(Option *option) throw(OptionException);
	void removeOption(Option *option);
	void parse(int argc, argv_t argv) throw(OptionException);
	void displayHelp(void);

	// accessors
	inline cstring getProgram(void) const { return info._program; }
	inline const Version& getVersion(void) const { return info._version; }
	inline cstring getAuthor(void) const { return info._author; }
	inline cstring getCopyright(void) const { return info._copyright; }
	inline cstring getDescription(void) const { return info._description; }
	inline cstring getFreeArgumentDescription(void) const { return info._free_argument_description; }

protected:
	Make info;
	/*CString program;
	Version version;
	CString author;
	CString copyright;
	CString description;
	CString free_argument_description;*/
	virtual void process(String arg);
	virtual void configure(int tag, VarArg& args);

private:
	genstruct::Vector<Option *> options;
	void processOption(Option *option, int& i, int argc, argv_t argv, const char *earg);
	void addShort(char cmd, Option *option) throw(OptionException);
	void addLong(cstring cmd, Option *option) throw(OptionException);
	void addCommand(string cmd, Option *option) throw(OptionException);
	genstruct::SortedBinMap<char, Option *> shorts;
	genstruct::SortedBinMap<string, Option *> cmds;
};

} } //elm::option

#endif // ELM_OPTION_MANAGER_H
