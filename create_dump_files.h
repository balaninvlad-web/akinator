#ifndef CREATE_LOG_FILE_H
#define CREATE_LOG_FILE_H
#include <TXLib.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#define DUMP_NORMAL   0
#define DUMP_PARSING  1

#include "tree_functions.h"

extern const int MAX_COMMAND_LENGTH;

void Create_dump_files(Tree_t* tree, const char* file, const char* func, int line, const char* reason, ...);
void Create_html_file(Tree_t* tree, int dump_counter, const char* graph_filename, const char* func, const char* formatted_reason, ...);
void Create_head_html (FILE** html_file);
void EscapeHtml(const char* input, char* output, size_t output_size);
void PrintBuffer (FILE* html_file, const char* buffer, size_t position);

void Create_log_file (Tree_t* tree, const char* filename);
void Create_head_log_file (FILE* dot_file, Tree_t* tree);
void Create_graph_node (Tree_t* tree, FILE* dot_file, Node_t* node);
void Make_arrow (Tree_t* tree, FILE* dot_file, Node_t* node);
void Create_picture (void);

#endif
