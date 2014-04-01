#include <otawa/proc/EdgeProcessor.h>
#include <otawa/ilp/features.h>
#include <otawa/ipet/features.h>
#include <otawa/ilp/System.h>
#include <otawa/ilp/Constraint.h>

namespace tcrest { namespace patmos {

using namespace otawa;

class MethodCacheContributor: public EdgeProcessor {
public:
	static p::declare reg;
	MethodCacheContributor(p::declare& r = reg): EdgeProcessor(r), sys(0), exp(false) { }

protected:

	virtual void configure(const PropList& props) {
		EdgeProcessor::configure(props);
		exp = ipet::EXPLICIT(props);
	}

	virtual void setup(WorkSpace *ws) {
		sys = ipet::SYSTEM(ws);
	}

	virtual void processEdge(WorkSpace *ws, CFG *cfg, Edge *edge) {
		if(!doesMethodLoad(edge))
			return;

		// x_f -- number of method load
		// c_f -- cost of load
		// x_f^h -- head block controlling the number of load (typically a loop header entry edge)
		// x_e -- number of time the edge is taken (performing the call)

		// make the variables x_f
		string name;
		if(exp)
			name = _ << "x_mc_" << cfg->label() << "_" << edge->source()->number() << "_" << edge->target()->number();
		ilp::Var *x_f = sys->newVar(name);

		// x_f <= x_f^h		(bounding by the controlling header)
		ilp::Constraint *c1 = sys->newConstraint("method cache control", ilp::Constraint::LE);
		c1->addLeft(1, x_f);
		c1->addRight(1, loadVar(edge));

		// x_f <= x_e
		ilp::Constraint *c2 = sys->newConstraint("method cache edge", ilp::Constraint::LE);
		c1->addLeft(1, x_f);
		c1->addRight(1, ipet::VAR(edge));

		// wcet += c_f x_f		add contribution to the WCET
		sys->addObjectFunction(double(loadTime(edge)), x_f);
	}

private:

	bool doesMethodLoad(Edge *edge) {
		// TODO do the job here
		return false;
	}

	ot::time loadTime(Edge *edge) {
		// TODO compute the load time here
		return 0;
	}

	ilp::Var *loadVar(Edge *edge) {
		// TODO variable to be dependent on (possibly null)
		return 0;
	}

	ilp::System *sys;
	bool exp;
};

p::feature METHOD_CACHE_CONTRIBUTION_FEATURE("tcrest::patmos::METHOD_CACHE_CONTRIBUTION_FEATURE", new Maker<MethodCacheContributor>());

p::declare MethodCacheContributor::reg = p::init("tcrest::patmos::MethodCacheContributor", Version(1, 0, 0))
	.base(EdgeProcessor::reg)
	.maker<MethodCacheContributor>()
	.require(ipet::ILP_SYSTEM_FEATURE)
	.require(ipet::ASSIGNED_VARS_FEATURE)
	.provide(METHOD_CACHE_CONTRIBUTION_FEATURE);

} }	// tcrest::patmos
