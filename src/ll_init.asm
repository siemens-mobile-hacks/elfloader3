	RSEG    PATCH_LL_INIT_HOOK:CODE:ROOT
	BL      LL_InitHook

	RSEG	CODE:CODE
LL_InitHook:
	EXTERN  LowLevelInit
	PUSH    { LR }
	BL	LowLevelInit
	BL	OldFuncInOsInit
	POP	{ PC }
