#include <stdlib.h>
#include <stdio.h>

#include "token_stream.h"

int main(int argc, char *argv[])
{
    const char input[] = "\"Jack Walls\" 'Y' 957"; // "Employee(0, \"Jack Walls\", 8360, 1)";

    struct token_stream *ts;
    int error = token_stream_create(&ts, input, sizeof input);
    if (error) {
        return EXIT_FAILURE;
    }

    while (token_stream_has_more(ts)) {
        struct token tok = token_stream_next(ts);
        switch (tok.type) {
        case TK_LITERAL_INT:
        case TK_LITERAL_STRING:
            char *str = malloc(tok.length + 1);
            if (str == NULL) break;
            for (size_t i = 0; i < tok.length; i++) {
                str[i] = tok.reference[i];
            }
            str[tok.length] = 0;
            printf("%d - %s\r\n", tok.type, str);
            break;
        
        case TK_LITERAL_CHAR:
            printf("%d - %c\r\n", tok.type, tok.value[0]);
            break;

        case TK_SPACE:
            continue;
        }
    }

    token_stream_destroy(ts);

    return EXIT_SUCCESS;
}
