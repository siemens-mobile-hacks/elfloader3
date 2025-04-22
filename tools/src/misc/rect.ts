import fs from 'node:fs';
import path from 'node:path';
import { sprintf } from 'sprintf-js';
import { globSync } from 'glob'
import { findPattern, ROOT_DIR } from "../utils.js";

const ptrs: Record<string, string> = {
    GetMenuHeaderRECT: '&(?? ?? 9F E5 0? ?0 A0 E1 ?? ?? ?? [1111101.] 0? ?0 A0 E1 38 40 B? E8 ?? ?? ?? EA)',
    GetOptionsRECT: '&(?? [00111...] ?? [0001110.] ?? [10010...] 01 [00100...] ?? [01001...] + 0x8)',
    GetInputAreaRECT: '&([........] [0000....] 9f e5 00 [....0000] 8d e5 { f0 b5 [1.......] b0 [00......] 1c [00......] 1c [........] [01001...] [........] [01001...] [........] [10011...] [00......] 1c  } [........] [1101....] 8d e2) + 0x18'
};

const stat: Record<string, Record<string, string[]>> = {};

for (const file of globSync(`${ROOT_DIR}/ff/*.bin`)) {
    const model = path.basename(file).replace(/\.bin$/, '');

    console.log(`${model}`);

    for (const name in ptrs) {
        const ptr = ptrs[name];

        const found = findPattern(model, ptr);
        if (found) {
            const rect = fs.readFileSync(file).subarray(found.value - 0xA0000000, found.value - 0xA0000000 + 8);

            const x1 = rect.readUInt16LE(0);
            const y1 = rect.readUInt16LE(2);
            const x2 = rect.readUInt16LE(4);
            const y2 = rect.readUInt16LE(6);

            const key = [x1, y1, x2, y2].join(', ');
            stat[name] = stat[name] || {};
            stat[name][key] = stat[name][key] || [];
            stat[name][key].push(model);

            console.log(name, sprintf("0x%08X", found.value), [x1, y1, x2, y2]);
        }
    }

    console.log();
}

for (const name in stat) {
    console.log(name);
    for (const coord in stat[name]) {
        console.log("  [" + coord + "]: " + stat[name][coord].join(', '));
    }
}
