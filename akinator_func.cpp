#include "tree_functions.h"
#include "create_dump_files.h"

TreeErr_t GuessObject (Tree_t* tree, Node_t* current_node)
{
    assert (tree);
    assert (current_node);

    //TreeVerify (tree);

    if (current_node->left == NULL && current_node->right == NULL)
    {
        printf("Is your answer: %s?\n", current_node->data);

        char answer[10] = {};

        fgets (answer, sizeof(answer), stdin);

        if (answer[0] == 'y' || answer[0] == 'Y')
        {
            printf("Hey, nigga i know, that, shit!\n");
        }
        else
        {
            printf("Oh, damn, i shoul to known that fucking shit!\n");

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
        //choice = AkinatorGetMode();
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
                Create_log_file(tree, "akinator_dump.dot", DUMP_NORMAL, NULL);
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

    printf("What you think about?\n");

    char new_data[MAX_STR_SIZE] = {};

    fgets(new_data, sizeof(new_data), stdin);

    new_data[strlen(new_data) - 1] = '\0';

    printf("A %s differs from a %s in that it is: ", new_data, current_node->data);

    char new_question[MAX_STR_SIZE] = {};

    fgets(new_question, sizeof(new_question), stdin);

    new_question[strlen(new_data) - 1] = '\0';

    printf("what will be the answer for %s? (y/n): ", new_data);

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
