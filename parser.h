#ifndef PARSER_H
#define PARSER_H
#include "lexer.h"

typedef enum {
    COMMAND,
    PIPELINE,
    ANDOR,
} node_type_t;

typedef struct {
    TOKEN** tokens;
    int total_size;
    int used;
    int position;
} PARSE_CONTEXT;

typedef enum {
    REDIR_APPEND,// >>
    REDIR_OUTPUT,// >
    REDIR_INPUT, // <
    REDIR_STDOUTERR, // &>
} REDIR_TYPE;

typedef struct {
    REDIR_TYPE redir_type;
    char* filename;
    int fd;
} REDIRECT;

typedef struct {
    TOKEN* words;
    int size;
    int used;
                
    REDIRECT* redirs;
    int redirs_size;
    int redirs_used;
} SIMPLE_COMMAND_T;


typedef struct {
    AST_NODE* left;
    AST_NODE* right;

    node_type_t node_type;
    union {
        struct {
            SIMPLE_COMMAND_T* command;
        } COMMAND;
        struct {

        } PIPELINE;
        struct {

        } ANDOR;
    };
} AST_NODE;


#endif