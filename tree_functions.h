#ifndef LIST_ON_SIGNS_FUNC_H
#define LIST_ON_SIGNS_FUNC_H
#include <TXLib.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <time.h>

#define QUICK_DUMP(tree, reason, ...) Create_dump_files(tree, __FILE__, __func__, __LINE__, reason, ##__VA_ARGS__)
#define LOADING_BASE_DUMP(tree, buf, pos, reason, progress) Create_dump_files(tree, __FILE__, __func__, __LINE__, reason, DUMP_LOAD, buf, pos, progress)
extern const int DUMMY_VALUE;
const int MAX_STR_SIZE = 1000;

typedef int TreeErr_t;
typedef char* tree_elem_t;

typedef struct Node_t
{
    char* data;
    struct Node_t* left;
    struct Node_t* right;
} Node_t;

typedef struct Tree_t
{
    Node_t* root;
    size_t size;
} Tree_t;

typedef struct
{
    Node_t* node;
    size_t rank;
} LoadNodeInfo;

typedef struct
{
    LoadNodeInfo* nodes;
    size_t capacity;
    size_t size;
    size_t current_rank;
} LoadProgress;


enum
{
    NOERORR = 0,
    ERORRTREENULL,
    ERORRNODENULL,
    ERORRDUMMYALLOC,
    ERORRTREEALLOC,
    ERORRINVALIDROOT,
    ERORRCYCLE,
    ERORRININSERT,
    ERORRFILEOPEN,
    ERORRBUFFER
};

typedef enum
{
    GUESSTYPE    = 0,
    COMPARETYPE  = 1,
    DEFINETYPE   = 2,
    SAVEBASETYPE = 3,
    EXITTYPE     = 4,
    UNKNOWN_TYPE = 5
} AkinatorMode;

typedef enum
{
    YES     = 0,
    NO      = 1,
    UNKNOWN = 2
} UserAnswer;

TreeErr_t TreeCtor (Tree_t** tree);
Node_t* NodeCtor (Tree_t* tree, const char* value);
TreeErr_t TreeDtor (Tree_t* tree);

TreeErr_t PrintNode (const Node_t* node);

TreeErr_t TreeVerify (Tree_t* tree);
TreeErr_t TreeVerifyNode (Tree_t* tree, Node_t* node, Node_t** visited, size_t* counter);

TreeErr_t TreeInsertRoot (Tree_t* tree, const char* question);
TreeErr_t TreeInsertLeft (Tree_t* tree, Node_t* parent, const char* answer);
TreeErr_t TreeInsertRight (Tree_t* tree, Node_t* parent, const char* answer);
Node_t* TreeInsertNode (Tree_t* tree, const char* data);
TreeErr_t DeleteSubtree (Tree_t* tree, Node_t** node);

TreeErr_t GuessObject (Tree_t* tree, Node_t* current_node);
void PrintMenu ();
void AkinatorMenu (Tree_t* tree);
AkinatorMode AkinatorGetMode(void);
UserAnswer AkinatorGetAnswer(void);
TreeErr_t AddNewObject (Tree_t* tree, Node_t* current_node);
int DescribeObject(Node_t* current_node, const char* word);
//void speak(const char* format, ...);
//void speakFlush();

static void SaveTreeToFile (Node_t* node, FILE* file, int depth);
TreeErr_t SaveDatabase (Tree_t* tree, const char* filename);

Node_t* LoadTreeFromFile (Tree_t* tree, const char** buffer, int* pos_in_buffer, LoadProgress* progress, const char* buffer_start);
TreeErr_t LoadDatabase (Tree_t* tree, const char* filename);

const char* GetNodeInfo (Tree_t* tree, Node_t* node, const char* current, int* pos_in_buffer, const char* buffer_start, LoadProgress* progress);
const char* SkipSpaces (const char* str, int* pos_in_buffer);
long getFileSize (FILE* file);

void CtorLoadProgress (LoadProgress* progress);
void AddNodeToLoadProgress (LoadProgress* progress, Node_t* node);
void DtorLoadProgress (LoadProgress* progress);

#endif
