; Fixing stack corruption in socket functions in case of race condition.

; socket()
	THUMB
	RSEG	PATCH_SOCKET_FUNC_SOCKET:CODE:ROOT(2)

	CMP R0, #0
	BNE B_SocketFuncSocketExitLoop
	MOVS R0, #1
	LDR R1, =NU_Sleep
	BLX R1
	B B_SocketFuncSocketReadStatus
	NOP
	NOP

; bind()
	THUMB
	RSEG	PATCH_SOCKET_FUNC_BIND:CODE:ROOT(2)

	CMP R0, #0
	BNE B_SocketFuncBindExitLoop
	MOVS R0, #1
	LDR R1, =NU_Sleep
	BLX R1
	B B_SocketFuncBindReadStatus
	NOP
	NOP

; connect()
	THUMB
	RSEG	PATCH_SOCKET_FUNC_CONNECT:CODE:ROOT(2)

	CMP R0, #0
	BNE B_SocketFuncConnectExitLoop
	MOVS R0, #1
	LDR R1, =NU_Sleep
	BLX R1
	B B_SocketFuncConnectReadStatus
	NOP
	NOP

; shutdown()
	THUMB
	RSEG	PATCH_SOCKET_FUNC_SHUTDOWN:CODE:ROOT(2)

	CMP R0, #0
	BNE B_SocketFuncShutdownExitLoop
	MOVS R0, #1
	LDR R1, =NU_Sleep
	BLX R1
	B B_SocketFuncShutdownReadStatus
	NOP
	NOP

; recv() #1
	THUMB
	RSEG	PATCH_SOCKET_FUNC_RECV:CODE:ROOT(2)

	CMP R0, #0
	BNE B_SocketFuncRecvExitLoop
	MOVS R0, #1
	LDR R1, =NU_Sleep
	BLX R1
	B B_SocketFuncRecvReadStatus
	NOP
	NOP

; recv() #2
	THUMB
	RSEG	PATCH_SOCKET_FUNC_RECV2:CODE:ROOT(2)

	CMP R0, #0
	BNE B_SocketFuncRecv2ExitLoop
	MOVS R0, #1
	LDR R1, =NU_Sleep
	BLX R1
	B B_SocketFuncRecv2ReadStatus
	NOP
	NOP

; sendto()
	THUMB
	RSEG	PATCH_SOCKET_FUNC_SENDTO:CODE:ROOT(2)

	CMP R0, #0
	BNE B_SocketFuncSendtoExitLoop
	MOVS R0, #1
	LDR R1, =NU_Sleep
	BLX R1
	B B_SocketFuncSendtoReadStatus
	NOP
	NOP

; listen()
	THUMB
	RSEG	PATCH_SOCKET_FUNC_LISTEN:CODE:ROOT(2)

	CMP R0, #0
	BNE B_SocketFuncListenExitLoop
	MOVS R0, #1
	LDR R1, =NU_Sleep
	BLX R1
	B B_SocketFuncListenReadStatus
	NOP
	NOP

; recvfrom() #1
	THUMB
	RSEG	PATCH_SOCKET_FUNC_RECVFROM:CODE:ROOT(2)

	CMP R0, #0
	BNE B_SocketFuncRecvfromExitLoop
	MOVS R0, #1
	LDR R1, =NU_Sleep
	BLX R1
	B B_SocketFuncRecvfromReadStatus
	NOP
	NOP

; recvfrom() #2
	THUMB
	RSEG	PATCH_SOCKET_FUNC_RECVFROM2:CODE:ROOT(2)

	CMP R0, #0
	BNE B_SocketFuncRecvfrom2ExitLoop
	MOVS R0, #1
	LDR R1, =NU_Sleep
	BLX R1
	B B_SocketFuncRecvfrom2ReadStatus
	NOP
	NOP

; send() #1
	THUMB
	RSEG	PATCH_SOCKET_FUNC_SEND:CODE:ROOT(2)

	CMP R0, #0
	BNE B_SocketFuncSendExitLoop
	MOVS R0, #1
	LDR R1, =NU_Sleep
	BLX R1
	B B_SocketFuncSendReadStatus
	NOP
	NOP

; send() #2
	THUMB
	RSEG	PATCH_SOCKET_FUNC_SEND2:CODE:ROOT(2)

	CMP R0, #0
	BNE B_SocketFuncSend2ExitLoop
	MOVS R0, #1
	LDR R1, =NU_Sleep
	BLX R1
	B B_SocketFuncSend2ReadStatus
	NOP
	NOP

; gethostbyname()
	THUMB
	RSEG	PATCH_SOCKET_FUNC_GETHOSTBYNAME:CODE:ROOT(2)

	CMP R0, #0
	BNE B_SocketFuncGethostbynameExitLoop
	MOVS R0, #1
	LDR R1, =NU_Sleep
	BLX R1
	B B_SocketFuncGethostbynameReadStatus
	NOP
	NOP
