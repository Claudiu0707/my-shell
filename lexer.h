#ifndef LEXER_H
#define LEXER_H

static const char SPACE = ' ';

typedef enum {
    WORD,
    OPERATOR,
} token_type_t;


// TODO: expand operator types for redirections
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
    OP_AMPERSANDGREATER, // '&>'
    OP_UNKNOWN,
} operator_type_t;

typedef enum _CURRENT_STATE {
    NORMAL_STATE = 0,
    IN_QUOTES,
    IN_DQUOTES
} CURRENT_STATE;

typedef struct {
    char* token_buffer;
    int used_buffer_size;
    int total_buffer_size;
    token_type_t type;
    operator_type_t op_type;
    CURRENT_STATE state;
} TOKEN;

struct TOKEN_DS {
    TOKEN* token;
    size_t used_size;
    size_t total_size;
};

#endif