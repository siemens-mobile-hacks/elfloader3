; Fix case when IDLE accidentally restarts on SIM error
#ifdef ELKA
	THUMB
	RSEG	PATCH_NO_IDLE_RESTART:CODE:ROOT(2)
	; Remove BL IsIdleUiOnTop
	NOP
	NOP
	; Remove CMP R0, #0
	NOP
	; Remove BNE LAB_XXXXX
	NOP
	; Remove BLX DoIDLE
	NOP
	NOP
	; Remove BLX GetIDLECSMID
	NOP
	NOP
	; Remove BLX CloseCSM
	NOP
	NOP
#else
	THUMB
	RSEG	PATCH_NO_IDLE_RESTART:CODE:ROOT(2)
	; Remove BLX DoIDLE
	CMP R0, #2
    BEQ B_SkipSimErrorPopup
	; Remove BLX GetIDLECSMID
	NOP
	NOP
	; Remove BLX CloseCSM
	NOP
	NOP
#endif
