/*
 * $Id$
 * Copyright (c) 2005, IRIT-UPS.
 *
 * test/test_lock.cpp -- unit tests for elm::Lock class.
 */

#include <elm/util/test.h>
#include <elm/sys/ProcessBuilder.h>
#include <elm/sys/System.h>
#include <elm/io.h>
#include <elm/io/OutFileStream.h>

#ifdef __unix
#elif defined(__WIN32) || defined(__WIN64)
#	include <windows.h>
#else
#	error "Unsupported OS"
#endif

using namespace elm;
using namespace elm::sys;

// test_process()
TEST_BEGIN(process)

#if defined (__unix)
	ProcessBuilder builder("ls");
#elif defined(__WIN32) || defined(__WIN64)
	ProcessBuilder builder("C:\\Win16App\\MinGW\\bin\\gcc.exe");
#endif
	Pair<PipeInStream *, PipeOutStream *> pipes = System::pipe();
	builder.setOutput(pipes.snd);
	io::OutFileStream err_file("error.txt");
	builder.setError(&err_file);
	Process *process = builder.run();
	delete pipes.snd;
	CHECK(process);
	io::Input input(*pipes.fst);
	String line;
	input >> line;
	while(line) {
		cout << "> " << line;
		input >> line;
	}
	process->wait();
	CHECK_EQUAL(process->returnCode(), 0);
	delete process;
	delete pipes.fst;
	
TEST_END

