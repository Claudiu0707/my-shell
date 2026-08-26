#ifndef LEXER_H
#define LEXER_H

static const char SPACE = ' ';

typedef enum {
    WORD,
    OPERATOR,
} token_type_t;

typedef enum {
    OP_GENERAL,       // any other operator
    OP_PIPE,          // '|'
    OP_AMPERSAND,     // '&'
    OP_SEMICOLON,     // ';'
    OP_LOWER_THAN,    // '<'
    OP_GREATER_THAN,  // '>'
    OP_OR,            // '||'
    OP_AND,           // '&&'
    OP_DGREATER_THAN, // '>>'
    OP_UNKNOWN,
} operator_type_t;

typedef enum _CURRENT_STATE {
    NORMAL_STATE = 0,
    IN_QUOTES,
    IN_DQUOTES
} CURRENT_STATE;

struct TOKEN {
    char* token_buffer;
    int used_buffer_size;
    int total_buffer_size;
    token_type_t type;
    CURRENT_STATE state;
};

struct TOKEN_DS {
    struct TOKEN* token;
    size_t used_size;
    size_t total_size;
};


#endif