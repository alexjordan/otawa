/*
 * $Id$
 * Copyright (c) 2003-07 IRIT-UPS <casse@irit.fr>
 *
 * Unit tests
 */

#include <elm/io.h>
#include <elm/util/test.h>

using namespace elm;

// Entry point
int main(int argc, char *argv[]) {

	// process the help
	for(int i = 1; i < argc; i++)
		if(string("-h") == argv[i] || string("--help") == argv[i]) {
		cerr << "Modules:\n";
		for(TestSet::Iterator test(TestSet::def); test; test++)
			cerr << "\t" << test->name() << io::endl;
		return 0;
	}
	
	// process the tests
	bool one = false;
	for(int i = 1; i < argc; i++) {
		bool found = false;
		one = true;

		// look in the structure
		for(TestSet::Iterator test(TestSet::def); test; test++)
			if(test->name() == argv[i]) {
				found = true;
				test->perform();
			}

		// not found: error
		if(!found)
			cerr << "ERROR: no test called \"" << argv[i] << "\"\n";
	}

	// if none selected, test all
	if(!one) {
		for(TestSet::Iterator test(TestSet::def); test; test++) {
			test->perform();
			if(test->hasFailed()) {
				cerr << "ERROR: " << test->name() << " failed!\n";
				return 1;
			}
		}
	}

	// success
	return 0;
}
