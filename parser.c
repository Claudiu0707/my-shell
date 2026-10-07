#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <stdbool.h>
#include "parser.h"

AST_NODE* parse_line(PARSE_CONTEXT* ctx);
AST_NODE* parse_andor(PARSE_CONTEXT* ctx);
AST_NODE* parse_pipeline(PARSE_CONTEXT* ctx);
AST_NODE* parse_command(PARSE_CONTEXT* ctx);

int allocate_command_space(SIMPLE_COMMAND_T* command);
int reallocate_command_space(SIMPLE_COMMAND_T* command);
int allocate_redirs_space(SIMPLE_COMMAND_T* command);
int reallocate_redirs_space(SIMPLE_COMMAND_T* command);



inline int advance_ctx(PARSE_CONTEXT* ctx);

bool op_is_redir(operator_type_t type) {
    switch(type) {
        case OP_LOWER_THAN:
            return true;
        case OP_GREATER_THAN:
            return true;
        case OP_DGREATER_THAN:
            return true;
        case OP_AMPERSANDGREATER:
            return true;
        default:
            return false;;
    }
}

inline int advance_ctx(PARSE_CONTEXT* ctx) {
    if (ctx->position < ctx->used - 1) {
        ctx->position++;
        return 0;
    }
    return -1;
}

AST_NODE* parse_line(PARSE_CONTEXT* ctx) {
    if (!ctx) return NULL;

    if (ctx->tokens[ctx->position]) {
        return parse_andor(ctx);
    }

    return NULL;
}

// How to process multiple AND/OR
AST_NODE* parse_andor(PARSE_CONTEXT* ctx) {
    if (!ctx) return NULL;

    AST_NODE* node = NULL;

    // Check if the token is an AND/OR operator
    if (ctx->tokens[ctx->position]->op_type == OP_AND || ctx->tokens[ctx->position]->op_type == OP_OR) {
        node = (AST_NODE*) calloc(1, sizeof(AST_NODE));
    
        // Process the AND/OR
    
        // Advance parse context further
    }
    
    if (node) {
        node->left = parse_pipeline(ctx);


        node->right = parse_pipeline(ctx);
        return node;
    }
        
    return parse_pipeline(ctx);
}

AST_NODE* parse_pipeline(PARSE_CONTEXT* ctx) {
    if (!ctx) return NULL;

    AST_NODE* node = NULL;

    // Check if the token is an AND/OR operator
    if (ctx->tokens[ctx->position]->op_type == OP_PIPE) {
        node = (AST_NODE*) calloc(1, sizeof(AST_NODE));
    
        // Process the pipeline
    
        // Advance parse context further
    }

    if (node) {
        node->left = parse_command(ctx);


        node->right = parse_command(ctx);
        return node;
    }
    
    return parse_command(ctx);
}


AST_NODE* parse_command(PARSE_CONTEXT* ctx) {
    if (!ctx) return NULL;
    AST_NODE* node = NULL;
    SIMPLE_COMMAND_T command;
    while (ctx->tokens[ctx->position]) {
        if (!node) {
            node = (AST_NODE*) calloc(1, sizeof(AST_NODE));
            node->node_type = COMMAND;
        }
        if (ctx->tokens[ctx->position]->type == WORD) {
            if (node->COMMAND.command == NULL) {
                node->COMMAND.command = (SIMPLE_COMMAND_T*) calloc(1, sizeof(SIMPLE_COMMAND_T));
            
                allocate_command_space(node->COMMAND.command);
            }

            // TODO: Before adding a token, check available space and if necessary reallocate
            node->COMMAND.command->words[node->COMMAND.command->used] = (*ctx->tokens)[ctx->position];
            node->COMMAND.command->used++;
            advance_ctx(ctx);
        } else {
            if (op_is_redir(ctx->tokens[ctx->position]->op_type)) {
                // allocate/reallocate space for redir type
                // identify next token for file name
                // maybe some special cases for other type of redirs
            } else {
                // reached an operator which is not redir
                // most probably will stop building this node
                break;
            }
        }
    }

    if (node) {
        node->left = NULL;
        node->right = NULL;
    }

    return node;
}




// Utilities
int allocate_command_space(SIMPLE_COMMAND_T* command) {
    command->size = 4;
    command->used = 0;
    command->words = (TOKEN*) calloc(command->size, sizeof(TOKEN));
    if (command->words == NULL) return -1;
    return 0;
}

int reallocate_command_space(SIMPLE_COMMAND_T* command) {
    int size_increase = 4;
    TOKEN* temp_words = (TOKEN*) realloc(command->words, (command->size + size_increase) * sizeof(TOKEN));
    if (temp_words == NULL) return -1;
    command->size += size_increase;
    command->words = temp_words;
    return 0;
}

int allocate_redirs_space(SIMPLE_COMMAND_T* command) {
    command->redirs_size = 4;
    command->redirs_used = 0;
    command->redirs = (REDIRECT*) calloc(command->redirs_size, sizeof(REDIRECT));
    if (command->redirs == NULL) return -1;
    return 0;
}

int reallocate_redirs_space(SIMPLE_COMMAND_T* command) {
    int size_increase = 2;
    REDIRECT* temp_redirs = (REDIRECT*) realloc(command->redirs, (command->redirs_size + sizeof(REDIRECT)));
    if (temp_redirs == NULL) return -1;
    command->redirs_size += size_increase;
    command->redirs = temp_redirs;
    return 0;
}