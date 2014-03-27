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
		info = otawa::patmos::INFO(ws->process());
		ASSERT(info);
	}
		
	virtual void addEdgesForFetch(void) {

		// prepare first bundle
		ParExeStage *fetch_stage = _microprocessor->fetchStage();
		ParExeStage::NodeIterator node(fetch_stage);
		ParExeNode *last = node;
		Address btop = last->inst()->inst()->address() + info->bundleSize(last->inst()->inst()->address());

		// loop over nodes
		for(node++; node; last = *node, node++) {
			
			// inside the bundle
			if(node->inst()->inst()->address() < btop)
				new ParExeEdge(last, *node, ParExeEdge::SLASHED);
			
			// new bundle
			else {
				new ParExeEdge(last, *node, ParExeEdge::SOLID);
				btop = node->inst()->inst()->address() + info->bundleSize(node->inst()->inst()->address());
			}
		}

	}

private:
	otawa::patmos::Info *info;
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
