import fs from 'node:fs';
import path from 'node:path';
import { sprintf } from 'sprintf-js';
import { globSync } from 'glob'
import { findPattern, ROOT_DIR } from "../utils.js";

const newData = Buffer.from("0880A0E1", "hex");

for (const file of globSync(`${ROOT_DIR}/ff/*.bin`)) {
	const model = path.basename(file).replace(/\.bin$/, '');
	const found = findPattern(model, "0040A0E1????????0400A0E1????????????????0120A0E301 +4");
	if (found) {
		const oldData = fs.readFileSync(file).subarray(found.offset, found.offset + newData.length);
		console.log(`; ${model}`);
		console.log(sprintf("%07X: %s %s", found.offset, oldData.toString('hex').toUpperCase(), newData.toString('hex').toUpperCase()));
		console.log();
	}
}
