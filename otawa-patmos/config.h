#define TARGET_ENDIANNESS big
#define HOST_ENDIANNESS little

/* WARNING: unsure */
#define LEON_SYSCALL_CODE(i, s) ((s)->R[0])
#define LEON_SYSCALL_MEM(s) ((s)->M)

/* start of a 16 reg window with 5 windows max */
#define WIN(s)	(((s)->PSR & 0x1F) << 4)
#define LEON_SYSPARM_REG32_RCNT 4
#define LEON_SYSPARM_REG32_REG(s, i) 	((s)->R[WIN(s) + (i)])
#define LEON_SYSPARM_REG32_SP(s) 	((s)->R[WIN(s) + 14])	/* %o6 */
/* WARNING: unsure */
#define LEON_SYSPARM_REG32_RETURN(s, v)	{ (s)->R[WIN(s) + 8] = (v); }
#define LEON_SYSPARM_REG32_SUCCEED(s)	{ (s)->R[WIN(s) + 9] = 0; }
#define LEON_SYSPARM_REG32_FAILED(s)	{ (s)->R[WIN(s) + 9] = 0; }
