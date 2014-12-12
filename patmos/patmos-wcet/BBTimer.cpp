/*
 * License HERE!
 */

#include <otawa/parexegraph/GraphBBTime.h>
#include <otawa/parexegraph/ParExeGraph.h>
#include <otawa/cfg/features.h>
#include "../otawa-patmos/patmos.h"

namespace tcrest { namespace patmos {

using namespace otawa;

class ExeGraph: public ParExeGraph {
public:
	ExeGraph(
		WorkSpace *ws,
		ParExeProc *proc,
		ParExeSequence *seq,
		const PropList &props = PropList::EMPTY): otawa::ParExeGraph(ws, proc, seq, props)
	{
		// initialization
		info = otawa::patmos::INFO(ws->process());
		ASSERT(info);
		reg_count = ws->process()->platform()->regCount();

		// lookout for stage
		for(ParExePipeline::StageIterator stage(proc->pipeline()); stage; stage++) {
			if(stage->name() == "MEM")
				mem_stage = stage;
			else if(stage->name() == "EX")
				for(int i = 0; i < stage->numFus(); i++)
					for(ParExePipeline::StageIterator fu(stage->fu(i)); fu; fu++)
						if(fu->name() == "ALU")
							exe_stage = fu;
		}
		ASSERT(exe_stage);
		ASSERT(mem_stage);
	}

	virtual void addEdgesForFetch(void) {
		addBundledProgramOrder(_microprocessor->fetchStage());
	}

	virtual void addEdgesForProgramOrder(genstruct::SLList<ParExeStage *> *list_of_stages) {
		
		// prepare the stages
		elm::genstruct::SLList<ParExeStage *> *list;
		if(list_of_stages)
			list = list_of_stages;
		else {
			list = new  elm::genstruct::SLList<ParExeStage *>;
			for(ParExePipeline::StageIterator stage(_microprocessor->pipeline()) ; stage ; stage++)
				if(stage->orderPolicy() == ParExeStage::IN_ORDER) {
					if(stage->category() != ParExeStage::FETCH
					&& stage->category() != ParExeStage::EXECUTE)
						list->add(stage);
				}
		}
		
		// build the edges
		for(genstruct::SLList<ParExeStage *>::Iterator stage(*list); stage; stage++)
			addBundledProgramOrder(stage);
	}

	virtual void findDataDependencies(void) {
#	if 0
		// prepare registers bank
		ParExeNode *regs[reg_count];
		//elm::array::set(regs, reg_count, static_cast<ParExeNode *>(0));
		for(int i = 0; i < reg_count; i++)
			regs[i] = 0;

		// look in the instructions
		for (InstIterator inst(this->getSequence()) ; inst ; inst++)  {

			// process read registers
			const elm::genstruct::Table<hard::Register *>& reads = inst->inst()->readRegs();
			ParExeNode *exe_node = 0;
			for(int i = 0; i < reads.count(); i++)
				if(regs[reads[i]->platformNumber()]) {
					if(!exe_node) {
						for(ParExeInst::NodeIterator node(inst); node; node++)
							if(node->stage() == exe_stage) {
								exe_node = node;
								break;
							}
						ASSERT(exe_node);
					}
					ASSERT(0 <= reads[i]->platformNumber() && reads[i]->platformNumber() < reg_count);
					new ParExeEdge(regs[reads[i]->platformNumber()], exe_node, ParExeEdge::SOLID);
				}

			// process written registers if load only
			const elm::genstruct::Table<hard::Register *>& writs = inst->inst()->writtenRegs();
			ParExeNode *mem_node = 0;
			for(int i = 0; i < writs.count(); i++) {
				if(inst->inst()->isLoad() && !mem_node) {
					for(ParExeInst::NodeIterator node(inst); node; node++)
						if(node->stage() == mem_stage) {
								mem_node = node;
								break;
							}
					ASSERT(mem_node);
				}
				regs[writs[i]->platformNumber()] = mem_node;
			}
		}
#		endif
	}

private:

	void addBundledProgramOrder(ParExeStage *stage) {

		// prepare first bundle
		ParExeStage::NodeIterator node(stage);
		ParExeNode *last = node;
		Address btop = last->inst()->inst()->address() + info->bundleSize(last->inst()->inst()->address());

		// loop over nodes
		for(node++; node; last = *node, node++) {
			
			// inside the bundle
			if(node->inst()->codePart() == last->inst()->codePart() && node->inst()->inst()->address() < btop)
				new ParExeEdge(last, *node, ParExeEdge::SLASHED);
			
			// new bundle
			else {
				new ParExeEdge(last, *node, ParExeEdge::SOLID);
				btop = node->inst()->inst()->address() + info->bundleSize(node->inst()->inst()->address());
			}
		}
	}

	otawa::patmos::Info *info;
	int reg_count;
	ParExeStage *exe_stage, *mem_stage;
};

class BBTimer: public GraphBBTime<ExeGraph> {
public:
	static p::declare reg;
	BBTimer(void): GraphBBTime<ExeGraph>(reg) { }

protected:

	virtual void processBB(WorkSpace *ws, CFG *cfg, BasicBlock *bb) {
		if(bb->isEnd())
			return;
		
		// computation of each edge
		genstruct::Vector<Edge *> edges;
		for(BasicBlock::InIterator edge(bb); edge; edge++) {
			if(!edge->source()->isEntry() || !cfg->hasProp(CALLED_BY))
				edges.add(edge);
			else
				for(Identifier<Edge *>::Getter edge(cfg, CALLED_BY); edge; edge++)
					edges.add(edge);
		}
		
		// compute the minimum
		genstruct::Vector<ot::time> times;
		ot::time btime = type_info<ot::time>::max;
		for(int i = 0; i < edges.count(); i++) {
			ot::time time = compute(ws, cfg, edges[i], bb);
			times.add(time);
			btime = min(btime, time);
		}
		
		// build the delta
		ipet::TIME(bb) = btime;
		if(logFor(Processor::LOG_BLOCK))
			log << "\t\t\t\ttime(" << bb << ") = " << btime << io::endl;
		for(int i = 0; i < edges.count(); i++) {
			ipet::TIME_DELTA(edges[i]) = times[i] - btime;
			if(logFor(Processor::LOG_BLOCK))
				log << "\t\t\t\ttime(" << edges[i] << ") = " << *ipet::TIME_DELTA(edges[i]) << io::endl;
		}
	}

	virtual ot::time compute(WorkSpace *ws, CFG *cfg, Edge *edge, BasicBlock *bb)  {
		if(logFor(Processor::LOG_BLOCK))
			log << "\t\t\t" << edge << io::endl;

		// compute source and target
		BasicBlock 	*source = edge->source(),
					*target = bb;

		// initialize the sequence
		int index = 0;
		ParExeSequence *seq = new ParExeSequence();

		// fill with previous block instructions
		if(!source->isEnd())
			for(BasicBlock::InstIterator inst(source); inst; inst++) {
				ParExeInst * par_exe_inst = new ParExeInst(inst, source, PROLOGUE, index++);
				seq->addLast(par_exe_inst);
			}

		// fill with current block instructions
		for(BasicBlock::InstIterator inst(target); inst; inst++) {
			ParExeInst * par_exe_inst = new ParExeInst(inst, target, BODY, index++);
			seq->addLast(par_exe_inst);
		}

		// build the graph
		PropList props;
		ExeGraph graph(this->workspace(), _microprocessor, seq, props);
		graph.build();
		
		// compute the graph
		ot::time cost = graph.analyze();
		outputGraph(&graph, source->number(), target->number(), 0, "");
		return cost;
	}
};

p::declare BBTimer::reg = p::init("tcrest::patmos_wcet::BBTimer", Version(1, 0, 0))
	.base(GraphBBTime<ParExeGraph>::reg)
	.maker<BBTimer>();

} }		// tcrest::patmos
