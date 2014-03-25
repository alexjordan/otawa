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

	#include "config.h"
}

#include "otawa_delayed.h"

/* instruction kind */
#include "otawa_kind.h"

/* used registers */
#include "otawa_used_regs.h"

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

// Platform class
class Platform: public hard::Platform {
public:
	static const Identification ID;
	Platform(const PropList& props = PropList::EMPTY);
	Platform(const Platform& platform, const PropList& props = PropList::EMPTY);

	// Registers
	static hard::Register PSR_reg;
	static hard::Register FSR_reg;
	static const hard::PlainBank R_bank;
	static const hard::PlainBank F_bank;
	static const hard::MeltedBank MISC_bank;

	// otawa::Platform overload
	virtual bool accept(const Identification& id);
};


// SimState class
class SimState: public otawa::SimState
{
public:
	SimState(Process *process, patmos_state_t *state, patmos_decoder_t *decoder, bool _free = false)
	: otawa::SimState(process), _leonState(state), _leonDecoder(decoder) {
		ASSERT(process);
		ASSERT(state);
	}

	virtual ~SimState(void) {
		patmos_delete_state(_leonState);
	}

	virtual void setSP(const Address& addr) { LEON_SYSPARM_REG32_SP(_leonState) = addr.offset(); }

	inline patmos_state_t *leonState(void) const { return _leonState; }

	virtual Inst *execute(Inst *oinst) {
		ASSERTP(oinst, "null instruction pointer");

		Address addr = oinst->address();
		patmos_inst_t *inst;
		_leonState->nPC = addr.address();
		inst = patmos_decode(_leonDecoder, _leonState->nPC);
		patmos_execute(_leonState, inst);
		patmos_free_inst(inst);
		if (_leonState->nPC == oinst->topAddress()) {
			Inst *next = oinst->nextInst();
			while (next && next->isPseudo())
				next = next->nextInst();
			if(next && next->address() == Address(_leonState->nPC))
				return next;
		}
		Inst *next = process()->findInstAt(_leonState->nPC);
		ASSERTP(next, "cannot find instruction at " << Address(_leonState->nPC) << " from " << oinst->address());
		return next;
	}

private:
	patmos_state_t *_leonState;
	patmos_decoder_t *_leonDecoder;
};


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
		patmos_state_t *s = patmos_new_state(_leonPlatform);
		ASSERTP(s, "otawa::leon::Process::newState(), cannot create a new leon_state");
		return new SimState(this, s, _leonDecoder, true);
	}

	virtual int instSize(void) const { return 4; }
	void decodeRegs( Inst *inst, elm::genstruct::AllocatedTable<hard::Register *> *in, elm::genstruct::AllocatedTable<hard::Register *> *out);
	inline patmos_decoder_t *leonDecoder() { return _leonDecoder;}
	inline void *leonPlatform(void) const { return _leonPlatform; }
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
	virtual patmos_memory_t *leonMemory(void) { return _leonMemory; }

private:
	otawa::Inst *_start;
	hard::Platform *_platform;
	patmos_platform_t *_leonPlatform;
	patmos_memory_t *_leonMemory;
	patmos_decoder_t *_leonDecoder;
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
		patmos_inst_t *inst = patmos_decode(proc.leonDecoder(), _addr);
		patmos_disasm(out_buffer, inst);
		patmos_free_inst(inst);
		out << out_buffer;
	}

	virtual kind_t kind() { return _kind; }
	virtual address_t address() const { return _addr; }
	virtual t::size size() const { return 4; }
	virtual Process &process() { return proc; }

	virtual const elm::genstruct::Table<hard::Register *>& readRegs() {
		if ( ! isRegsDone)
		{
			decodeRegs();
			isRegsDone = true;
		}
		return in_regs;
	}

	virtual const elm::genstruct::Table<hard::Register *>& writtenRegs() {
		if ( ! isRegsDone)
		{
			decodeRegs();
			isRegsDone = true;
		}
		return out_regs;
	}

	virtual void semInsts (sem::Block &block) {
		proc.getSem(this, block);
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
		inst = patmos_decode(proc.leonDecoder(), (patmos_address_t)address());
		otawa::delayed_t delay = patmos_delayed(inst);
		if(delay != otawa::DELAYED_None)
			otawa::DELAYED(this) = delay;
	}

	virtual t::size size() const { return 4; }

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


/**
 * Register banks.
 */
static const RegBank *banks[] = {
	&Platform::R_bank,
	&Platform::F_bank,
	&Platform::MISC_bank	// PSR, FSR
};


static const elm::genstruct::Table<const RegBank *> banks_table(banks, 3);


/**
 * R register bank.
 */
const PlainBank Platform::R_bank("R", hard::Register::INT,  32, "%%r%d", 8+8*16/*GET SIZE FROM A TPL*/);


/**
 * F register bank.
 */
const PlainBank Platform::F_bank("F", hard::Register::FLOAT, 64, "%%f%d", 32);


/**
 * PSR register
 */
hard::Register Platform::PSR_reg("psr", hard::Register::INT, 32);


/**
 * FSR register
 */
hard::Register Platform::FSR_reg("fsr", hard::Register::INT, 32);


/**
 * MISC register bank
 */
const hard::MeltedBank Platform::MISC_bank("MISC", &Platform::PSR_reg, &Platform::FSR_reg, 0);


/**
 * Identification of the default platform.
 */
const Platform::Identification Platform::ID("patmos-elf-");


/**
 * Build a new gliss platform with the given configuration.
 * @param props		Configuration properties.
 */
Platform::Platform(const PropList& props): hard::Platform(ID, props) {
	setBanks(banks_table);
}


/**
 * Build a new platform by cloning.
 * @param platform	Platform to clone.
 * @param props		Configuration properties.
 */
Platform::Platform(const Platform& platform, const PropList& props)
: hard::Platform(platform, props) {
	setBanks(banks_table);
}


/**
 */
bool Platform::accept(const Identification& id) {
	return id.abi() == "elf" && id.architecture() == "powerpc";
}


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
	_leonMemory(0),
	init(false),
	map(0),
	file(0)
{
	ASSERTP(manager, "manager required");
	ASSERTP(platform, "platform required");

	// gliss2 leon structs
	_leonPlatform = patmos_new_platform();
	ASSERTP(_leonPlatform, "otawa::leon::Process::Process(..), cannot create a leon_platform");
	_leonDecoder = patmos_new_decoder(_leonPlatform);
	ASSERTP(_leonDecoder, "otawa::leon::Process::Process(..), cannot create a leon_decoder");
	_leonMemory = patmos_get_memory(_leonPlatform, SPARC_MAIN_MEMORY);
	ASSERTP(_leonMemory, "otawa::leon::Process::Process(..), cannot get main leon_memory");
	patmos_lock_platform(_leonPlatform);

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
	patmos_delete_decoder(_leonDecoder);
	patmos_unlock_platform(_leonPlatform);
	if(_gelFile)
		gel_close(_gelFile);
}



/**
 */
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


/**
 */
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


/**
 */
hard::Platform *Process::platform(void) {
	return _platform;
}


/**
 */
otawa::Inst *Process::start(void) {
	return _start;
}


/**
 */
File *Process::loadFile(elm::CString path) {
	LTRACE;

	// Check if there is not an already opened file !
	if(program())
		throw LoadException("loader cannot open multiple files !");

	File *file = new otawa::File(path);
	addFile(file);

	// initialize the environment
	ASSERTP(_leonPlatform, "invalid leon_platform !");
	patmos_env_t *env = patmos_get_sys_env(_leonPlatform);
	ASSERT(env);
	env->argc = argc;
	env->argv = argv;
	env->envp = envp;

	// load the binary
	if(patmos_load_platform(_leonPlatform, (char *)&path) == -1)
		throw LoadException(_ << "cannot load \"" << path << "\".");

	// get the initial state
	SimState *state = dynamic_cast<SimState *>(newState());
	patmos_state_t *leonState = state->leonState();
	if (!leonState)
		throw LoadException("invalid leon_state !");

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
	_leonMemory = leonMemory();
	ASSERTP(_leonMemory, "memory information mandatory");
	_start = findInstAt((address_t)infos.entry);
	return file;
}


// Memory read
#define GET(t, s) \
	void Process::get(Address at, t& val) { \
			val = patmos_mem_read##s(_leonMemory, at.address()); \
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


/**
 */
void Process::get(Address at, string& str) {
	Address base = at;
	while(!patmos_mem_read8(_leonMemory, at.address()))
		at = at + 1;
	int len = at - base;
	char buf[len];
	get(base, buf, len);
	str = String(buf, len);
}


/**
 */
void Process::get(Address at, char *buf, int size)
	{ patmos_mem_read(_leonMemory, at.address(), buf, size); }


/**
 */
otawa::Inst *Process::decode(Address addr) {
	//cerr << "DECODING: " << addr << io::endl;

	// Decode the instruction
	patmos_inst_t *inst;
	TRACE("ADDR " << addr);
	inst = patmos_decode(_leonDecoder, (patmos_address_t)addr.address());

	// Build the instruction
	Inst::kind_t kind = 0;
	otawa::Inst *result = 0;

	// get the kind from the nmp otawa_kind attribute
	if(inst->ident == SPARC_UNKNOWN)
		TRACE("UNKNOWN !!!\n" << result);
	else
		kind = patmos_kind(inst);

	// detect the false branch instructions
	/*switch (inst->ident)
	{
		case PPC_BL_D:
			if (PPC_BL_D_x_x_BRANCH_ADDR_n == 1)
				kind = Inst::IS_ALU | Inst::IS_INT;
			break;
		case PPC_BCL_D_D_D:
			if (PPC_BCL_D_D_D_x_x_x_BD_n == 1) {
				kind = Inst::IS_ALU | Inst::IS_INT;
				cerr << "INFO: no control at " << io::endl;
			}
			break;
	}*/
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


/**
 */
patmos_address_t BranchInst::decodeTargetAddress(void) {

	// Decode the instruction
	patmos_inst_t *inst;
	TRACE("ADDR " << addr);
	inst = patmos_decode(proc.leonDecoder(), (patmos_address_t)address());

	// retrieve the target addr from the nmp otawa_target attribute
	Address target_addr;
	patmos_address_t res = patmos_target(inst);
	if(res != 0)
		target_addr = res;

	/* pattern recognition of
	 *	sethi	%hi(base), %r
	 *	jmp[l]	[%r + offset], %s
	 * target address is (base << 10) + offset.
	 */
	if(res == 0
	&& inst->ident == SPARC_JMPL__LT_S__P__D_RT___S
	&& SPARC_JMPL__LT_S__P__D_RT___S_x_x_i != 0) {

		// get information
		patmos_inst_t *old_inst = inst;
		t::uint32 rs1 = SPARC_JMPL__LT_S__P__D_RT___S_x_x_rs1_idx;
		t::int32 offset = SPARC_JMPL__LT_S__P__D_RT___S_x_x_rs2_val;

		// get previous instructions
		otawa::Inst *prev = this->prevInst();
		if(prev) {
			inst = patmos_decode(proc.leonDecoder(), patmos_address_t(prev->address()));
			if(inst->ident == SPARC_SETHI_D__S
			&& rs1 == SPARC_SETHI_D__S_x_x_rd_idx )
				target_addr = (SPARC_SETHI_D__S_x_x_imm22 << 10) + offset;
			patmos_free_inst(inst);
		}

		// restore
		inst = old_inst;
	}

	// Return result
	patmos_free_inst(inst);
	return target_addr;
}


// Platform definition
static hard::Platform::Identification PFID("leon-*-*");



// read and written registers infos

// values taken from nmp files

// access types
#define READ_REG     1
#define WRITE_REG    2
#define REG_RANGE    0x10
#define READ_RANGE   READ_REG | REG_RANGE
#define WRITE_RANGE  WRITE_REG | REG_RANGE
#define END_REG      0

// reg banks macros
#define BANK_R		1
#define BANK_F		2
#define BANK_PC		3
#define BANK_nPC	4
#define BANK_Y		5
#define BANK_WIM	6
#define BANK_PSR	7
#define BANK_FSR	8
#define BANK_ASR	9
#define BANK_TBR	10


// convert a gliss reg info into one or several otawa Registers,
// in and out are supposed initialized by the caller
static void translate_gliss_reg_info(otawa_patmos_reg_t reg_info, elm::genstruct::Vector<hard::Register *> &in, elm::genstruct::Vector<hard::Register *> &out)
{
	if (reg_info == END_REG)
		return;

	uint8_t access_type	= ((reg_info & 0xFF000000) >> 24);
	uint8_t gliss_bank 	= ((reg_info & 0x00FF0000) >> 16);
	bool is_range 		= access_type & REG_RANGE;
	// READ_REG and WRITE_REG can be both specified at the same time
	bool is_read 		= access_type & READ_REG;
	bool is_write 		= access_type & WRITE_REG;
	uint16_t reg_num_lo 	= (is_range) ? ((reg_info & 0x0000FF00) >> 8) : (reg_info & 0x0000FFFF);
	uint16_t reg_num_up 	= (is_range) ? (reg_info & 0x000000FF) : reg_num_lo;
	int reg_count 		= reg_num_up - reg_num_lo + 1;
	assert(reg_count > 0);

	const hard::RegBank *reg_bank = 0;
	hard::Register *reg_no_bank = 0;
	switch (gliss_bank)
	{
		case 255:
			return;
		case BANK_R:
			reg_bank = &Platform::R_bank;
			break;
		case BANK_F:
			reg_bank = &Platform::F_bank;
			break;
		case BANK_PSR:
			reg_no_bank = &Platform::PSR_reg;
			break;
		case BANK_FSR:
			reg_no_bank = &Platform::FSR_reg;
			break;
		case BANK_PC:
		case BANK_nPC:
		case BANK_Y:
		case BANK_WIM:
		case BANK_ASR:
		case BANK_TBR:
			return;
		default:
			ASSERTP(false, "unknown bank " << gliss_bank << ", code=" << io::hex(reg_info));
	}

	//otawa_reg.allocate(reg_count);
	for (int i = reg_num_lo ; i <= reg_num_up ; i++)
	{
		if (reg_bank)
		{
			if (is_read)
				in.add(reg_bank->get(i));
			if (is_write)
				out.add(reg_bank->get(i));
		}
		else
		{
			if (is_read)
				in.add(reg_no_bank);
			if (is_write)
				out.add(reg_no_bank);
		}
	}
}


/**
 */
void Process::decodeRegs(otawa::Inst *oinst, elm::genstruct::AllocatedTable<hard::Register *> *in,
	elm::genstruct::AllocatedTable<hard::Register *> *out)
{

	// Decode instruction
	patmos_inst_t *inst;
	inst = patmos_decode(_leonDecoder, oinst->address().address());
	if(inst->ident == SPARC_UNKNOWN)
	{
		patmos_free_inst(inst);
		return;
	}

	// get register infos
	elm::genstruct::Vector<hard::Register *> reg_in;
	elm::genstruct::Vector<hard::Register *> reg_out;
	otawa_patmos_reg_t *addr_reg_info = patmos_used_regs(inst);
	if(addr_reg_info)
		for (int i = 0; addr_reg_info[i] != END_REG; i++ )
			translate_gliss_reg_info(addr_reg_info[i], reg_in, reg_out);

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

// otawa::loader::leon::Loader class
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
	"elf_20"
};
static elm::genstruct::Table<string> leon_aliases(table, 1);


/**
 * Build a new loader.
 */
Loader::Loader(void)
: otawa::Loader("leon", Version(2, 0, 0), OTAWA_LOADER_VERSION, leon_aliases) {
}


/**
 * Get the name of the loader.
 * @return Loader name.
 */
CString Loader::getName(void) const
{
	return "leon";
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
	//cout << "INFO: using leon loader.\n";	// !!DEBUG!!
	return new Process(man, new Platform(props), props);
}

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
#include "otawa_sem.h"


namespace otawa { namespace patmos {

/**
 */
void Process::getSem(::otawa::Inst *oinst, ::otawa::sem::Block& block) {
	patmos_inst_t *inst;
	inst = patmos_decode(_leonDecoder, oinst->address().address());
	patmos_sem(inst, block);
	patmos_free_inst(inst);
}

} }	// namespace otawa::patmos

// Patmos GLISS Loader entry point
otawa::patmos::Loader OTAWA_LOADER_HOOK;
otawa::patmos::Loader& leon_plugin = OTAWA_LOADER_HOOK;
