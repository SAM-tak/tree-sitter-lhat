# Tree-sitter for L^ and LTON

Syntax-aware editor grammars for [L^](https://github.com/SAM-tak/lhat).
The Zed integration lives in [lhat-zed-extension](https://github.com/SAM-tak/lhat-zed-extension).

## Features

- Declarations, functions/procedures, expressions, types, definitions, enums,
  typed errors, control flow and pattern matching have named syntax nodes.
- `hat_word` and `hat_suffix` are separate nodes: an editor can dim `^` without
  dimming the word. Bare words such as `if` and `c` remain usable as names.
- Unicode identifiers, quoted names, interpolation, numeric member keys,
  nested block comments, and disabled-code comments are supported.
- LTON shares the rules but starts at table entries rather than statements.
- An external C scanner handles nested comments, string content and
  newline-sensitive calls/returns. It is also used by the LTON grammar.

This is an editor parser, not a replacement for L^'s compiler or `lhatls`.
It deliberately accepts some constructs that the compiler rejects. Type,
scope, effects and LTON evaluation restrictions remain the language server's
responsibility. The corpus and language samples cover the initial structural
grammar; they do not establish complete equivalence with `src/parser.c`.

## Development

Node.js 22+ and a C compiler are required. The CLI is pinned to 0.25.10 and
parsers are generated with ABI 14 for editor compatibility.

```sh
npm ci
npm run generate
npm test
npm run test:incremental
# With the language repository checked out next to this repository:
npm run check:samples
# Or specify its sample directory:
npm run check:samples -- /path/to/lhat/sample
```

Commit generated `src/` and `lton/src/` files together with grammar changes.
Consumers such as Zed fetch a committed revision, not the working tree.
The LTON scanner includes `../../src/scanner.c`, so keep the repository's
directory structure when building the `lton` grammar.

## Layout

| Path | Purpose |
| --- | --- |
| `grammar.js` | Shared L^ rules and L^ start rule |
| `lton/grammar.js` | LTON start rule |
| `src/scanner.c` | Shared external scanner |
| `queries/`, `lton/queries/` | Highlight queries for each grammar |
| `test/corpus/`, `lton/test/corpus/` | Reviewed expected syntax trees |
| `reference/lexical/` | Snapshot of the previous Zed lexical grammar, including the uncommitted work present at migration; historical reference only |

Zed-specific outline, indentation, bracket and text-object queries belong to
the extension. When node names change, update and validate those queries too.

License: Apache-2.0.
