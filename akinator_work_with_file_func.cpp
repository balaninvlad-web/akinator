#include "tree_functions.h"
#include "create_dump_files.h"

TreeErr_t SaveTreeToFile(Node_t* node, FILE* file)
{
    // TODO: assert

    if (!node)
    {
        fprintf(file, "nil");
        return NOERORR;
    }

    fprintf(file, "(\"");

    fwrite(node->data, 1, strlen(node->data), file);

    fprintf(file, "\" ");

    SaveTreeToFile(node->left, file);
    fprintf(file, " ");
    SaveTreeToFile(node->right, file);
    fprintf(file, ")");

    return NOERORR;
}

TreeErr_t SaveDatabase(Tree_t* tree, const char* filename)
{
    if (!tree) return ERORRNODENULL;

    FILE* file = fopen(filename, "w");
    if (!file) return ERORRFILEOPEN;

    SaveTreeToFile(tree->root, file);
    fclose(file);

    printf("Database saved to %s\n", filename);
    return NOERORR;
}

const char* SkipSpaces (const char* str, int* pos_in_buffer)
{
    assert(str);
    assert(pos_in_buffer);

    while (*str && isspace((unsigned char)*str))
    {
        str++;
        (*pos_in_buffer)++;
    }

    return str;
}

const char* ReadQuotedString (const char* current, char* output, int max_len)
{
    // TODO: assert

    if (*current != '"') return current;

    current++;
    int i = 0;

    while (*current && *current != '"' && i < max_len - 1)
    {
        output[i++] = *current++;
    }
    output[i] = '\0';

    if (*current == '"')
        current++;

    return current;
}

// TODO: third argument
Node_t* LoadTreeFromFile(Tree_t* tree, const char** buffer, int* pos_in_buffer, LoadProgress* progress, const char* buffer_start)
{
    assert (tree);
    assert (buffer);
    assert (pos_in_buffer);

    progress->current_rank++;

    #ifdef DEBUG
        printf ("DEBUG: LoadTreeFromFile called, buffer starts with: '%.10s...'\n", *buffer);
    #endif

    const char* current = SkipSpaces (*buffer, pos_in_buffer);

    #ifdef DEBUG
        printf ("DEBUG: After SkipSpaces: '%.10s...', pos_in_buffer: %d\n", current, *pos_in_buffer);

        LOADING_BASE_DUMP (tree, buffer_start, *pos_in_buffer, "Start making node", progress);
    #endif

    if (*current == '(')
    {
        #ifdef DEBUG
            printf ("DEBUG: Found opening bracket, starting new node\n");
        #endif

        current = SkipSpaces(current + 1, pos_in_buffer + 1);

        #ifdef DEBUG
            printf ("DEBUG: After skipping spaces after '(': '%.10s...', pos_in_buffer: %d\n", current, *pos_in_buffer);
        #endif

        Node_t* node = NodeCtor(tree, "");

        if (node && progress) AddNodeToLoadProgress(progress, node);

        if (!node)
        {
            printf ("DEBUG: ERROR: NodeCtor failed!\n");
            return NULL;
        }

        #ifdef DEBUG
            printf ("DEBUG: Node created successfully at %p\n", (void*)node);
            LOADING_BASE_DUMP (tree, buffer_start, *pos_in_buffer, "Node created", progress);
        #endif

        // TODO: GetNodeInfo

        if (*current == '"')
        {
            #ifdef DEBUG
                printf ("DEBUG: Found opening quote, reading data\n");
            #endif

            current++;

            char data[MAX_STR_SIZE] = {};
            int i = 0;

            while (*current && *current != '"' && i < MAX_STR_SIZE - 1)
            {
                data[i++] = *current++;
                (*pos_in_buffer)++;
            }
            data[i] = '\0';

            #ifdef DEBUG
                printf ("\nDEBUG: Data read: '%s'\n", data);
            #endif

            strcpy(node->data, data);

            #ifdef DEBUG
                printf ("DEBUG: Node data set to: '%s'\n", node->data);
                LOADING_BASE_DUMP (tree, buffer_start, *pos_in_buffer, "Node data filled", progress);
            #endif

            if (*current == '"')
            {
                current++;
                (*pos_in_buffer)++;
                #ifdef DEBUG
                    printf ("DEBUG: Closing quote found, moving forward\n");
                #endif
            }
        }
        else
        {
            #ifdef DEBUG
                printf ("DEBUG: WARNING: No opening quote found at: '%.10s...', pos_in_buffer: %d\n", current, *pos_in_buffer);
            #endif
        }

        current = SkipSpaces (current, pos_in_buffer);

        #ifdef DEBUG
            printf ("DEBUG: Before left subtree: '%.10s...', pos_in_buffer: %d\n", current, *pos_in_buffer);
        #endif

        if (*current == '(')
        {
            #ifdef DEBUG
                printf ("DEBUG: Left subtree starts with '('\n");
                LOADING_BASE_DUMP (tree, buffer_start, *pos_in_buffer, "before making LEFT subtree", progress);
            #endif

                node->left = LoadTreeFromFile (tree, &current, pos_in_buffer, progress, buffer_start);

            #ifdef DEBUG
                printf ("DEBUG: Left subtree, result: %p\n", (void*)node->left);
                LOADING_BASE_DUMP (tree, buffer_start, *pos_in_buffer, "LEFT subtree completed", progress);
            #endif
        }
        else if (strncmp (current, "nil", 3) == 0)
        {
            #ifdef DEBUG
                printf ("DEBUG: Left subtree is nil\n");
            #endif

            current += 3;
            (*pos_in_buffer) += 3;

            node->left = NULL;
        }

        current = SkipSpaces (current, pos_in_buffer);

        if (*current == '(')
        {
            #ifdef DEBUG
                printf ("DEBUG: Right subtree starts with '(',\n");
                LOADING_BASE_DUMP (tree, buffer_start, *pos_in_buffer, "before making RIGHT subtree", progress);
            #endif

            node->right = LoadTreeFromFile (tree, &current, pos_in_buffer, progress, buffer_start);

            #ifdef DEBUG
                printf ("DEBUG: Right subtree, result: %p\n", (void*)node->right);
                LOADING_BASE_DUMP (tree, buffer_start, *pos_in_buffer, "RIGHT subtree completed", progress);
            #endif
        }
        else if (strncmp (current, "nil", 3) == 0)
        {
            #ifdef DEBUG
            printf ("DEBUG: Right subtree is nil\n");
            #endif

            current += 3;
            (*pos_in_buffer) += 3;
            node->right = NULL;
        }
        else
        {
            #ifdef DEBUG
                printf ("DEBUG: ERROR: Expected open bracket but found: '%.10s...'\n", current);
            #endif

            return NULL;
        }

        current = SkipSpaces (current, pos_in_buffer);

        #ifdef DEBUG
            printf ("DEBUG: Before closing bracket: '%.10s...', pos_in_buffer: %d\n", current, *pos_in_buffer);
        #endif

        if (*current == ')')
        {
            current++;
            (*pos_in_buffer)++;

            #ifdef DEBUG
                printf ("DEBUG: Closing bracket found, node complete: '%s'\n", node->data);
            #endif
        }
        else
        {
            #ifdef DEBUG
                printf ("DEBUG: ERROR: Expected closing bracket but found: '%.10s...', pos_in_buffer: %d\n", current, *pos_in_buffer);
            #endif

            return NULL;
        }

        *buffer = current;

        #ifdef DEBUG
            printf ("DEBUG: Returning node '%s' at %p\n", node->data, (void*)node);
            LOADING_BASE_DUMP (tree, buffer_start, *pos_in_buffer, "Node FULL BUILT  ;)", progress);
        #endif

        progress->current_rank--;

        return node;
    }
    else
    {
        #ifdef DEBUG
        printf ("DEBUG: No opening bracket found, returning NULL\n");
        #endif
    }

    return NULL;
}

TreeErr_t LoadDatabase (Tree_t* tree, const char* filename)
{
    assert (tree);
    assert (filename);

    #ifdef DEBUG
        printf("DEBUG: LoadDatabase called for file: %s\n", filename);
    #endif

    LoadProgress progress = {};
    InitLoadProgress (&progress);

    FILE* file = fopen (filename, "r");

    if (!file)
    {
        #ifdef DEBUG
        printf("DEBUG: ERROR: Cannot open file %s\n", filename);
        #endif
        printf("No saved database found. Starting with empty knowledge.\n");

        FreeLoadProgress (&progress);

        return NOERORR;
    }

    #ifdef DEBUG
        printf("DEBUG: File opened successfully\n");
    #endif

    // TODO: getFileSize
    fseek (file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek (file, 0, SEEK_SET);

    #ifdef DEBUG
        printf("DEBUG: File size: %ld bytes\n", file_size);
    #endif

    char* buffer = (char*) calloc (file_size + 1, sizeof(char));

    if (!buffer)
    {
        #ifdef DEBUG
            printf("DEBUG: ERROR: Cannot allocate buffer of size %ld\n", file_size + 1);
        #endif

        fclose(file);

        FreeLoadProgress (&progress);

        return ERORRBUFFER;
    }

    fread (buffer, 1, file_size, file);
    buffer[file_size] = '\0';
    fclose (file);

    const char* buffer_ptr = buffer;
    int position = 0;
    const char* buffer_start = buffer;

    tree->root = LoadTreeFromFile(tree, &buffer_ptr, &position, &progress, buffer_start);

    #ifdef DEBUG
        printf("DEBUG: Tree parsing completed, root: %p\n", (void*)tree->root);
    #endif

    free (buffer);
    FreeLoadProgress (&progress);

    #ifdef DEBUG
        printf("DEBUG: Buffer freed\n");
    #endif

    printf ("Database loaded from %s\n", filename);

    return tree->root ? NOERORR : ERORRTREENULL;
}

void InitLoadProgress(LoadProgress* progress)
{
    progress->capacity = 10;
    progress->size = 0;
    progress->current_rank = 0;
    progress->nodes = (LoadNodeInfo*)calloc(progress->capacity, sizeof(LoadNodeInfo));
}

void AddNodeToLoadProgress(LoadProgress* progress, Node_t* node)
{
    if (progress->size >= progress->capacity)
    {
        progress->capacity *= 2;
        progress->nodes = (LoadNodeInfo*)realloc(progress->nodes,
                      progress->capacity * sizeof(LoadNodeInfo));
    }
    progress->nodes[progress->size].node = node;
    progress->nodes[progress->size].rank = progress->current_rank;
    progress->size++;
}

void FreeLoadProgress(LoadProgress* progress)
{
    free(progress->nodes);
    progress->nodes = NULL;
    progress->capacity = 0;
    progress->size = 0;
    progress->current_rank = 0;
}
