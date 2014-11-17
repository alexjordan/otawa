/*
 * $Id$
 * Copyright (c) 2006, IRIT-UPS.
 *
 * test/test_formatter.cpp -- unit tests for Formatter class.
 */

#include <elm/util/test.h>
#include <elm/util/Formatter.h>
#include <elm/io/BlockInStream.h>

using namespace elm;
using namespace elm::util;
using namespace elm::io;

class MyFormatter: public Formatter {
	int index;
protected:
	virtual int process(io::OutStream& out, char chr) {
		io::Output fout(out);
		switch(chr) {
		case 'd':
			fout << index;
			return DONE;
		case 'a':
		case 'A':
			fout << (char)(chr + index);
			return DONE;
		default:
			return REJECT;
		}
	}
public:
	MyFormatter(int idx): index(idx) {
	}
};

TEST_BEGIN(formatter)
	
	MyFormatter format(3);
	BlockInStream temp("r%d = %%%a");
	BlockOutStream buf;
	format.format(temp, buf);
	String result = buf.toString();
	CString res = buf.toCString();
	CHECK_EQUAL(res, CString("r3 = %d"));
	delete [] res.chars();
	
TEST_END
