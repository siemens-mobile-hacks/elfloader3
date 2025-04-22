import child_process from "node:child_process";
import path from "node:path";

export const ROOT_DIR = path.resolve(`${import.meta.dirname}/../../`);

export type Ptr89PatternResult = {
	offset: number;
	value: number;
};

export type Ptr89Pattern = {
	results: Ptr89PatternResult[]
};

export type Ptr89 = {
	patterns: Ptr89Pattern[];
};

export function findPattern(model: string, pattern: string) {
	const { stdout } = child_process.spawnSync(`ptr89`, [
		'-f', `${ROOT_DIR}/ff/${model}.bin`,
		'-p', pattern,
		'-J',
	], {});
	return (JSON.parse(stdout.toString()) as Ptr89).patterns[0].results[0];
}

export function findPatternAll(model: string, pattern: string) {
	const { stdout } = child_process.spawnSync(`ptr89`, [
		'-f', `${ROOT_DIR}/ff/${model}.bin`,
		'-p', pattern,
		'-J',
	], {});
	return (JSON.parse(stdout.toString()) as Ptr89).patterns[0].results;
}

export function ucfirst(string: string) {
	return string.charAt(0).toUpperCase() + string.slice(1).toLowerCase();
}
