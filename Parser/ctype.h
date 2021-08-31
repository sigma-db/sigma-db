#ifndef CTYPE_H
#define CTYPE_H

extern int character_class_map[128];

#define CC_OTHER 0
#define CC_NULL 1
#define CC_SPACE 2
#define CC_QUOTE 4
#define CC_BRACKET 8
#define CC_PUNCT 16
#define CC_DIGIT 32
#define CC_ALPHA 64

#define character_class(c) character_class_map[c]
#define isclass(c, cc_mask) ((character_class(c) & cc_mask) != 0)

#define isnull(c) (c == 0)
#define isspace(c) isclass(c, CC_SPACE)
#define isquote(c) isclass(c, CC_QUOTE)
#define isbracket(c) isclass(c, CC_BRACKET)
#define ispunct(c) isclass(c, CC_PUNCT)
#define isdigit(c) isclass(c, CC_DIGIT)
#define isalpha(c) isclass(c, CC_ALPHA)
#define isalnum(c) (isalpha(c) | isdigit(c))

#endif // !CTYPE_H
