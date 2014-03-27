#include <elm/assert.h>
#include <otawa/prog/Manager.h>
#include <otawa/prog/Loader.h>
#include <otawa/platform.h>
#include <otawa/hard/Register.h>
#include <gel/gel.h>
#include <gel/gel_elf.h>
#include <gel/image.h>
#include <gel/debug_line.h>
#include <otawa/proc/Processor.h>
#include <otawa/util/FlowFactLoader.h>
#include <elm/genstruct/SortedSLList.h>
#include <otawa/sim/features.h>
#include <otawa/prop/Identifier.h>
#include "patmos.h"


extern "C"
{
	// gliss2 C include files
	#include <patmos/api.h>
	#include <patmos/id.h>
	#include <patmos/macros.h>

	// generated code
	#include <patmos/used_regs.h>

	#include "config.h"
}

#include "otawa_delayed.h"

/* instruction kind */
#include "otawa_kind.h"

/* target computation */
#include "otawa_target.h"


using namespace otawa::hard;


#define TRACE(m) //cerr << m << io::endl
#define LTRACE	 //cerr << "POINT " << __FILE__ << ":" << __LINE__ << io::endl
#define RTRACE(m)	//m
//#define SCAN_ARGS

// Trace for switch parsing
#define STRACE(m)	//cerr << m << io::endl


namespace otawa { namespace patmos {

/**
 * Register banks.
 */
static hard::PlainBank regR("D", hard::Register::INT,  32, "$r%d", 32);
static hard::PlainBank regS("S", hard::Register::INT,  32, "$s%d", 16);
static hard::Register  regMCB("MCB", hard::Register::ADDR, 32);
static hard::Register  regPC ("PC",  hard::Register::ADDR, 32);
static hard::Register  regNPC("nPC", hard::Register::ADDR, 32);
static hard::MeltedBank misc("misc", &regMCB, &regPC,&regNPC, 0);

static const hard::RegBank *banks[] = {
        &regR,
        &regS,
        &misc
};

static const elm::genstruct::Table<const hard::RegBank *> banks_table(banks, 3);


// register decoding
class RegisterDecoder {
public:
    RegisterDecoder(void) {

        // clear the map
        for(int i = 0; i < PATMOS_REG_COUNT; i++)
            map[i] = 0;

        // initialize it
        for(int i = 0; i < 32; i++) {
            map[PATMOS_REG_R(i)] = regR[i];
	}
        for(int i = 0; i < 16; i++) {
            //map[PATMOS_REG_S(i)] = regS[i];
            map[PATMOS_REG_S(i)] = 0;
        }
        map[PATMOS_REG_MCB] = &regMCB;
        //map[PATMOS_REG_PC]  = &regPC;
        //map[PATMOS_REG_NPC] = &regNPC;
    }

    inline hard::Register *operator[](int i) const { return map[i]; }

private:
    hard::Register *map[PATMOS_REG_COUNT];
};
static RegisterDecoder register_decoder; 


// Platform class
class Platform: public hard::Platform {
public:
	static const Identification ID;

	/**
	 * Build a new gliss platform with the given configuration.
	 * @param props		Configuration properties.
	 */
        Platform(const PropList& props = PropList::EMPTY): hard::Platform(ID, props)
                { setBanks(banks_table); }
	/**
	 * Build a new platform by cloning.
	 * @param platform	Platform to clone.
	 * @param props		Configuration properties.
	 */
        Platform(const Platform& platform, const PropList& props = PropList::EMPTY)
                : hard::Platform(platform, props)
                { setBanks(banks_table); }

        // otawa::Platform overload
        virtual bool accept(const Identification& id)
                { return id.abi() == "elf" && id.architecture() == "patmos"; }
};

/**
 * Identification of the default platform.
 */
const Platform::Identification Platform::ID("patmos-elf-");



// SimState class
/*
class SimState: public otawa::SimState
{
public:
	SimState(Process *process, patmos_state_t *state, patmos_decoder_t *decoder, bool _free = false)
	: otawa::SimState(process), _patmosState(state), _patmosDecoder(decoder) {
		ASSERT(process);
		ASSERT(state);
	}

	virtual ~SimState(void) {
		patmos_delete_state(_patmosState);
	}

	// virtual void setSP(const Address& addr) { LEON_SYSPARM_REG32_SP(_patmosState) = addr.offset(); }

	inline patmos_state_t *patmosState(void) const { return _patmosState; }

	virtual Inst *execute(Inst *oinst) {
		ASSERTP(oinst, "null instruction pointer");

		Address addr = oinst->address();
		patmos_inst_t *inst;
		_patmosState->nPC = addr.address();
		inst = patmos_decode(_patmosDecoder, _patmosState->nPC);
		patmos_execute(_patmosState, inst);
		patmos_free_inst(inst);
		if (_patmosState->nPC == oinst->topAddress()) {
			Inst *next = oinst->nextInst();
			while (next && next->isPseudo())
				next = next->nextInst();
			if(next && next->address() == Address(_patmosState->nPC))
				return next;
		}
		Inst *next = process()->findInstAt(_patmosState->nPC);
		ASSERTP(next, "cannot find instruction at " << Address(_patmosState->nPC) << " from " << oinst->address());
		return next;
	}

private:
	patmos_state_t *_patmosState;
	patmos_decoder_t *_patmosDecoder;
};
*/

/**
 * This class provides support to build a loader plug-in based on the GLISS V2
 * with ELF file loading based on the GEL library. Currently, this only includes
 * the PPC ISA.
 *
 * This class allows to load a binary file, extract the instructions and the
 * symbols (labels and function). You have to provide a consistent
 * platform description for the processor.
 *
 * The details of the interface with V2 GLISS are managed by this class and you
 * have only to write :
 *   - the platform description,
 *   - the recognition of the instruction,
 *	 - the assignment of the memory pointer.
 */
class Process: public otawa::Process
{
public:
	Process(Manager *manager, hard::Platform *pf, const PropList& props = PropList::EMPTY);

	~Process();

	virtual otawa::SimState *newState(void) {
		//patmos_state_t *s = patmos_new_state(_patmosPlatform);
		//ASSERTP(s, "otawa::patmos::Process::newState(), cannot create a new patmos_state");
		//return new SimState(this, s, _patmosDecoder, true);
		return 0;
	}

	virtual int instSize(void) const { return 4; }
	void decodeRegs( Inst *inst, elm::genstruct::AllocatedTable<hard::Register *> *in, elm::genstruct::AllocatedTable<hard::Register *> *out);
	inline patmos_decoder_t *patmosDecoder() { return _patmosDecoder;}
	inline void *patmosPlatform(void) const { return _patmosPlatform; }
	void setup(void);
	void getSem(otawa::Inst *inst, sem::Block& block);

	// Process Overloads
	virtual hard::Platform *platform(void);
	virtual otawa::Inst *start(void);
	virtual File *loadFile(elm::CString path);
	virtual void get(Address at, signed char& val);
	virtual void get(Address at, unsigned char& val);
	virtual void get(Address at, signed short& val);
	virtual void get(Address at, unsigned short& val);
	virtual void get(Address at, signed long& val);
	virtual void get(Address at, unsigned long& val);
	virtual void get(Address at, signed long long& val);
	virtual void get(Address at, unsigned long long& val);
	virtual void get(Address at, Address& val);
	virtual void get(Address at, string& str);
	virtual void get(Address at, char *buf, int size);
	virtual Option<Pair<cstring, int> > getSourceLine(Address addr)
		throw (UnsupportedFeatureException);
	virtual void getAddresses(cstring file, int line, Vector<Pair<Address, Address> >& addresses)
		throw (UnsupportedFeatureException);

protected:
	friend class Segment;
	virtual otawa::Inst *decode(Address addr);
	virtual gel_file_t *gelFile(void) { return _gelFile; }
	virtual patmos_memory_t *patmosMemory(void) { return _patmosMemory; }

private:
	otawa::Inst *_start;
	hard::Platform *_platform;
	patmos_platform_t *_patmosPlatform;
	patmos_memory_t *_patmosMemory;
	patmos_decoder_t *_patmosDecoder;
	int argc;
	char **argv, **envp;
	bool no_stack;
	bool init;
	struct gel_line_map_t *map;
	struct gel_file_info_t *file;
	gel_file_t *_gelFile;
};

// Process display
elm::io::Output& operator<<(elm::io::Output& out, Process *proc)
	{ out << "Process(" << (void *)proc << ")"; }


// Inst class
class Inst: public otawa::Inst {
public:

	inline Inst(Process& process, kind_t kind, Address addr)
		: proc(process), _kind(kind), _addr(addr), isRegsDone(false) { }

	/**
	 */
	void dump(io::Output& out) {
		char out_buffer[200];
		patmos_inst_t *inst = patmos_decode(proc.patmosDecoder(), _addr);
		patmos_disasm(out_buffer, inst);
		patmos_free_inst(inst);
		out << out_buffer;
	}

	virtual kind_t kind() { return _kind; }
	virtual address_t address() const { return _addr; }
	virtual ot::size size() const { return 4; }
	virtual Process &process() { return proc; }

	virtual const elm::genstruct::Table<hard::Register *>& readRegs() {
/*		if ( ! isRegsDone)
		{
			decodeRegs();
			isRegsDone = true;
		}
*/
		return in_regs;
	}

	virtual const elm::genstruct::Table<hard::Register *>& writtenRegs() {
/*
		if ( ! isRegsDone)
		{
			decodeRegs();
			isRegsDone = true;
		}
*/
		return out_regs;
	}

	virtual void semInsts (sem::Block &block) {
//		proc.getSem(this, block);
	}

protected:
	virtual void decodeRegs(void) {
		proc.decodeRegs(this, &in_regs, &out_regs);
	}

	kind_t _kind;
	elm::genstruct::AllocatedTable<hard::Register *> in_regs;
	elm::genstruct::AllocatedTable<hard::Register *> out_regs;
	Process &proc;

private:
	patmos_address_t _addr;
	bool isRegsDone;
};


// BranchInst class
class BranchInst: public Inst {
public:

	inline BranchInst(Process& process, kind_t kind, Address addr)
	: Inst(process, kind, addr), _target(0), isTargetDone(false) {
		patmos_inst_t *inst;
		inst = patmos_decode(proc.patmosDecoder(), (patmos_address_t)address());
		otawa::delayed_t delay = patmos_delayed(inst);
		if(delay != otawa::DELAYED_None)
			otawa::DELAYED(this) = delay;
	}

	virtual ot::size size() const { return 4; }

	virtual otawa::Inst *target() {
		if (!isTargetDone) {
			patmos_address_t a = decodeTargetAddress();
			if (a)
				_target = process().findInstAt(a);
			isTargetDone = true;
		}
		return _target;
	}

protected:
	virtual patmos_address_t decodeTargetAddress(void);

private:
	otawa::Inst *_target;
	bool isTargetDone;
};


// Segment class
class Segment: public otawa::Segment {
public:
	Segment(Process& process,
		CString name,
		address_t address,
		size_t size)
	: otawa::Segment(name, address, size, EXECUTABLE), proc(process) { }

protected:
	virtual otawa::Inst *decode(address_t address)
		{ return proc.decode(address); }

private:
	Process& proc;
};




 /**
  * Build a process for the new GLISS V2 system.
  * @param manager	Current manager.
  * @param platform	Current platform.
  * @param props	Building properties.
  */
Process::Process(Manager *manager, hard::Platform *platform, const PropList& props)
:	otawa::Process(manager, props),
 	_start(0),
 	_platform(platform),
	_patmosMemory(0),
	init(false),
	map(0),
	file(0)
{
	ASSERTP(manager, "manager required");
	ASSERTP(platform, "platform required");

	// gliss2 patmos structs
	_patmosPlatform = patmos_new_platform();
	ASSERTP(_patmosPlatform, "otawa::patmos::Process::Process(..), cannot create a patmos_platform");
	_patmosDecoder = patmos_new_decoder(_patmosPlatform);
	ASSERTP(_patmosDecoder, "otawa::patmos::Process::Process(..), cannot create a patmos_decoder");
	_patmosMemory = patmos_get_memory(_patmosPlatform, PATMOS_MAIN_MEMORY);
	ASSERTP(_patmosMemory, "otawa::patmos::Process::Process(..), cannot get main patmos_memory");
	patmos_lock_platform(_patmosPlatform);

	// build arguments
	char no_name[1] = { 0 };
	static char *default_argv[] = { no_name, 0 };
	static char *default_envp[] = { 0 };
	argc = ARGC(props);
	if (argc < 0)
		argc = 1;
	argv = ARGV(props);
	if (!argv)
		argv = default_argv;
	envp = ENVP(props);
	if (!envp)
		envp = default_envp;

	// handle features
	INFO(this) = new Info(*this);
	provide(INFO_FEATURE);
	provide(MEMORY_ACCESS_FEATURE);
	provide(SOURCE_LINE_FEATURE);
	provide(CONTROL_DECODING_FEATURE);
	provide(REGISTER_USAGE_FEATURE);
	provide(MEMORY_ACCESSES);
	provide(DELAYED_FEATURE);
}


/**
 */
Process::~Process() {
	patmos_delete_decoder(_patmosDecoder);
	patmos_unlock_platform(_patmosPlatform);
	if(_gelFile)
		gel_close(_gelFile);
}



Option<Pair<cstring, int> > Process::getSourceLine(Address addr) throw (UnsupportedFeatureException) {
	setup();
	if (!map)
		return none;
	const char *file;
	int line;
	if (!map || gel_line_from_address(map, addr.offset(), &file, &line) < 0)
		return none;
	return some(pair(cstring(file), line));
}


void Process::getAddresses(cstring file, int line, Vector<Pair<Address, Address> >& addresses) throw (UnsupportedFeatureException) {
	setup();
	addresses.clear();
	if (!map)
		return;
	gel_line_iter_t iter;
	gel_location_t loc, ploc = { 0, 0, 0, 0 };
	for (loc = gel_first_line(&iter, map); loc.file; loc = gel_next_line(&iter))
	{
		cstring lfile = loc.file;
		//cerr << loc.file << ":" << loc.line << ", " << loc.low_addr << "-" << loc.high_addr << io::endl;
		if (file == loc.file || lfile.endsWith(file))
		{
			if (line == loc.line)
			{
				//cerr << "added (1) " << loc.file << ":" << loc.line << " -> " << loc.low_addr << io::endl;
				addresses.add(pair(Address(loc.low_addr), Address(loc.high_addr)));
			}
			else if(loc.file == ploc.file && line > ploc.line && line < loc.line)
			{
				//cerr << "added (2) " << ploc.file << ":" << ploc.line << " -> " << ploc.low_addr << io::endl;
				addresses.add(pair(Address(ploc.low_addr), Address(ploc.high_addr)));
			}
		}
		ploc = loc;
	}
}


/**
 * Setup the source line map.
 */
void Process::setup(void) {
	ASSERT(_gelFile);
	if(init)
		return;
	init = true;
	map = gel_new_line_map(_gelFile);
}

hard::Platform *Process::platform(void) {
	return _platform;
}

otawa::Inst *Process::start(void) {
	return _start;
}

File *Process::loadFile(elm::CString path) {
	LTRACE;

	// Check if there is not an already opened file !
	if(program())
		throw LoadException("loader cannot open multiple files !");

	File *file = new otawa::File(path);
	addFile(file);

	// initialize the environment
	ASSERTP(_patmosPlatform, "invalid patmos_platform !");
	patmos_env_t *env = patmos_get_sys_env(_patmosPlatform);
	ASSERT(env);
	env->argc = argc;
	env->argv = argv;
	env->envp = envp;

	// load the binary
	if(patmos_load_platform(_patmosPlatform, (char *)&path) == -1)
		throw LoadException(_ << "cannot load \"" << path << "\".");

	// get the initial state
	//SimState *state = dynamic_cast<SimState *>(newState());
	//patmos_state_t *patmosState = state->patmosState();
	//if (!patmosState)
	//	throw LoadException("invalid patmos_state !");

	// build segments
	_gelFile = gel_open(&path, 0, GEL_OPEN_NOPLUGINS);
	if(!_gelFile)
		throw LoadException(_ << "cannot load \"" << path << "\".");
	gel_file_info_t infos;
	gel_file_infos(_gelFile, &infos);
	for (int i = 0; i < infos.sectnum; i++) {
		gel_sect_info_t infos;
		gel_sect_t *sect = gel_getsectbyidx(_gelFile, i);
		assert(sect);
		gel_sect_infos(sect, &infos);
		if (infos.flags & SHF_EXECINSTR) {
			Segment *seg = new Segment(*this, infos.name, infos.vaddr, infos.size);
			file->addSegment(seg);
		}
	}

	// Initialize symbols
	LTRACE;
	gel_enum_t *iter = gel_enum_file_symbol(_gelFile);
	gel_enum_initpos(iter);
	for(char *name = (char *)gel_enum_next(iter); name; name = (char *)gel_enum_next(iter)) {
		ASSERT(name);
		address_t addr = 0;
		Symbol::kind_t kind;
		gel_sym_t *sym = gel_find_file_symbol(_gelFile, name);
		assert(sym);
		gel_sym_info_t infos;
		gel_sym_infos(sym, &infos);
		switch(ELF32_ST_TYPE(infos.info)) {
		case STT_FUNC:
			kind = Symbol::FUNCTION;
			addr = (address_t)infos.vaddr;
			TRACE("SYMBOL: function " << infos.name << " at " << addr);
			break;
		case STT_NOTYPE:
			kind = Symbol::LABEL;
			addr = (address_t)infos.vaddr;
			TRACE("SYMBOL: notype " << infos.name << " at " << addr);
			break;
		default:
			continue;
		}

		// Build the label if required
		if(addr) {
			String label(infos.name);
			Symbol *sym = new Symbol(*file, label, kind, addr);
			file->addSymbol(sym);
			TRACE("function " << label << " at " << addr);
		}
	}
	gel_enum_free(iter);

	// Last initializations
	LTRACE;
	_patmosMemory = patmosMemory();
	ASSERTP(_patmosMemory, "memory information mandatory");
	_start = findInstAt((address_t)infos.entry);
	return file;
}


// Memory read
#define GET(t, s) \
	void Process::get(Address at, t& val) { \
			val = patmos_mem_read##s(_patmosMemory, at.offset()); \
			/*cerr << "val = " << (void *)(int)val << " at " << at << io::endl;*/ \
	}
GET(signed char, 8);
GET(unsigned char, 8);
GET(signed short, 16);
GET(unsigned short, 16);
GET(signed long, 32);
GET(unsigned long, 32);
GET(signed long long, 64);
GET(unsigned long long, 64);
GET(Address, 32);


void Process::get(Address at, string& str) {
	Address base = at;
	while(!patmos_mem_read8(_patmosMemory, at.offset()))
		at = at + 1;
	int len = at - base;
	char buf[len];
	get(base, buf, len);
	str = String(buf, len);
}


void Process::get(Address at, char *buf, int size)
	{ patmos_mem_read(_patmosMemory, at.offset(), buf, size); }



void Process::decodeRegs(otawa::Inst *oinst, elm::genstruct::AllocatedTable<hard::Register *> *in,
	elm::genstruct::AllocatedTable<hard::Register *> *out)
{
	// Decode instruction
	patmos_inst_t *inst;
	inst = patmos_decode(_patmosDecoder, oinst->address().address());
	if(inst->ident == PATMOS_UNKNOWN)
	{
		patmos_free_inst(inst);
		return;
	}

	// get register infos
	patmos_used_regs_read_t rds;
	patmos_used_regs_write_t wrs;
	elm::genstruct::Vector<hard::Register *> reg_in;
	elm::genstruct::Vector<hard::Register *> reg_out;
        patmos_used_regs(inst, rds, wrs);
        for (int i = 0; rds[i] != -1; i++ ) {
            hard::Register *r = register_decoder[rds[i]];
            if(r)
                reg_in.add(r);
        }
        for (int i = 0; wrs[i] != -1; i++ ) {
            hard::Register *r = register_decoder[wrs[i]];
            if(r)
                reg_out.add(r);
        } 

	// store results
	int cpt_in = reg_in.length();
	in->allocate(cpt_in);
	for (int i = 0 ; i < cpt_in ; i++)
		in->set(i, reg_in.get(i));
	int cpt_out = reg_out.length();
	out->allocate(cpt_out);
	for (int i = 0 ; i < cpt_out ; i++)
		out->set(i, reg_out.get(i));

	// Free instruction
	patmos_free_inst(inst);
}


otawa::Inst *Process::decode(Address addr) {
	//cerr << "DECODING: " << addr << io::endl;

	// Decode the instruction
	patmos_inst_t *inst;
	TRACE("ADDR " << addr);
	inst = patmos_decode(_patmosDecoder, (patmos_address_t)addr.offset());

	// Build the instruction
	Inst::kind_t kind = 0;
	otawa::Inst *result = 0;

	// get the kind from the nmp otawa_kind attribute
	if(inst->ident == PATMOS_UNKNOWN)
		TRACE("UNKNOWN !!!\n" << result);
	else
		kind = patmos_kind(inst);

	bool is_branch = kind & Inst::IS_CONTROL;

	// build the object
	if (is_branch)
		result = new BranchInst(*this, kind, addr);
	else
		result = new Inst(*this, kind, addr);

	// cleanup
	ASSERT(result);
	patmos_free_inst(inst);
	return result;
}



patmos_address_t BranchInst::decodeTargetAddress(void) {

	// Decode the instruction
	patmos_inst_t *inst;
	TRACE("ADDR " << addr);
	inst = patmos_decode(proc.patmosDecoder(), (patmos_address_t)address());

	// retrieve the target addr from the nmp otawa_target attribute
	Address target_addr;
	patmos_address_t res = patmos_target(inst);
	if(res != 0)
		target_addr = res;

	// Return result
	patmos_free_inst(inst);
	return target_addr;
}


// otawa::loader::patmos::Loader class
class Loader: public otawa::Loader {
public:
	Loader(void);

	// otawa::Loader overload
	virtual CString getName(void) const;
	virtual otawa::Process *load(Manager *_man, CString path, const PropList& props);
	virtual otawa::Process *create(Manager *_man, const PropList& props);
};


// Alias table
static string table[] = {
	"elf_48875"
};
static elm::genstruct::Table<string> patmos_aliases(table, 1);


/**
 * Build a new loader.
 */
Loader::Loader(void)
: otawa::Loader("patmos", Version(2, 0, 0), OTAWA_LOADER_VERSION, patmos_aliases) {
}


/**
 * Get the name of the loader.
 * @return Loader name.
 */
CString Loader::getName(void) const
{
	return "patmos";
}


/**
 * Load a file with the current loader.
 * @param man		Caller manager.
 * @param path		Path to the file.
 * @param props	Properties.
 * @return	Created process or null if there is an error.
 */
otawa::Process *Loader::load(Manager *man, CString path, const PropList& props)
{
	otawa::Process *proc = create(man, props);
	if (!proc->loadProgram(path))
	{
		delete proc;
		return 0;
	}
	else
		return proc;
}


/**
 * Create an empty process.
 * @param man		Caller manager.
 * @param props	Properties.
 * @return		Created process.
 */
otawa::Process *Loader::create(Manager *man, const PropList& props)
{
	return new Process(man, new Platform(props), props);
}


/**
 * @class Info
 * Provide information specific to the PatMOS architecture.
 */
 
 
/**
 * Constructor.
 */
Info::Info(otawa::Process& _proc): proc(_proc) {
}

	
/**
 * Get the size in bytes of a bundle starting at the given address.
 * @param addr	Address to look to.
 * @return		4 for a 32-bits bundle, 8 bits for a 64-bits bundle.
 */
int Info::bundleSize(const Address& addr) {
	t::uint32 w;
	proc.get(addr, w);
	if(w & 0x80000000)
		return 8;
	else
		return 4;
}


/**
 * Provide access to @ref Info data structure.
 * 
 * @p Features
 * @li @ref INFO_FEATURE
 * 
 * @p Hooks
 * @li Process
 */
Identifier<Info *> INFO("otawa::patmos::INFO", 0);


/**
 * Feature ensuring that information about PatMOS are available.
 * 
 * @p Properties
 * @li @ref INFO
 */
Feature<NoProcessor> INFO_FEATURE("otawa::patmos::INFO_FEATURE");

} }	// otawa::patmos

// semantics information
#define _ADD(d, s1, s2)		block.add(otawa::sem::add(d, s1, s2))
#define _SUB(d, s1, s2)		block.add(otawa::sem::sub(d, s1, s2))
#define _AND(d, s1, s2)		block.add(otawa::sem::_and(d, s1, s2))
#define _OR(d, s1, s2)		block.add(otawa::sem::_or(d, s1, s2))
#define _XOR(d, s1, s2)		block.add(otawa::sem::xor(d, s1, s2))
#define _CMP(d, s1, s2)		block.add(otawa::sem::cmp(d, s1, s2))
#define _SHL(d, s1, s2)		block.add(otawa::sem::shl(d, s1, s2))
#define _SHR(d, s1, s2)		block.add(otawa::sem::shr(d, s1, s2))
#define _ASR(d, s1, s2)		block.add(otawa::sem::asr(d, s1, s2))
#define _IF(d, s1, s2)		block.add(otawa::sem::_if(d, s1, s2))
#define _LOAD(d, s1, s2)	block.add(otawa::sem::load(d, s1, s2))
#define _STORE(d, s1, s2)	block.add(otawa::sem::store(d, s1, s2))
#define _BRANCH(d)		block.add(otawa::sem::branch(d))
#define _SETI(d, i)			block.add(otawa::sem::seti(d, i))
#define _SET(d, s)			block.add(otawa::sem::set(d, s))
#define _SCRATCH(d)			block.add(otawa::sem::scratch(d))
#define _R(n)				otawa::patmos::Platform::R_bank[n]->platformNumber()
#define _F(n)				otawa::patmos::Platform::R_bank[n]->platformNumber()
#define _PSR				otawa::patmos::Platform::PSR_reg.platformNumber()
#define _FSR				otawa::patmos::Platform::FSR_reg.platformNumber()
#define _NO_COND	otawa::sem::NO_COND
#define _EQ			otawa::sem::EQ
#define _LT			otawa::sem::LT
#define _LE			otawa::sem::LE
#define _GE			otawa::sem::GE
#define _GT			otawa::sem::GT
#define _ANY_COND	otawa::sem::ANY_COND
#define _NE			otawa::sem::NE
#define _ULT		otawa::sem::ULT
#define _ULE		otawa::sem::ULE
#define _UGE		otawa::sem::UGE
#define _UGT		otawa::sem::UGT
#define _WR_SAVE()		block.add(otawa::sem::inst(otawa::sem::SPEC, otawa::patmos::SPEC_SAVE))
#define _WR_RESTORE()	block.add(otawa::sem::inst(otawa::sem::SPEC, otawa::patmos::SPEC_RESTORE))
// #include "otawa_sem.h"


namespace otawa { namespace patmos {

/**
 */
void Process::getSem(::otawa::Inst *oinst, ::otawa::sem::Block& block) {
	//patmos_inst_t *inst;
	//inst = patmos_decode(_patmosDecoder, oinst->address().address());
	//patmos_sem(inst, block);
	//patmos_free_inst(inst);
}

} }	// namespace otawa::patmos

// Patmos GLISS Loader entry point
otawa::patmos::Loader OTAWA_LOADER_HOOK;
otawa::patmos::Loader& patmos_plugin = OTAWA_LOADER_HOOK;
