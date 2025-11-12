#include "tree_functions.h"
#include "create_dump_files.h"

const int DUMMY_VALUE = 0xC0C1C0DE;

TreeErr_t TreeCtor (Tree_t** tree)
{
    *tree = (Tree_t*)calloc(1, sizeof(Tree_t));

    if (!*tree)
    {
        fprintf(stderr, "ERORR: in Ctor - memory allocation\n");

        return ERORRTREEALLOC;
    }

    (*tree)->dummy = (Node_t*)calloc(1, sizeof(Node_t));

    if (!(*tree)->dummy)
    {
        fprintf (stderr, "ERROR: in TreeCtor - memory allocation for dummy node\n");

        free(*tree);

        *tree = NULL;

        return ERORRDUMMYALLOC;
    }

    (*tree)->dummy->left = (*tree)->dummy;

    (*tree)->dummy->right = (*tree)->dummy;

    (*tree)->root = NULL;

    (*tree)->size = 0;

    TreeVerify(*tree);

    //Create_log_file(tree, "list_dump.dot");

    return NOERORR;
}

TreeErr_t TreeInsertLeft (Tree_t* tree, Node_t* parent, const tree_elem_t* answer)
{
    if (!tree || !parent) return ERORRNODENULL;

    Node_t* left_node = NodeCtor(tree, answer);

    if (!left_node)
    {
        fprintf (stderr, "ERORR: invalid ctor of left node!\n");
        return ERORRNODENULL;
    }

    parent->left = left_node;
    tree->size++;
    return NOERORR;
}

TreeErr_t TreeInsertRight (Tree_t* tree, Node_t* parent, const tree_elem_t* answer)
{
    if (!tree || !parent) return ERORRNODENULL;

    Node_t* right_node = NodeCtor(tree, answer);

    if (!right_node)
    {
        fprintf (stderr, "ERORR: invalid ctor of right node!\n");
        return ERORRNODENULL;
    }

    parent->right = right_node;
    tree->size++;
    return NOERORR;
}

TreeErr_t TreeInsertRoot (Tree_t* tree, const char* question)
{
    if (!tree) return ERORRNODENULL;

    Node_t* root_node = NodeCtor(tree, question);

    if (!root_node)
    {
        fprintf (stderr,"ERORR: invalid insert root!\n");
        return ERORRDUMMYALLOC;
    }

    tree->root = root_node;
    tree->size = 1;
    return NOERORR;
}

Node_t* NodeCtor (Tree_t* tree, const tree_elem_t* value)
{
    assert(tree);

    Node_t* new_node = (Node_t*) calloc (1, sizeof(Node_t));

    if (!new_node)
    {
        fprintf (stderr, "ERROR: in NodeCtor - memory allocation\n");

        return NULL;
    }

    strncpy (new_node->data, value, MAX_STR_SIZE - 1);
    new_node->data[MAX_STR_SIZE - 1] ='\0';
    new_node->left= NULL;
    new_node->right = NULL;

    return new_node;
}

TreeErr_t TreeDtor (Tree_t* tree)
{
    if (!tree) return ERORRNODENULL;

    DeleteSubtree (tree, &tree->root);
    free (tree->dummy);
    free (tree);

    return NOERORR;
}

TreeErr_t PrintNode (const Node_t* node)
{
    if (!node)
    {
        fprintf(stderr, "ERROR: in NodeCtor - memory allocation\n");

        return ERORRNODENULL;
    }

    printf("(");

    if (node->left)
        PrintNode(node->left);

    printf ("%s", node->data);

    if (node->right)
        PrintNode(node->right);

    printf(")");

    return NOERORR;
}

TreeErr_t TreeVerify (Tree_t* tree)
{
    if (!tree)
    {
        fprintf(stderr, "ERORR: NULL tree\n");
        return ERORRTREEALLOC;
    }

    if (tree->dummy->left != tree->dummy || tree->dummy->right != tree->dummy)
    {
        fprintf(stderr, "ERORR: invalid dummy\n");
        return ERORRDUMMYALLOC;
    }

    if (tree->size > 0 && tree->root == NULL)
    {
        fprintf(stderr, "ERORR: invalid ROOT\n");
        return ERORRINVALIDROOT;
    }

    Node_t** visited = (Node_t**)calloc (tree->size + 10, sizeof(Node_t*));

    size_t visited_count = 0;

    int tree_verify_result = TreeVerifyNode (tree, tree->root, visited, &visited_count);

    free (visited);

    return tree_verify_result;
}

TreeErr_t TreeVerifyNode (Tree_t* tree, Node_t* node, Node_t** visited, size_t* counter)
{
    if (node == NULL || node == tree->dummy) return NOERORR;

    for (size_t i = 0; i < *counter; i++)
    {
        if (visited[i] == node)
        {
            fprintf(stderr, "ERORR: tree has cycle!!");
            return ERORRCYCLE;
        }
    }

    if (node->left == node || node->right == node)
    {
        fprintf(stderr, "ERORR: pointer to parent!!!!\n");
    }

    visited[*counter] = node;
    (*counter)++;

    TreeErr_t left_result = TreeVerifyNode (tree, node->left, visited, counter);

    TreeErr_t right_result = TreeVerifyNode (tree, node->right, visited, counter);

    if (left_result == NOERORR)
        return right_result;
    else
        return left_result;
}

TreeErr_t DeleteSubtree (Tree_t* tree, Node_t** node)
{
    if (!tree || !node || !*node) return ERORRNODENULL;

    Node_t* node_2_delete = *node;

    if (node_2_delete->left != NULL)
    {
        DeleteSubtree (tree, &node_2_delete->left);
    }

    if (node_2_delete->right != NULL)
    {
        DeleteSubtree (tree, &node_2_delete->right);
    }

    *node = NULL;
    free (node_2_delete);
    tree->size--;

    return NOERORR;
}

TreeErr_t GuessObject (Tree_t* tree, Node_t* current_node)
{
    assert (tree);
    assert (current_node);

    TreeVerify (tree);

    if (current_node->left == NULL && current_node->right == NULL)
    {
        printf("Is your answer: %s?\n", current_node->data);

        char answer[10] = {};

        fgets (answer, sizeof(answer), stdin);

        if (answer[0] == 'y' || answer[0] == 'Y')
        {
            printf("Hey, nigga i know, that, shit!\n");
        }
        else // Todo if NO ли правашу СДЕЛАЙ ФУНКЦИЮ ОБРАБОТКИ ОТВЕТА ДАУН
        {
            printf("Oh, damn, i shoul to known that fucking shit!");

            AddNewObject(tree, current_node);
        }
        return NOERORR;
    }

    printf("%s (y/n): ", current_node->data);

    char answer[10] = {};
    fgets(answer, sizeof(answer), stdin);

    if (answer[0] == 'y' || answer[0] == 'Y')
    {
        return GuessObject(tree, current_node->right);
    }
    else if (answer[0] == 'n' || answer[0] == 'N')
    {
        return GuessObject(tree, current_node->left);
    }
    else
    {
        printf("Please answer 'yes' or 'no'\n");
        return GuessObject(tree, current_node);
    }
}

void PrintMenu()
{
    printf("\n=== AKINATOR MENU ===\n");
    printf("[G]uess object\n");
    printf("[D]efine object\n");
    printf("[C]ompare two objects\n");
    printf("[S]how knowledge base\n");
    printf("[A]dd new object\n");
    printf("[E]xit\n");
    printf("Choose action: ");
}

void AkinatorMenu (Tree_t* tree)
{
    char choice = 0;

    LoadDatabase(tree, "okinator_db.txt");

    while (choice != 'e' && choice != 'E')
    {
        PrintMenu();

        scanf("%c", &choice);
        // TODO: func
        choice = AkinatorGetMode();
        getchar(); // нет \n теперь

        QUICK_DUMP(tree, "check tree before action");

        switch (choice)
        {
            case 'g':
            case 'G': // AKINATOR_GUESS
            {
                printf("\n--- Guess object---\n");
                GuessObject (tree, tree->root);
                break;
            }
            case 'c':
            case 'C': // AKINATOR_COMPARE
            {
                printf("\n--- Compare two objects ---\n");
                // CompareObjects(tree->root);
                break;
            }
            case 's':
            case 'S':
            {
                printf("\n--- Knowledge base ---\n");
                PrintNode(tree->root);
                printf("\n");
                Create_log_file(tree, "akinator_dump.dot");
                break;
            }
            case 'd':
            case 'D':
            {
                // TODO: func

                printf("\n--- Define object ---\n");

                printf("Enter object: ");

                char object[100] = {};
                fgets(object, sizeof(object), stdin);
                object[strlen(object) - 1] = '\0';

                printf("Searching for: %s\n", object);

                int found = DescribeObject(tree->root, object);

                if (!found)
                {
                    printf("Object not found!");
                }
                printf("\n");
                break;
            }
            case 'e':
            case 'E':
            {
                printf("Saving database...\n");
                SaveDatabase(tree, "okinator_db.txt");
                printf("Goodbye, bitch!\n");
                return;
            }
            default:
                printf("Hey, bitch, your command invalid!!\n");

        }
    }
}

TreeErr_t AddNewObject(Tree_t* tree, Node_t* current_node)
{
    if (!tree || !current_node) return ERORRNODENULL;

    printf("Введите новый объект\n");

    char new_data[MAX_STR_SIZE] = {};

    fgets(new_data, sizeof(new_data), stdin);

    new_data[strlen(new_data) - 1] = '\0';

    printf("чем отличается %s от %s: ", new_data, current_node->data);

    char new_question[MAX_STR_SIZE] = {};

    fgets(new_question, sizeof(new_question), stdin);

    new_question[strcspn(new_question, "\n")] = '\0'; // TODO

    printf("Каков ответ для %s? (y/n): ", new_data);

    char answer[10] = {};
    fgets(answer, sizeof(answer), stdin);

    char old_data[MAX_STR_SIZE] = {};
    strcpy(old_data, current_node->data);

    strcpy(current_node->data, new_question);

    Node_t* yes_node = NodeCtor(tree, new_data);
    Node_t* no_node = NodeCtor(tree, old_data);

    if (!yes_node || !no_node)
    {
        strcpy(current_node->data, old_data);
        return ERORRNODENULL;
    }

    if (answer[0] == 'y' || answer[0] == 'Y') // TODO: AkinatorGetAnswer
    {
        current_node->right = yes_node;
        current_node->left = no_node;
    }
    else
    {
        current_node->left = yes_node;
        current_node->right = no_node;
    }

    tree->size += 2;
    printf("\nOk, my nigga, I'll remember that shit\n");
    return NOERORR;
}

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

const char* SkipSpaces(const char* str)
{
    assert (str);

    while (*str && isspace((unsigned char)*str))
    {
        str++;
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
Node_t* LoadTreeFromFile (Tree_t* tree, const char** buffer /* , int pos_in_buffer */)
{
    assert(tree);
    assert(buffer);

    #ifdef DEBUG
        printf("DEBUG: LoadTreeFromFile called, buffer starts with: '%.10s...'\n", *buffer);
    #endif

    const char* current = SkipSpaces (*buffer);

    #ifdef DEBUG
        printf("DEBUG: After SkipSpaces: '%.10s...'\n", current);
    #endif

    if (*current == '(')
    {
        #ifdef DEBUG
            printf("DEBUG: Found opening bracket, starting new node\n");
        #endif

        current = SkipSpaces (current + 1);

        #ifdef DEBUG
            printf("DEBUG: After skipping spaces after '(': '%.10s...'\n", current);
        #endif

        Node_t* node = NodeCtor(tree, "");

        if (!node)
        {
            printf("DEBUG: ERROR: NodeCtor failed!\n");
            return NULL;
        }

        #ifdef DEBUG
            printf("DEBUG: Node created successfully at %p\n", (void*)node);
        #endif

        // TODO: GetNodeInfo

        if (*current == '"')
        {
            #ifdef DEBUG
                printf("DEBUG: Found opening quote, reading data\n");
            #endif
            current++;

            char data[MAX_STR_SIZE] = {};
            int i = 0;

            while (*current && *current != '"' && i < MAX_STR_SIZE - 1)
            {
                data[i++] = *current++;
            }
            data[i] = '\0';

            #ifdef DEBUG
                printf("\nDEBUG: Data read: '%s'\n", data);
            #endif

            strcpy(node->data, data);

            #ifdef DEBUG
                printf("DEBUG: Node data set to: '%s'\n", node->data);
            #endif

            if (*current == '"')
            {
                current++;
                #ifdef DEBUG
                    printf("DEBUG: Closing quote found, moving forward\n");
                #endif
            }
        }
        else
        {
            #ifdef DEBUG
                printf("DEBUG: WARNING: No opening quote found at: '%.10s...'\n", current);
            #endif
        }

        current = SkipSpaces (current);

        #ifdef DEBUG
            printf("DEBUG: Before left subtree: '%.10s...'\n", current);
        #endif

        if (*current == '(')
        {
            #ifdef DEBUG
                printf("DEBUG: Left subtree starts with '(', parsing recursively\n");
            #endif

            node->left = LoadTreeFromFile (tree, &current);

            #ifdef DEBUG
                printf("DEBUG: Left subtree parsed, result: %p\n", (void*)node->left);
            #endif
        }
        else if (strncmp(current, "nil", 3) == 0)
        {
            #ifdef DEBUG
                printf("DEBUG: Left subtree is nil\n");
            #endif

            current += 3;
            node->left = NULL;
        }

        current = SkipSpaces (current);

        if (*current == '(')
        {
            #ifdef DEBUG
            printf("DEBUG: Right subtree starts with '(', parsing recursively\n");
            #endif

            node->right = LoadTreeFromFile (tree, &current);

            #ifdef DEBUG
            printf("DEBUG: Right subtree parsed, result: %p\n", (void*)node->right);
            #endif
        }
        else if (strncmp(current, "nil", 3) == 0)
        {
            #ifdef DEBUG
            printf("DEBUG: Right subtree is nil\n");
            #endif

            current += 3;
            node->right = NULL;
        }
        else
        {
            #ifdef DEBUG
                printf("DEBUG: ERROR: Expected open bracket but found: '%.10s...'\n", current);
            #endif

            return NULL;
        }

        current = SkipSpaces (current);

        #ifdef DEBUG
        printf("DEBUG: Before closing bracket: '%.10s...'\n", current);
        #endif

        if (*current == ')')
        {
            current++;
            #ifdef DEBUG
                printf("DEBUG: Closing bracket found, node complete: '%s'\n", node->data);
            #endif
        }
        else
        {
            #ifdef DEBUG
                printf("DEBUG: ERROR: Expected closing bracket but found: '%.10s...'\n", current);
            #endif

            return NULL;
        }

        *buffer = current;

        #ifdef DEBUG
            printf("DEBUG: Returning node '%s' at %p\n", node->data, (void*)node);
        #endif

        return node;
    }
    else
    {
        #ifdef DEBUG
        printf("DEBUG: No opening bracket found, returning NULL\n");
        #endif
    }

    return NULL;
}

TreeErr_t LoadDatabase(Tree_t* tree, const char* filename)
{
    assert(tree);
    assert(filename);

    #ifdef DEBUG
        printf("DEBUG: LoadDatabase called for file: %s\n", filename);
    #endif

    FILE* file = fopen(filename, "rb");

    if (!file)
    {
        #ifdef DEBUG
        printf("DEBUG: ERROR: Cannot open file %s\n", filename);
        #endif
        printf("No saved database found. Starting with empty knowledge.\n");
        return NOERORR;
    }

    #ifdef DEBUG
        printf("DEBUG: File opened successfully\n");
    #endif

    // TODO: getFileSize
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    #ifdef DEBUG
        printf("DEBUG: File size: %ld bytes\n", file_size);
    #endif

    char* buffer = (char*)calloc(file_size + 1, sizeof(char));

    if (!buffer)
    {
        #ifdef DEBUG
            printf("DEBUG: ERROR: Cannot allocate buffer of size %ld\n", file_size + 1);
        #endif

        fclose(file);
        return ERORRBUFFER;
    }

    fread(buffer, 1, file_size, file);
    buffer[file_size] = '\0';
    fclose(file);

    tree->root = LoadTreeFromFile(tree, &buffer_ptr);

    #ifdef DEBUG
        printf("DEBUG: Tree parsing completed, root: %p\n", (void*)tree->root);
    #endif

    free(buffer);

    #ifdef DEBUG
        printf("DEBUG: Buffer freed\n");
    #endif

    printf("Database loaded from %s\n", filename);

    return tree->root ? NOERORR : ERORRTREENULL;
}

int DescribeObject(Node_t* current_node, const char* word)
{
    if (current_node == NULL)
    {
    #ifdef DEBUG
        printf("DEBUG: FOUND! %s\n", current_node->data);
        printf("DEBUG: Reached NULL node\n");
    #endif
        return 0;
    }

    #ifdef DEBUG
        printf("DEBUG: Checking node: '%s' vs target: '%s'\n", current_node->data, word);
    #endif

    if (strcmp(current_node->data, word) == 0)
    {
        printf ("%s ", current_node->data);
        return 1;
    }

    #ifdef DEBUG
        printf("DEBUG: Searching left...\n");
    #endif

    if (DescribeObject(current_node->left, word) == 1)
    {
        printf ("not(%s) ", current_node->data);
        return 1;
    }

    #ifdef DEBUG
        printf("DEBUG: Searching right...\n");
    #endif

    if (DescribeObject(current_node->right, word) == 1)
    {
        printf ("(%s) ", current_node->data);
        return 1;
    }

    return 0;
}

/*TreeErr_t DescribeObject(Tree_t* tree)
{
    if (!tree || !tree->root)
    {
        printf("Tree is empty!\n");
        return ERORRTREENULL;
    }

    printf("Enter object: ");

    char object[100];
    fgets(object, sizeof(object), stdin);
    object[strcspn(object, "\n")] = '\0';

    printf("Searching for: %s\n", object);

    TreeErr_t result = FindAndPrintPath(tree->root, object, "Start");

    if (result != NOERORR)
    {
        printf("Object not found!\n");
    }

    return result;
}


TreeErr_t FindAndPrintPath(Node_t* node, const char* target, const char* path)
{
    if (!node) return ERORRNODENULL;

    if (strcmp(node->data, target) == 0)
    {
        printf("Path: %s -> %s\n", path, node->data);
        return NOERORR;
    }

    if (node->left)
    {
        char new_path[1000];
        snprintf(new_path, sizeof(new_path), "%s -> NO: %s", path, node->left->data);
        TreeErr_t left_result = FindAndPrintPath(node->left, target, new_path);
        if (left_result == NOERORR) return NOERORR;
    }

    if (node->right)
    {
        char new_path[1000];
        snprintf(new_path, sizeof(new_path), "%s -> YES: %s", path, node->right->data);
        TreeErr_t right_result = FindAndPrintPath(node->right, target, new_path);
        if (right_result == NOERORR) return NOERORR;
    }

    return ERORRNODENULL;
}*/
