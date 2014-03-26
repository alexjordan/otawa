/*
 * License HERE!
 */

#include <otawa/parexegraph/GraphBBTime.h>
#include <otawa/parexegraph/ParExeGraph.h>
#include <otawa/cfg/features.h>

namespace tcrest { namespace patmos {

using namespace otawa;

class ExeGraph: public ParExeGraph {
public:
	ExeGraph(
		WorkSpace *ws,
		ParExeProc *proc,
		ParExeSequence *seq,
		const PropList &props = PropList::EMPTY): otawa::ParExeGraph(ws, proc, seq, props) { }
};

class BBTimer: public GraphBBTime<ExeGraph> {
public:
	static p::declare reg;
	BBTimer(void): GraphBBTime<ExeGraph>(reg) { }

protected:

	virtual void processBB(WorkSpace *ws, CFG *cfg, BasicBlock *bb) {
		if(bb->isEnd())
			return;
		
		// compute of each edge
		for(BasicBlock::InIterator edge(bb); edge; edge++)
			processEdge(ws, cfg, edge);
		
		// compute the minimum
		ot::time btime = type_info<ot::time>::max;
		for(BasicBlock::InIterator edge(bb); edge; edge++)
			btime = min(btime, *ipet::TIME(edge));
		ipet::TIME(bb) = btime;
		
		// build the delta
		for(BasicBlock::InIterator edge(bb); edge; edge++)
			ipet::TIME_DELTA(edge) = ipet::TIME(edge) - btime;
	}

	virtual void processEdge(WorkSpace *ws, CFG *cfg, Edge *edge)  {
		if(logFor(Processor::LOG_BLOCK))
			log << "\t\t\t" << edge << io::endl;

		// compute source and target
		BasicBlock *source = edge->source();
		if(source->isEntry() && CALLED_BY(source->cfg()).exists())
				return;
		BasicBlock *target;
		if(edge->kind() == Edge::CALL)
			target = edge->calledCFG()->firstBB();
		else
			target = edge->target();

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
		if(logFor(Processor::LOG_BLOCK))
			log << "\t\t\t\ttime(" << edge << ") = " << cost << io::endl;
		ipet::TIME(edge) = cost;
	}
};

p::declare BBTimer::reg = p::init("tcrest::patmos::BBTimer", Version(1, 0, 0))
	.base(GraphBBTime<ParExeGraph>::reg)
	.maker<BBTimer>();

} }		// tcrest::patmos
