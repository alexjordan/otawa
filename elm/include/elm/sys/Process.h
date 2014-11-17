/*
 *	Process class interface
 *
 *	This file is part of OTAWA
 *	Copyright (c) 2006-12, IRIT UPS.
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
#ifndef ELM_SYS_PROCESS_H
#define ELM_SYS_PROCESS_H


namespace elm { namespace sys {

// Process class
class Process {
	friend class ProcessBuilder;
#if defined(__unix) || defined(__APPLE__)
	int pid, rcode;
	Process(int _pid);
#elif defined(__WIN32) || defined(__WIN64)
	void* pi;
	unsigned long  rcode;
	Process(void const* _pi);
#else
#	error "System not supported"
#endif

public:
	bool isAlive(void);
	int returnCode(void);
	void kill(void);
	void wait(void);
};

} } // elm::system

#endif // ELM_SYS_PROCESS_H
