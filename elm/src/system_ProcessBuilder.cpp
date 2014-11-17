/*
 *	$Id$
 *	ProcessBuilder class implementation
 *
 *	This file is part of OTAWA
 *	Copyright (c) 2006-07, IRIT UPS.
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

#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <elm/sys/ProcessBuilder.h>
#include <elm/sys/SystemException.h>
#if defined(__WIN32) || defined(WIN64)
#include <windows.h>
#endif

extern char **environ;

namespace elm { namespace sys {

/**
 * @class ProcessBuilder
 * This class is used to build a new process by launching a command line.
 * @ingroup system_inter
 */


/**
 * Construct a process builder.
 * @param command	Command to use.
 */
ProcessBuilder::ProcessBuilder(CString command)
:	in(&io::in),
 	out(&io::out),
 	err(&io::err)
{
	args.add(command);
}


/**
 * Add an argument to the command line.
 * @param argument	Argument to add.
 */	
void ProcessBuilder::addArgument(CString argument) {
	args.add(argument);
}


/**
 * Set the input stream of the built process.
 * @param _in	New process input.
 */
void ProcessBuilder::setInput(SystemInStream *_in) {
	in = _in;
}


/**
 * Set the output stream of the built process.
 * @param _out	New process output.
 */
void ProcessBuilder::setOutput(SystemOutStream *_out) {
	out = _out;
}


/**
 * Set the error stream of the built process.
 * @param _out	New process error.
 */
void ProcessBuilder::setError(SystemOutStream *_err) {
	err = _err;
}


/**
 * Run the built process.
 * @return	The built process.
 * @throws SystemException	Thrown if there is an error during the build.
 */
Process *ProcessBuilder::run(void) {
#if defined(__unix) || defined(__APPLE__)
	int error = 0;
	int old_in = -1, old_out = -1, old_err = -1;

	Process *process = 0;

	// Prepare the streams
	if(in->fd() != io::in.fd()) {
		old_in = dup(0);
		if(old_in < 0)
			error = errno;
		else
			dup2(in->fd(), 0);
	}
	if(!errno && out->fd() != io::out.fd()) {
		old_out = dup(1);
		if(old_out < 0)
			error = errno;
		else
			dup2(out->fd(), 1);
	}
	if(!errno && err->fd() != io::err.fd()) {
		old_err = dup(2);
		if(old_err < 0)
			error = errno;
		else
			dup2(err->fd(), 2);
	}

	// Create the process
	if(!error) {
		int pid = fork();
		// error
		if(pid < 0)
			error = errno;
		// father
		else if(pid != 0)
			process = new Process(pid);
		// son
		else {

			// Build arguments
			char *tab[args.count() + 1];
			for(int i = 0; i < args.count(); i++)
				tab[i] = (char *)&args[i];
			tab[args.count()] = 0;

			// Launch the command
			execvp(tab[0], tab);
			exit(1);
		}
	}

	// Reset the streams
	if(old_in >= 0) {
		close(0);
		dup2(old_in, 0);
		close(old_in);
	}
	if(old_out >= 0) {
		close(1);
		dup2(old_out, 1);
		close(old_out);
	}
	if(old_err >= 0) {
		close(2);
		dup2(old_err, 2);
		close(old_err);
	}

#elif defined(__WIN32) || defined(__WIN64)
	// no  need to redirect output, if bInheritHandles is set to false when creating process
	// it uses standard input, output and error output

	int error = 0;
	Process *process;
	PROCESS_INFORMATION *pi = new PROCESS_INFORMATION;
	STARTUPINFO si = {sizeof(si)};

	if (!error) {
		// Build arguments
		StringBuffer tab;
		for(int i = 0; i < args.count() ; i++)
		{
					tab << args[i];
					tab << " ";
		}
		char tabtemp[tab.length() +1];
		strcpy(tabtemp,tab.toString().chars());
		cout << tabtemp << " error1 = "<< GetLastError() << io::endl;
		// Launch process
		if(CreateProcess(NULL,tabtemp,0,0,TRUE,0,0,0,&si,pi) == 0)
			error = GetLastError();
		cout << "error2 = " << GetLastError() << "error value = " << error << io::endl;
		process = new Process(pi);
		cout << "erro3r = " << GetLastError() << io::endl;

	}

	// Return the result
	if(error){
		cout << "error detected " << GetLastError() << io::endl;
		throw new SystemException(error, "process building");
	}
	else
		return process;


#else
#error "System Unsupported"
#endif


// Return the result
if(error)
	throw new SystemException(error, "process building");
else
	return process;
}

} } // elm::sys
