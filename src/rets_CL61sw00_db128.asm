//CL61v00 db128 (p)Evgr

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
	DCD	0xA07F0518+1
	
	PUBLIC	OldOnCreate
OldOnCreate:
	DCD	0xA07F02B8+1
	
	PUBLIC	OldShowMsg
OldShowMsg:
	DCD	0xA0AF3EFC+1

	PUBLIC	PITgetN
PITgetN:
	DCD	0xA0198C5C
	
	PUBLIC	PITret
PITret:
	DCD	0xA047E808+1
	
	defadr	StoreErrInfoAndAbort,0xA03FACC8
	defadr	StoreErrString,0xA03FAB94

; <config>
; Autogenerated with tools/porting.js

	defadr	OldFuncInOsInit,0xA076AD80
	defadr	B_SocketFuncSocketReadStatus,0xA0B6C442+1
	defadr	B_SocketFuncSocketExitLoop,0xA0B6C464+1
	defadr	B_SocketFuncBindReadStatus,0xA0B6C75E+1
	defadr	B_SocketFuncBindExitLoop,0xA0B6C780+1
	defadr	B_SocketFuncConnectReadStatus,0xA0B6C966+1
	defadr	B_SocketFuncConnectExitLoop,0xA0B6C988+1
	defadr	B_SocketFuncShutdownReadStatus,0xA0B6CB00+1
	defadr	B_SocketFuncShutdownExitLoop,0xA0B6CB22+1
	defadr	B_SocketFuncRecvReadStatus,0xA0B6CD26+1
	defadr	B_SocketFuncRecvExitLoop,0xA0B6CD48+1
	defadr	B_SocketFuncRecv2ReadStatus,0xA0B6CDB2+1
	defadr	B_SocketFuncRecv2ExitLoop,0xA0B6CDD4+1
	defadr	B_SocketFuncSendtoReadStatus,0xA0B6D526+1
	defadr	B_SocketFuncSendtoExitLoop,0xA0B6D548+1
	defadr	B_SocketFuncListenReadStatus,0xA0B6D6A0+1
	defadr	B_SocketFuncListenExitLoop,0xA0B6D6C2+1
	defadr	B_SocketFuncRecvfromReadStatus,0xA0B6CF6C+1
	defadr	B_SocketFuncRecvfromExitLoop,0xA0B6CF8E+1
	defadr	B_SocketFuncRecvfrom2ReadStatus,0xA0B6D004+1
	defadr	B_SocketFuncRecvfrom2ExitLoop,0xA0B6D026+1
	defadr	B_SocketFuncSendReadStatus,0xA0B6D21C+1
	defadr	B_SocketFuncSendExitLoop,0xA0B6D23E+1
	defadr	B_SocketFuncSend2ReadStatus,0xA0B6D2E4+1
	defadr	B_SocketFuncSend2ExitLoop,0xA0B6D306+1
	defadr	B_SocketFuncGethostbynameReadStatus,0xA0B6DC12+1
	defadr	B_SocketFuncGethostbynameExitLoop,0xA0B6DC34+1
	defadr	NU_Sleep,0xA009CD14

#include "patches/no_idle_restart.asm"
#include "patches/race_condition_in_sockets.asm"
; </config>

	END
