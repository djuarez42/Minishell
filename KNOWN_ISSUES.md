Fixed: backslash escaping of '$' was incorrectly allowing variable expansion. Lexer now marks odd backslash+`$` sequences as literal so `\$VAR` yields `$VAR` (literal) instead of expanding.

Fixed: when building tokens from mixed quoted/unquoted fragments, leading spaces coming from quoted fragments were being stripped by IFS splitting (example: `echo "  "Hello "World "` lost the two leading spaces). The parser now performs fragment-aware splitting that preserves whitespace originating from quoted fragments.

