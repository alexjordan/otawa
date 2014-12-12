#define TARGET_ENDIANNESS big
#define HOST_ENDIANNESS little

/* WARNING: unsure */
//#define PATMOS_SYSCALL_CODE(i, s) ((s)->R[0])
//#define PATMOS_SYSCALL_MEM(s) ((s)->M)

/* start of a 16 reg window with 5 windows max */
//#define WIN(s)	(((s)->PSR & 0x1F) << 4)
//#define PATMOS_SYSPARM_REG32_RCNT 4
//#define PATMOS_SYSPARM_REG32_REG(s, i) 	((s)->R[WIN(s) + (i)])
//#define PATMOS_SYSPARM_REG32_SP(s) 	((s)->R[WIN(s) + 14])	/* %o6 */
/* WARNING: unsure */
//#define PATMOS_SYSPARM_REG32_RETURN(s, v)	{ (s)->R[WIN(s) + 8] = (v); }
//#define PATMOS_SYSPARM_REG32_SUCCEED(s)	{ (s)->R[WIN(s) + 9] = 0; }
//#define PATMOS_SYSPARM_REG32_FAILED(s)	{ (s)->R[WIN(s) + 9] = 0; }
