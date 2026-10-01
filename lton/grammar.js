const lhat = require('../grammar');

module.exports = grammar(lhat, {
  name: 'lton',
  rules: {
    source_file: $ => seq(optional(seq($._table_entry, repeat(seq(',', $._table_entry)))), optional(',')),
  },
});
