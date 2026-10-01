#include "tree_sitter/parser.h"
#include <wctype.h>

enum Token { BLOCK_COMMENT, CALL_OPEN, OPTIONAL_CALL_OPEN, INDEX_OPEN, OPTIONAL_INDEX_OPEN, LINE_END, STRING_CONTENT, INTERPOLATED_CONTENT };

void *tree_sitter_lhat_external_scanner_create(void) { return NULL; }
void tree_sitter_lhat_external_scanner_destroy(void *payload) { (void)payload; }
unsigned tree_sitter_lhat_external_scanner_serialize(void *payload, char *buffer) {
    (void)payload; (void)buffer; return 0;
}
void tree_sitter_lhat_external_scanner_deserialize(void *payload, const char *buffer, unsigned length) {
    (void)payload; (void)buffer; (void)length;
}

bool tree_sitter_lhat_external_scanner_scan(void *payload, TSLexer *lexer, const bool *valid) {
    (void)payload;
    if (valid[STRING_CONTENT] && valid[INTERPOLATED_CONTENT]) return false;
    if (valid[STRING_CONTENT] || valid[INTERPOLATED_CONTENT]) {
        bool consumed = false;
        while (!lexer->eof(lexer) && lexer->lookahead != '"' && lexer->lookahead != '\\' &&
               !(valid[INTERPOLATED_CONTENT] && (lexer->lookahead == '{' || lexer->lookahead == '}'))) {
            lexer->advance(lexer, false);
            consumed = true;
        }
        if (!consumed) return false;
        lexer->mark_end(lexer);
        lexer->result_symbol = valid[STRING_CONTENT] ? STRING_CONTENT : INTERPOLATED_CONTENT;
        return true;
    }
    bool newline = false;
    lexer->mark_end(lexer);
    while (iswspace(lexer->lookahead)) {
        newline |= lexer->lookahead == '\n' || lexer->lookahead == '\r';
        lexer->advance(lexer, true);
    }
    // A bare return/yield ends at a newline or a closing statement boundary.
    // Do not manufacture empty tokens during error recovery.
    if (valid[LINE_END] && !(valid[CALL_OPEN] && valid[OPTIONAL_CALL_OPEN] && valid[INDEX_OPEN] && valid[OPTIONAL_INDEX_OPEN]) &&
        (newline || lexer->eof(lexer) || lexer->lookahead == '}' || lexer->lookahead == ';')) {
        lexer->result_symbol = LINE_END;
        return true;
    }
    if (valid[BLOCK_COMMENT] && lexer->lookahead == '#') {
        lexer->advance(lexer, false);
        if (lexer->lookahead != '[') return false;
        lexer->advance(lexer, false);
        unsigned depth = 1;
        while (depth && !lexer->eof(lexer)) {
            int32_t ch = lexer->lookahead;
            lexer->advance(lexer, false);
            if (ch == '#' && lexer->lookahead == '[') {
                lexer->advance(lexer, false);
                ++depth;
            } else if (ch == ']' && lexer->lookahead == '#') {
                lexer->advance(lexer, false);
                --depth;
            }
        }
        lexer->mark_end(lexer);
        lexer->result_symbol = BLOCK_COMMENT;
        return true;
    }
    if (newline) return false;
    bool optional = lexer->lookahead == '?';
    if (optional) lexer->advance(lexer, false);
    enum Token token;
    if (lexer->lookahead == '(') token = optional ? OPTIONAL_CALL_OPEN : CALL_OPEN;
    else if (lexer->lookahead == '[') token = optional ? OPTIONAL_INDEX_OPEN : INDEX_OPEN;
    else return false;
    if (!valid[token]) return false;
    lexer->advance(lexer, false);
    lexer->mark_end(lexer);
    lexer->result_symbol = token;
    return true;
}
