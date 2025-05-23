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

; <config>
; Autogenerated with tools/src/porting.ts

	defadr	OldFuncInOsInit,0xA054055C
	defadr	B_SocketFuncSocketReadStatus,0xA0A594C2+1
	defadr	B_SocketFuncSocketExitLoop,0xA0A594E4+1
	defadr	B_SocketFuncBindReadStatus,0xA0A597DE+1
	defadr	B_SocketFuncBindExitLoop,0xA0A59800+1
	defadr	B_SocketFuncConnectReadStatus,0xA0A599E6+1
	defadr	B_SocketFuncConnectExitLoop,0xA0A59A08+1
	defadr	B_SocketFuncShutdownReadStatus,0xA0A59B80+1
	defadr	B_SocketFuncShutdownExitLoop,0xA0A59BA2+1
	defadr	B_SocketFuncRecvReadStatus,0xA0A59DA6+1
	defadr	B_SocketFuncRecvExitLoop,0xA0A59DC8+1
	defadr	B_SocketFuncRecv2ReadStatus,0xA0A59E32+1
	defadr	B_SocketFuncRecv2ExitLoop,0xA0A59E54+1
	defadr	B_SocketFuncSendtoReadStatus,0xA0A5A5A6+1
	defadr	B_SocketFuncSendtoExitLoop,0xA0A5A5C8+1
	defadr	B_SocketFuncListenReadStatus,0xA0A5A720+1
	defadr	B_SocketFuncListenExitLoop,0xA0A5A742+1
	defadr	B_SocketFuncRecvfromReadStatus,0xA0A59FEC+1
	defadr	B_SocketFuncRecvfromExitLoop,0xA0A5A00E+1
	defadr	B_SocketFuncRecvfrom2ReadStatus,0xA0A5A084+1
	defadr	B_SocketFuncRecvfrom2ExitLoop,0xA0A5A0A6+1
	defadr	B_SocketFuncSendReadStatus,0xA0A5A29C+1
	defadr	B_SocketFuncSendExitLoop,0xA0A5A2BE+1
	defadr	B_SocketFuncSend2ReadStatus,0xA0A5A364+1
	defadr	B_SocketFuncSend2ExitLoop,0xA0A5A386+1
	defadr	B_SocketFuncGethostbynameReadStatus,0xA0A5AC92+1
	defadr	B_SocketFuncGethostbynameExitLoop,0xA0A5ACB4+1
	defadr	NU_Sleep,0xA009CD50

#include "patches/no_idle_restart.asm"
#include "patches/race_condition_in_sockets.asm"
; </config>

	END
