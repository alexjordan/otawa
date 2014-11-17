/* Generated by gep ($(date)) copyright (c) 2011 IRIT - UPS */

#include <$(proc)/used_regs.h>
#include <$(proc)/api.h>
#include <$(proc)/macros.h>
#include <$(proc)/grt.h>

#define add_read(i)		rds[rd_cnt++] = i
#define add_write(i)	wrs[wr_cnt++] = i

typedef void (*used_regs_fun_t)($(proc)_inst_t *inst, $(proc)_used_regs_read_t rds, $(proc)_used_regs_write_t wrs);

/* functions */
static void used_regs_unknown($(proc)_inst_t *inst, $(proc)_used_regs_read_t rds, $(proc)_used_regs_write_t wrs) {
	rds[0] = -1;
	wrs[0] = -1;
}

$(foreach instructions)
static void used_regs_$(IDENT)($(proc)_inst_t *inst, $(proc)_used_regs_read_t rds, $(proc)_used_regs_write_t wrs) {
	int rd_cnt = 0, wr_cnt = 0;
$(used_regs)
	rds[rd_cnt] = -1;
	wrs[wr_cnt] = -1;
}

$(end)

/* table */
static used_regs_fun_t used_regs_tab[] = {
	used_regs_unknown$(foreach instructions),
	used_regs_$(IDENT)$(end)
};

/**
 * Get the register used in the given instruction.
 * @param buffer	Buffer to write in.
 * @param inst		Decode instruction.
 */
void $(proc)_used_regs($(proc)_inst_t *inst, $(proc)_used_regs_read_t rds, $(proc)_used_regs_write_t wrs) {
	used_regs_tab[inst->ident](inst, rds, wrs);
}