export const UNUSED_RAM_NSG = `&(?? ?? 9F E5 [.000....] ?? 8? E0 [.000....] ?? 8? E0 ?? ?? 8? E2 45 ?0 A0 E3 9? 0? 0? E0 [.000....] ?? 8? E0 [.000....] ?? 8? E0 ?? ?? 8? E2 49 ?0 A0 E3 9? 0? 0? E0 ?? ?? 8? E2 [.000....] ?? 8? E0 ?? ?? 8? E2 [.000....] ?? 8? E0 B? ?? D? E1 ?? ?? 8? E2 B? ?? C? E1 ?? ?? A0 E3 ?? 1? C? E5 ?? 0? 9? E5 ?? ?? ?? EB ?? ?? 8? E5 B? 0? D? E1 00 10 A0 E3 ?? ?? ?? EB ?? ?? 9? E5 ?? ?? ?? EB 0? ?0 A0 E1 B? 0? D? E1 01 10 A0 E3 ?? ?? ?? EB)`;

export const LL_INIT_NSG = `[........] [........] [........] eb [........] [........] [........] eb [........] [........] [........] eb [........] [........] [........] eb [........] [........] [........] eb [........] [........] [........] eb [........] [........] [........] eb [........] [........] [........] eb [........] [........] 9f e5 [........] [........] [1001....] e5 [........] [........] 9f e5 [........] [........] [1001....] e5 [........] [........] [........] [1111101.] [........] [........] 8f e2 [........] [........] a0 e3 [........] [........] [........] [1111101.] 10 40 [1011....] e8 [........] [........] [........] ea`;

export const LL_INIT_OLD_FUNC_NSG = `f0 41 [0010....] e9 [........] [........] 1f e5 [........] [........] 1f e5 [.000....] [........] a0 e1 [.010....] [........] b0 e1 [........] [........] [........] 0a [0000....] [....0000] a0 e1 02 00 00 ef [........] [........] a0 e3 [........] [........] a0 e3 [........] [........] 1f e5 01 00 a0 e3 [........] [........] [........] eb [0000....] [....0000] a0 e1`;

export const NO_RESTART_ON_SIM_ERROR_ELKA = `?? [111100..] ?? [11.1....] 00 28 ?? D1 ?? [111100..] ?? [11.0....] ?? [111100..] ?? [11.0....] ?? [111100..] ?? [11.0....] 03 20 ?? [111100..] ?? [11.1....] ?? [111100..] ?? [11.1....] [.....000] [01100...] ?? [11100...]`;

export const NO_RESTART_ON_SIM_ERROR_NSG = `?? [111100..] ?? [11.0....] ?? [111100..] ?? [11.0....] ?? [111100..] ?? [11.0....] 03 20 ?? [111100..] ?? [11.1....] ?? [111100..] ?? [11.1....] ?? [01100...] 02 [00100...]`;

export const NO_RESTART_ON_SIM_ERROR_ELKA_V2 = `?? [111100..] ?? [11.0....] ?? [111100..] ?? [11.0....] ?? [111100..] ?? [11.0....] 03 20 ?? [111100..] ?? [11.0....] ?? [111100..] ?? [11.0....] ?? [01100...]`;

export const SOCKETS_ERROR_EBSD_CHECK = `[........] 98 [........] 28 ?? D1 LDR[ 7f1665627364e3 ] [00......] 1C 01 20 ?? [111100..] ?? [11.0....] 00 28 ?? D0 ?? DF [00......] 1C [00......] 1C 03 [00100...] 01 20 ?? [111100..] ?? [11.0....] ?? [11100...] + 2`;

export const SOCKETS_ERROR_EBSD_CHECK_V2 = `[........] 98 00 28 [........] d1 LDR[ 7f1665627364e3 ] [00......] 1c 01 20 [........] [111100..] [........] [11.0....] 00 28 [........] d0 02 df 03 [00100...] [00......] 1c 01 20 [........] [01001...] [........] [111100..] [........] [11.0....] [........] [11100...]  + 2`;

export const NU_SLEEP_FUNC_NSG = `000050E3E0109F150010911500005113`;


/*

; Fix USB Vendor ID
0D25A8C: F5110000 7B060000

; Fix USB Product ID (when "2nd COM" is on)
0D25A90: 04100000 03230000

; Fix USB Product ID (when "2nd COM" is off)
0D259FC: 00 03
0D25952: 00 03

*/