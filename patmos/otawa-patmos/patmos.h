/*
 *	Patmos loader header file
 */
#ifndef OTAWA_PATMOS_H
#define OTAWA_PATMOS_H

#include <otawa/proc/Feature.h>

namespace otawa { namespace patmos {

using namespace elm;
using namespace otawa;

class Info {
public:
	Info(Process& _proc);
	int bundleSize(const Address& addr);
private:
	Process& proc;
};

extern Identifier<Info *> INFO;
extern Feature<NoProcessor> INFO_FEATURE;

} } // otawa::patmos

#endif // OTAWA_PATMOS_H
