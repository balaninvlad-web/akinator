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

    if ((uintptr_t)node < 0x1000)
    {
        fprintf(stderr, "ERORR: invalid node pointer %p\n", (void*)node);
        return ERORRNODENULL;
    }

    if (node->left == node || node->right == node)
    {
        fprintf(stderr, "ERORR: pointer to parent!!!!\n");
        return ERORRCYCLE;
    }

    for (size_t i = 0; i < *counter; i++)
    {
        if (visited[i] == node)
        {
            fprintf(stderr, "ERORR: tree has cycle!!");
            return ERORRCYCLE;
        }
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


