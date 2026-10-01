import { readdirSync } from 'node:fs';
import { execFileSync } from 'node:child_process';
import { fileURLToPath } from 'node:url';
import path from 'node:path';

const root = fileURLToPath(new URL('../', import.meta.url));
const samples = path.resolve(process.argv[2] ?? path.join(root, '../lhat/sample'));
const files = readdirSync(samples, { recursive: true }).filter(file => file.endsWith('.lh')).map(file => path.join(samples, file));
if (!files.length) throw new Error(`No .lh samples found in ${samples}`);
const cli = path.join(root, 'node_modules/tree-sitter-cli', process.platform === 'win32' ? 'tree-sitter.exe' : 'tree-sitter');
execFileSync(cli, ['parse', '--quiet', ...files], { cwd: root, stdio: 'inherit' });
console.log(`OK ${files.length} L^ sample files without ERROR or MISSING nodes`);
