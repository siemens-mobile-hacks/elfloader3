//CL61v00 (p)Marie

        
#include "ep3.asm"


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
	DCD	0xA07E3740+1
	
	PUBLIC	OldOnCreate
OldOnCreate:
	DCD	0xA07E34C8+1
	
	PUBLIC	OldShowMsg
OldShowMsg:
	DCD	0xA0AE321C+1

	PUBLIC	PITgetN
PITgetN:
	DCD	0xA0B6E5C4
	
	PUBLIC	PITret
PITret:
	DCD	0xA046B654+1
	
	defadr	StoreErrInfoAndAbort,0xA0737838
	defadr	StoreErrString,0xA0737704

; <config>

; </config>

	END
