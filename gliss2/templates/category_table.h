/* Generated by gep ($(date)) copyright (c) 2008 IRIT - UPS */

#ifndef GLISS_$(PROC)_INCLUDE_$(PROC)_CATEGORY_TABLE_H
#define GLISS_$(PROC)_INCLUDE_$(PROC)_CATEGORY_TABLE_H


#if defined(__cplusplus)
extern  "C"
{
#endif

#include <$(proc)/api.h>
#include <$(proc)/macros.h>


#define gliss_error(e) fprintf(stderr, (e))


static int $(proc)_category_table[] =
{
	$(invalid_category)$(foreach instructions),
	$(category)$(end)
};



#if defined(__cplusplus)
}
#endif

#endif /* GLISS_$(PROC)_INCLUDE_$(PROC)_CATEGORY_TABLE_H */
