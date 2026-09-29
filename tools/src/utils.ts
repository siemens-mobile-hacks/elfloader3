import fs from "node:fs";
import path from "node:path";
import { Ptr89 } from "@sie-js/ptr89";

export const ROOT_DIR = path.resolve(`${import.meta.dirname}/../../`);

const ptr89 = new Ptr89();
let openedModel: string | undefined;

async function openModel(model: string) {
	if (openedModel === model)
		return;

	ptr89.close();
	openedModel = undefined;
	await ptr89.open(fs.readFileSync(`${ROOT_DIR}/ff/${model}.bin`));
	openedModel = model;
}

export async function findPattern(model: string, pattern: string) {
	await openModel(model);
	return ptr89.find(pattern, 1).results[0];
}

export async function findPatternAll(model: string, pattern: string) {
	await openModel(model);
	return ptr89.find(pattern).results;
}

export function ucfirst(string: string) {
	return string.charAt(0).toUpperCase() + string.slice(1).toLowerCase();
}
