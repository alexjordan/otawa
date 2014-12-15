/* Generated by gliss-attr ($(date)) copyright (c) 2009 IRIT - UPS */

#include <$(proc)/api.h>
#include <$(proc)/id.h>
#include <$(proc)/macros.h>

#include <otawa/prog/features.h>

typedef int (*delayed_fun_t)($(proc)_inst_t *inst);



static int otawa_delayed_UNKNOWN($(proc)_inst_t *inst)
{
        /* this code should also be used as default value if
        an instruction has no otawa_target field */
        return 0;
}

$(foreach instructions)
static int otawa_delayed_$(IDENT)($(proc)_inst_t *inst) {
$(otawa_delayed)
};

$(end)


/* function table */
static delayed_fun_t $(proc)_delayed_table[] =
{
	otawa_delayed_UNKNOWN$(foreach instructions),
	otawa_delayed_$(IDENT)$(end)
};



/**
 * Get the OTAWA target of the instruction.
 * @return OTAWA target.
 */
otawa::delayed_t $(proc)_delayed($(proc)_inst_t *inst)
{
        return (otawa::delayed_t)$(proc)_delayed_table[inst->ident](inst);
}