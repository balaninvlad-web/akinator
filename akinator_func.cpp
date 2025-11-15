#include "tree_functions.h"
#include "create_dump_files.h"

TreeErr_t GuessObject (Tree_t* tree, Node_t* current_node)
{
    assert (tree);
    assert (current_node);

    //TreeVerify (tree);

    Node_t* current = current_node;

    while (current->left != NULL && current->right != NULL)
    {
        UserAnswer answer = UNKNOWN;

        while (answer == UNKNOWN)
        {
            printf("%s (y/n): ", current->data);

            answer = AkinatorGetAnswer();

            if (answer == NO)
                current = current->left;

            else if (answer == YES)
                current = current->right;

            else
                printf("Please answer with 'y' or 'n'\n");
        }
    }

    printf("Is your answer: %s?\n", current->data);

    UserAnswer final_answer = UNKNOWN;

    while (final_answer == UNKNOWN)
    {
        printf("Did I guess right?");

        final_answer = AkinatorGetAnswer();

        if (final_answer == YES)
        {
            printf("Great! I guessed it!\n");
            return NOERORR;
        }
        else if (final_answer == NO)
        {
            printf("Oh, I should learn that!\n");
            return AddNewObject (tree, current);
        }
        else
        {
            printf("Please answer with 'y' or 'n'\n");
        }
    }

    return NOERORR;
}

void PrintMenu()
{
    printf("\n=== AKINATOR MENU ===\n");
    //speak("poshaluista viberi deistvie brooo");
    printf("[G]uess object\n");
    printf("[D]efine object\n");
    printf("[C]ompare two objects\n");
    printf("[S]how knowledge base\n");
    printf("[E]xit\n");
    printf("Choose action: ");
}

void AkinatorMenu (Tree_t* tree)
{
    LoadDatabase(tree, "okinator_db.txt");

    AkinatorMode mode = UNKNOWN_TYPE;

    do
    {
        PrintMenu();

        mode = AkinatorGetMode();

        QUICK_DUMP(tree, "check tree before action");

        switch (mode)
        {
            case GUESSTYPE:
            {
                printf("\n--- Guess object---\n");
                GuessObject (tree, tree->root);
                break;
            }
            case COMPARETYPE:
            {
                printf("\n--- Compare two objects ---\n");
                // CompareObjects(tree->root);
                break;
            }
            case SAVEBASETYPE:
            {
                printf("\n--- Knowledge base ---\n");
                PrintNode(tree->root);
                printf("\n");
                Create_log_file(tree, "akinator_dump.dot", DUMP_NORMAL, NULL);
                break;
            }
            case DEFINETYPE:
            {
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
            case EXITTYPE:
            {
                printf("Saving database...\n");
                SaveDatabase(tree, "okinator_db.txt");
                printf("Goodbye, bitch!\n");
                return;
            }
            case UNKNOWN_TYPE:
            default:
                printf("Hey, bitch, your command invalid!!\n");

        }
    } while (mode != EXITTYPE);
}

TreeErr_t AddNewObject(Tree_t* tree, Node_t* current_node)
{
    if (!tree || !current_node) return ERORRNODENULL;

    printf ("What you think about?\n");
    //speak ("What you think about?\n");

    char new_data[MAX_STR_SIZE] = {};

    fgets(new_data, sizeof(new_data), stdin);

    new_data[strlen(new_data) - 1] = '\0';

    printf ("A %s differs from a %s in that it is: ", new_data, current_node->data);
    //speak ("A %s differs from a %s in that it is: ", new_data, current_node->data);

    char new_question[MAX_STR_SIZE] = {};

    fgets(new_question, sizeof(new_question), stdin);

    new_question[strlen(new_data) - 1] = '\0';

    char* old_data = strdup(current_node->data);

    free(current_node->data);
    current_node->data = strdup(new_question);

    Node_t* yes_node = NodeCtor(tree, new_data);
    Node_t* no_node = NodeCtor(tree, old_data);

    free(old_data);

    if (!yes_node || !no_node)
    {
        free(current_node->data);
        current_node->data = strdup(old_data);
        return ERORRNODENULL;
    }

    current_node->right = yes_node;
    current_node->left = no_node;

    tree->size += 2;
    TreeVerify (tree);
    printf("\nOk, my nigga, I'll remember that shit\n");
    return NOERORR;
}

int DescribeObject (Node_t* current_node, const char* word)
{
    assert (word);

    if (current_node == NULL)
    {
        #ifdef DEBUG
            printf("DEBUG: Reached NULL node\n");
        #endif
        return 0;
    }

    #ifdef DEBUG
        printf("DEBUG: Checking node: '%s' vs target: '%s'\n", current_node->data, word);
    #endif

    if (strcmp (current_node->data, word) == 0)
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

AkinatorMode AkinatorGetMode(void)
{
    char choice = 0;
    char buffer[MAX_STR_SIZE] = {};

    while (1)
    {
        if (fgets(buffer, sizeof(buffer), stdin) == NULL)
        {
            printf("Input error\n");
            return EXITTYPE;
        }

        char* input = buffer;
        while (*input == ' ' || *input == '\t') input++;

        choice = *input;

        switch (choice)
        {
            case 'g':
            case 'G':
                printf("Starting Guess mode...\n");
                return GUESSTYPE;
            case 'c':
            case 'C':
                printf("Starting Compare mode...\n");
                return COMPARETYPE;
            case 'd':
            case 'D':
                printf("Starting Define mode...\n");
                return DEFINETYPE;
            case 's':
            case 'S':
                printf("Saving database...\n");
                return SAVEBASETYPE;
            case 'e':
            case 'E':
                printf("Goodbye!\n");
                return EXITTYPE;
            case '\n':
                printf("Please enter a command\n");
                break;
            default:
                printf("Invalid command '%c'. Please use: G, C, D, S, E\n", choice);
                break;
        }
    }
}

UserAnswer AkinatorGetAnswer(void)
{
    char buffer[100] = {};

    while (1)
    {
        if (fgets(buffer, sizeof(buffer), stdin) == NULL)
        {
            printf("Input error, please try again: ");
            continue;
        }

        if (strchr(buffer, '\n') == NULL)
        {
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
        }

        char* input = buffer;
        while (*input == ' ' || *input == '\t') input++;

        char choice = *input;

        switch (choice)
        {
            case 'y':
            case 'Y':
                return YES;
            case 'n':
            case 'N':
                return NO;
            case '\n':
                printf("Please enter 'y' for yes or 'n' for no: ");
                break;
            default:
                printf("Invalid answer '%c'. Please enter 'y' for yes or 'n' for no: ", choice);
                break;
        }
    }
}
