#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include "lexer.h"

#define MAX_LENGTH 1024

#define EXIT_SUCCESS 0
#define SUCCESS 0
#define FAILURE -1


/*************************************************************************************
*************************************************************************************/

bool should_increment_token_ds(struct TOKEN_DS* token_ds) {
    return token_ds->used_size > token_ds->total_size;
}

bool should_increment_token_buffer(TOKEN* current_token) {
    return current_token->used_buffer_size > current_token->total_buffer_size;
}

/*************************************************************************************
*************************************************************************************/
int initialize_token_ds(struct TOKEN_DS* token_ds, size_t size);
int increment_token_space(struct TOKEN_DS* token_ds);

int initialize_token(TOKEN* token, size_t size);
int double_token_buffer( TOKEN* token);

void display_token_ds(struct TOKEN_DS* token_ds);

operator_type_t get_operator_from_char(char* a, char* b) {
    if (b != NULL) {
        if (*a == '|' && *b == '|') 
            return OP_OR;
        if (*a == '&' && *b == '&')
            return OP_AND;
        if (*a == '>' && *b == '>')
            return OP_DGREATER_THAN;
        if (*a == '&' && *b == '>')
            return OP_AMPERSANDGREATER;
    }
    switch (*a) {
        case '|':
            return OP_PIPE;
        case '&':
            return OP_AMPERSAND;
        case ';':
            return OP_SEMICOLON;
        case '<':
            return OP_LOWER_THAN;
        case '>':
            return OP_GREATER_THAN;
        default:
            return OP_GENERAL;
    }
}
/*************************************************************************************
*************************************************************************************/

/*
    The caller must free the memory allocated for tokens

    @return number of tokens on sucess, -1 on failure
*/

const int msh_lexer(struct TOKEN_DS* token_ds, char* line, size_t line_length) {
    int function_result = SUCCESS;

    unsigned int current_char_index = 0;
    unsigned int current_token_index = 0;
    bool has_token = false;

    TOKEN* current_token = NULL;
    if ((function_result = initialize_token_ds(token_ds, 4)) != SUCCESS) {
        goto cleanup;
    }

    while (current_char_index < line_length) {
        char current_char = line[current_char_index];
        char next_char;

        operator_type_t type;
        if (current_char_index + 1 < line_length) {
            next_char = line[current_char_index+1];
            type = get_operator_from_char(&current_char, &next_char);
        }
        else type = get_operator_from_char(&current_char, NULL);
        
        // Consume non-token white spaces
        if (has_token == false && current_char == SPACE) {
            current_char_index++;
            continue;
        }

        if (type == OP_GENERAL || (type != OP_GENERAL && current_token != NULL && current_token->state != NORMAL_STATE)) {
            // Start of a token was detected
            if (has_token == false && current_char != SPACE) {
                has_token = true;
                token_ds->used_size++;
                if (should_increment_token_ds(token_ds)) {
                    if ((function_result = increment_token_space(token_ds)) != SUCCESS) {
                        goto cleanup;
                    }
                }
                current_token = &token_ds->token[current_token_index];
                if ((function_result = initialize_token(current_token, MAX_LENGTH)) != SUCCESS) {
                    goto cleanup;
                }
            }

            // End of a token was reached
            if (has_token == true && current_char == SPACE && 
                (current_token->state != IN_DQUOTES && current_token->state != IN_QUOTES) )
            {
                has_token = false;
                current_token->type = WORD;
                current_token->op_type = type;
                current_token = NULL;
                current_token_index++;
                current_char_index++;
                continue;
            }

            // Process the token
            if (has_token == true) {
                switch(current_char) {
                    case '\"':
                        if (current_token->state == IN_DQUOTES) {
                            current_token->state = NORMAL_STATE;
                            current_char_index++;
                            continue;
                        } else if (current_token->state == NORMAL_STATE) {
                            current_token->state = IN_DQUOTES; 
                            current_char_index++;
                            continue;
                        } 
                        break;
                    case '\'':
                        if (current_token->state == IN_QUOTES) {
                            current_token->state = NORMAL_STATE; 
                            current_char_index++;
                            continue;
                        } else if (current_token->state == NORMAL_STATE) {
                            current_token->state = IN_QUOTES; 
                            current_char_index++;
                            continue;
                        } 
                        break;
                    default:
                        break;
                }

                if (should_increment_token_buffer(current_token)) {
                    if ((function_result = double_token_buffer(current_token)) != SUCCESS) {
                        goto cleanup;
                    }
                }
                current_token->token_buffer[current_token->used_buffer_size] = current_char;
                current_token->used_buffer_size++;
                current_char_index++;
            }
         } else {
                if (has_token && current_token != NULL && current_token->state == NORMAL_STATE) {
                    // Finish processing the word token
                    has_token = false;
                    current_token->type = WORD;
                    current_token->op_type = type;
                    current_token = NULL;
                    current_token_index++;
                    current_char_index++;
                }
                
                if (has_token == false) {
                    // The operator itself is a special token
                    token_ds->used_size++;
                    if (should_increment_token_ds(token_ds) == true) {
                        if ((function_result = increment_token_space(token_ds)) != SUCCESS) {
                            goto cleanup;
                        }
                    }
                    current_token = &token_ds->token[current_token_index++];
                    if ((function_result = initialize_token(current_token, MAX_LENGTH)) != SUCCESS) {
                        goto cleanup;
                    }

                    if (type == OP_OR || type == OP_AND || type == OP_DGREATER_THAN) {
                        // Special operator with 2 chars
                        if (should_increment_token_buffer(current_token)) {
                            if ((function_result = double_token_buffer(current_token)) != SUCCESS) {
                                goto cleanup;
                            }
                        }
                        current_token->token_buffer[current_token->used_buffer_size] = current_char;
                        current_token->used_buffer_size++;
                        current_char_index++;
                        if (should_increment_token_buffer(current_token)) {
                            if ((function_result = double_token_buffer(current_token)) != SUCCESS) {
                                goto cleanup;
                            }
                        }
                        current_token->token_buffer[current_token->used_buffer_size] = next_char;
                        current_token->used_buffer_size++;
                        current_char_index++;
                    } else {
                        // Special operator with 1 char
                        if (should_increment_token_buffer(current_token)) {
                            if ((function_result = double_token_buffer(current_token)) != SUCCESS) {
                                goto cleanup;
                            }
                        }
                        current_token->token_buffer[current_token->used_buffer_size] = current_char;
                        current_token->used_buffer_size++;
                        current_char_index++;
                    }
                }

                current_token->type = OPERATOR;
                current_token->op_type = type;
        }
    }
    cleanup:
    return function_result;
}


int msh_loop() {
    char* line = NULL;
    size_t size = 0;
    __ssize_t nread = 0;
    errno = 0;

    struct TOKEN_DS token_ds;
    while (true) {
        printf("> ");
        if ((nread = getline(&line, &size, stdin)) == -1) {
            exit(1);
        }
        line[nread-1] = '\0';
        msh_lexer(&token_ds, line, nread-1);
        printf("FULL INPUT: %s\n", line);
        display_token_ds(&token_ds);
        free(line);
        size = 0;
    }

    if (line) free(line);
    return 0;
}

int main(void) {

    msh_loop();
    
    return EXIT_SUCCESS;
}


/********************************************************
 ***************        UTILITIES          **************
*********************************************************/
int initialize_token_ds(struct TOKEN_DS* token_ds, size_t size) {
    token_ds->total_size = size;
    token_ds->used_size = 0;
    token_ds->token = NULL;

    token_ds->token = (TOKEN*) calloc(token_ds->total_size, sizeof(TOKEN));
    if (token_ds->token == NULL) 
        return FAILURE;
    return SUCCESS;
}

int increment_token_space(struct TOKEN_DS* token_ds) {
    TOKEN* new_tokens = NULL;
    int new_size = token_ds->total_size + 4;
    new_tokens = (TOKEN* ) realloc(token_ds->token, sizeof(TOKEN) * new_size);
    if (new_tokens ==  NULL) 
        return FAILURE;
    token_ds->total_size += 4;
    token_ds->token = new_tokens;
    return SUCCESS;
}

int initialize_token(TOKEN* token, size_t size) {
    token->token_buffer = (char*) calloc(size, sizeof(char));
    if (token->token_buffer == NULL) {
        return FAILURE;
    }

    token->used_buffer_size = 0;
    token->total_buffer_size = size;
    token->state = NORMAL_STATE;
    return SUCCESS;
}

int double_token_buffer(TOKEN* token) {
    char* new_buffer = NULL;
    new_buffer = realloc(token->token_buffer, token->total_buffer_size * 2);
    if (new_buffer == NULL) 
        return FAILURE;

    token->total_buffer_size *= 2;
    token->token_buffer = new_buffer;
    return SUCCESS;
}


void display_token_ds(struct TOKEN_DS* token_ds) {
    printf("TOKENIZED COMMAND:\n");
    for (int i = 0; i < token_ds->used_size; i++) {
        printf("Token: %s | Type: ", token_ds->token[i].token_buffer);
        if (token_ds->token[i].type == WORD) printf("WORD\n");
        else {
            printf("OPERATOR: ");
            switch(token_ds->token[i].op_type) {
                case OP_LOWER_THAN:
                    printf("<\n");
                    break;  
                case OP_GREATER_THAN:
                    printf(">\n");
                    break;
                case OP_DGREATER_THAN:
                    printf(">>\n");
                    break;
                case OP_AMPERSANDGREATER:
                    printf("&>\n");
                    break;
                default:
                    printf("other\n");
            }
        }
    }
}

