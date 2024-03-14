#include "ep3.asm"
#include "ll_init.asm"


	RSEG	DATA_N
	RSEG	SWILIB_FUNC1B8_1BB:CODE
	EXTERN	EXT2_AREA
	EXTERN	pngtop
	EXTERN	pLIB_TOP
	DCD	EXT2_AREA
	DCD	pngtop
	DCD	pLIB_TOP
	DCD	SFE(DATA_N)
        


defadr	MACRO	a,b
	PUBLIC	a
a	EQU	b
	ENDM

        RSEG	CODE:CODE

	PUBLIC	OldOnClose
OldOnClose:
	DCD	0xA066D3B8+1
	
	PUBLIC	OldOnCreate
OldOnCreate:
	DCD	0xA066D178+1
	
	PUBLIC	OldShowMsg
OldShowMsg:
	DCD	0xA094EA78+1

	PUBLIC	PITgetN
PITgetN:
	DCD	0xA0A32284
	
	PUBLIC	PITret
PITret:
	DCD	0xA094CEA4+1
	
	defadr	StoreErrInfoAndAbort,0xA04CCDD4
	defadr	StoreErrString,0xA04CCCA0
	defadr  OldFuncInOsInit,0xA054055C ; Hooked function in OS initialization (see PATCH_LL_INIT_HOOK)

	END
