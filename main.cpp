#include "tree_functions.h"
#include "create_dump_files.h"

int main ()
{
    Tree_t* Tree = NULL;

    TreeCtor (&Tree);

    //TreeInsertRoot(Tree, "Is is amimal?");

    //QUICK_DUMP(Tree, "check tree after Ctor");

    //TreeInsertRoot(Tree, "Это животное");

    //QUICK_DUMP(Tree, "check after insert_root");

    /*TreeInsertLeft(Tree, Tree->root, "Это растение");
    TreeInsertRight(Tree, Tree->root, "Оно мяукает");

    TreeInsertLeft(Tree, Tree->root->left, "Стул");
    TreeInsertRight(Tree, Tree->root->left, "Дуб");

    TreeInsertLeft(Tree, Tree->root->right, "Собака");
    TreeInsertRight(Tree, Tree->root->right, "Кот");*/

    QUICK_DUMP(Tree, "check tree after make tree nods");

    PrintNode (Tree->root);

    AkinatorMenu(Tree);

    Create_log_file (Tree, "tree_dump.dot");

    system("dot -V");
}
