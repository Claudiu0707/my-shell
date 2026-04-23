#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>

#define MAX_LENGTH 1024

#define EXIT_SUCCESS 0
#define SUCCESS 0

#define CRITICAL_ERROR -1000
/*
    token_string = contains the token itself
    in_quotes = signals if a special segment is active (e.g. echo "message in quotes is a whole segment")
        - where I use it: in the above e.g., during token separation, If in quotes, I treat ' ' chars as regular chars
*/

struct TOKEN {
    char* token_buffer;
    int used_buffer_size;
    int total_buffer_size;
    bool in_quotes; 
};

struct TOKEN_DS {
    struct TOKEN* token;
    size_t used_size;
    size_t total_size;
};


/*************************************************************************************
*************************************************************************************/
int initialize_token_ds(struct TOKEN_DS* token_ds, size_t size);
int double_token_space(struct TOKEN_DS* token_ds);

int initialize_token(struct TOKEN* token, size_t size);
int double_token_buffer(struct TOKEN* token);
/*************************************************************************************
*************************************************************************************/


/*
    The caller must free the memory allocated for tokens

    @return number of tokens on sucess, -1 on failure
*/
int msh_tokenizer(struct TOKEN_DS* token_ds, char* line, size_t line_length) {
    int function_result = SUCCESS;

    int cur_char_index = 0;
    char current_char;
    int found_tokens_count = 0;
    int token_index = 0;
    bool has_token = false;
    
    struct TOKEN* current_token = NULL;
    // Initialize with default size 8
    if((function_result = initialize_token_ds(token_ds, 8)) != SUCCESS) 
        goto cleanup;

    while (cur_char_index < line_length) {
        current_char = line[cur_char_index];
        // Consume non-token white spaces
        if (!has_token && current_char == ' ') {
            cur_char_index++;
            continue;
        }
        // Finished extracting a token
        if (has_token && current_char == ' ' && !current_token->in_quotes) {
            has_token = false;
            current_token = false;
            token_index++;
            cur_char_index++;
            continue;
        } 
        // Detected a token
        if (!has_token && current_char != ' ') {
            has_token = true;
            token_ds->used_size++;
            if (token_ds->used_size > token_ds->total_size) {
                if ((function_result = double_token_space(token_ds)) != SUCCESS) 
                    goto cleanup;
            }
            
            current_token = &token_ds->token[token_index];

            if ((function_result = initialize_token(current_token, MAX_LENGTH)) != SUCCESS)
                goto cleanup;
        }

        // Process the token
        // TODO allow escaping characters
        if (has_token && current_char != ' ') {
            if (current_char == '"') {
                if (current_token->in_quotes) 
                    current_token->in_quotes = false;
                else 
                    current_token->in_quotes = true;
                    continue;
            }

            if (current_token->used_buffer_size > current_token->total_buffer_size) {
                if ((function_result = double_token_buffer(current_token)) != SUCCESS) 
                    goto cleanup;
            }

            current_token->token_buffer[current_token->used_buffer_size] = current_char;
            current_token->used_buffer_size++;
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
    while ((nread = getline(&line, &size, stdin)) != -1) {
        msh_tokenizer(&token_ds, line, size);
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

    token_ds->token = (struct TOKEN* ) calloc(token_ds->total_size, sizeof(struct TOKEN));
    if (token_ds->token == NULL) 
        return CRITICAL_ERROR;
    return SUCCESS;
}

int double_token_space(struct TOKEN_DS* token_ds) {
    struct TOKEN* new_tokens = NULL;
    new_tokens = (struct TOKEN* ) realloc(token_ds->token, token_ds->total_size * 2);
    if (new_tokens ==  NULL) 
        return CRITICAL_ERROR;
    token_ds->total_size *= 2;
    free(token_ds->token);
    token_ds->token = new_tokens;
    return SUCCESS;
}

int initialize_token(struct TOKEN* token, size_t size) {
    token->token_buffer = (char*) calloc(size, sizeof(char));
    if (token->token_buffer == NULL) 
        return CRITICAL_ERROR;

    token->used_buffer_size = 0;
    token->total_buffer_size = size;
    return SUCCESS;
}

int double_token_buffer(struct TOKEN* token) {
    char* new_buffer = NULL;
    new_buffer = realloc(token->token_buffer, token->total_buffer_size * 2);
    if (new_buffer == NULL) 
        return CRITICAL_ERROR;

    token->total_buffer_size *= 2;
    free(token->token_buffer);
    token->token_buffer = new_buffer;
    return SUCCESS;
}

int free_tokens(struct TOKEN_DS tokens) {

}