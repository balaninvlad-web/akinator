#include "create_dump_files.h"

const int MAX_COMMAND_LENGTH  = 200;

void Create_log_file (Tree_t* tree, const char* filename)
{
    assert (tree);

    FILE* dot_file = fopen (filename, "w");

    Create_head_log_file (dot_file, tree);

    Create_graph_node (tree, dot_file, tree->root);

    Make_arrow (tree, dot_file, tree->root);

    fprintf (dot_file,"}\n}\n");

    fclose (dot_file);

    Create_picture ();
}

void Create_head_log_file (FILE* dot_file, Tree_t* tree)
{
    assert (dot_file);

    printf ("START Writing TO file.....\n");

    fprintf (dot_file,"digraph tree{\n");
    fprintf (dot_file, "    charset=\"UTF-8\";\n");

    fprintf (dot_file, "    bgcolor=\"#001f29\"\n");

    fprintf (dot_file, "    rankdir = TB\n");
    fprintf (dot_file, "    nodesep = 0.5;\n");
    fprintf (dot_file, "    ranksep = 0.7;\n");

    fprintf (dot_file, "    node [shape=plaintext, style=filled, fontname=\"Arial\"];\n");

    fprintf (dot_file, "    edge [fontname=\"Arial\"];\n\n");

    fprintf (dot_file, "    labelloc=\"t\";\n");
    fprintf (dot_file, "    label=\"Tree size: %d\\nRoot: %p\";\n", tree->size, (void*)tree->root);

    fprintf (dot_file,"{\n");
}

void Create_graph_node (Tree_t* tree, FILE* dot_file, Node_t* node)
{
    assert (tree);

    assert (dot_file);

    if (node == tree->dummy || node == NULL) return;

    const char* fillcolor = "";
    const char* color = "black";

    if (node == tree->root)
    {
        fillcolor = "#5f3035";  // Корень
        color = "#fdfdfd";
    }
    else if (node->right == NULL && node->left == NULL)
    {
        fillcolor = "#445c00";  // Свободный узел обьекты
        color = "#fdfdfd";
    }
    else
    {
        fillcolor = "#2799a0";  // Обычный узел вопросы
        color = "#fdfdfd";
    }

    char escaped_data[MAX_STR_SIZE * 2] = {0};
    EscapeHtml(node->data, escaped_data, sizeof(escaped_data));

    fprintf (dot_file, "    node%p [label=<<TABLE BORDER='1' CELLBORDER='1' CELLSPACING='0'>"
            "<TR><TD COLSPAN='2'>%s</TD></TR>"
            "<TR><TD>NO</TD><TD>YES</TD></TR></TABLE>>, "
            "fillcolor=\"%s\", color=\"%s\", fontcolor=\"%s\"];\n",
            (void*)node, escaped_data, fillcolor, color, color);

    printf ("BLOCKS Writing TO file.....\n");

    Create_graph_node (tree, dot_file, node->left);

    Create_graph_node (tree, dot_file, node->right);
}

void Make_arrow (Tree_t* tree, FILE* dot_file, Node_t* node)
{
    assert (tree);

    assert (dot_file);

    if (node == tree->dummy || node == NULL) return;

    printf("Make_arrow: creating connections...\n");

    if (node->right)
    {
        fprintf (dot_file, "    node%p -> node%p [color=\"#adebff\", penwidth=3, label=\"YES\", fontcolor=\"#adebff\", fontsize=13, arrowsize=0.8, penwidth=1, weight=3];\n",
                (void*)node, (void*)node->right);
    }

    if (node->left)
    {
        fprintf (dot_file, "    node%p -> node%p [color=\"#ffadb1\", penwidth=3, label=\"NO\"fontcolor=\"#ffadb1\", fontsize=13, arrowsize=0.8, penwidth=2, weight=3];\n",
                (void*)node, (void*)node->left);
    }

    Make_arrow(tree, dot_file, node->left);

    Make_arrow(tree, dot_file, node->right);
}

void Create_picture (void)
{
    static int image_counter = 1;

    char command[MAX_COMMAND_LENGTH] = "";

    snprintf (command, sizeof(command), "dot -Tpng tree_dump.dot -o imagesDump/tree_dump%d.png", image_counter);

    system (command);

    printf ("dot -Tpng tree_dump.dot -o imagesDump/tree_dump%d.png", image_counter);

    printf ("Graph generated: list_dump%d.png\n", image_counter);

    image_counter++;
}

void Create_dump_files(Tree_t* tree, const char* file, const char* func, int line, const char* reason, ...){
    assert (tree);
    assert(file);
    assert(func);
    assert(reason);

    char formatted_reason[300] = {};
    va_list args = NULL;
    va_start(args, reason);
    vsnprintf(formatted_reason, sizeof(formatted_reason), reason, args);

    int dump_type = DUMP_NORMAL;
    const char* buffer = NULL;
    size_t position = 0;

    if (args != NULL)
    {
        dump_type = va_arg(args, int);
        if (dump_type == DUMP_PARSING)
        {
            buffer = va_arg(args, const char*);
            position = va_arg(args, size_t);
        }
    }
    va_end(args);

     printf("Dump called from %s:%d (%s): %s\n", file, line, func, formatted_reason);

    char graph_filename[50] = {};
    char dot_filename[50] = {};
    char command[100] = {};
    static int dump_counter = 1;
    int result = 0;

    printf ("Create_dump_files STARTED for %s\n", func);

    if (dump_counter == 1)
    {
        system("mkdir -p imagesDump");
    }

    printf ("=== Make_html_file called #%d ===\n", dump_counter);


    snprintf (graph_filename, sizeof(graph_filename), "graph_%d.png", dump_counter);

    snprintf (dot_filename, sizeof(dot_filename), "graph_%d.dot", dump_counter);

    Create_html_file(tree, dump_counter, graph_filename, func, formatted_reason, dump_type, buffer, position);

    printf ("Creating DOT file: %s\n", dot_filename);

    Create_log_file (tree, dot_filename);

    printf ("Converting to PNG...\n");

    snprintf (command, sizeof(command), "dot -Tpng %s -o imagesDump/%s", dot_filename, graph_filename);

    result = system (command);

    if (result != 0)
    {
            printf ("WARNING: Graph generation failed for dump #%d\n", dump_counter);
    }

    remove (dot_filename);

    printf ("Create_dump_files COMPLETED\n");

    dump_counter++;
}

void Create_html_file(Tree_t* tree, int dump_counter, const char* graph_filename, const char* func, const char* formatted_reason, ...)
{
    static FILE* html_file = NULL;

    if (html_file == NULL)
    {
        Create_head_html (&html_file);
    }

    fprintf (html_file, "<div class='dump'>\n");
    fprintf (html_file, "<h2>Dump %d Called from: <font color=white>%s</font></h2>", dump_counter, func);
    fprintf(html_file, "<h2>Dump %d Called because: <font color=white>%s</font></h2>\n", dump_counter, formatted_reason);

    va_list args;
    va_start(args, formatted_reason);

    int dump_type = DUMP_NORMAL;
    const char* buffer = NULL;
    size_t position = 0;

    if (args != NULL)
    {
        dump_type = va_arg(args, int);
        if (dump_type == DUMP_PARSING)
        {
            buffer = va_arg(args, const char*);
            position = va_arg(args, size_t);
        }
    }
    va_end(args);

    if (dump_type == DUMP_PARSING && buffer != NULL)
    {
        PrintBuffer(html_file, buffer, position);
    }

    if (tree == NULL)
    {
        printf ("ERROR: tree is NULL\n");
        fprintf (html_file, "<p><b>ERROR: List is NULL</b></p>\n");
        fprintf (html_file, "</div>\n");
        dump_counter++;
        return;
    }

    fprintf (html_file, "<h3>Graph Visualization</h3>\n");

    fprintf (html_file, "<img src='imagesDump/%s' alt='Graph %d' width='1500'>\n", graph_filename, dump_counter);

    fprintf (html_file, "</pre>\n");
    fprintf  (html_file, "</div>\n");

    fflush (html_file);
    printf ("HTML dump #%d completed\n", dump_counter);
}

void Create_head_html (FILE** html_file)
{
    printf ("Creating HTML file...\n");
    *html_file = fopen ("All_dumps.html", "w");

    if (html_file == NULL)
    {
        printf("ERROR: Cannot create HTML file\n");
        return;
    }

    fprintf (*html_file, "<!DOCTYPE html>\n");
    fprintf (*html_file, "<html lang='ru'>\n");
    fprintf (*html_file, "<head>\n");
    fprintf (*html_file, "<meta charset='UTF-8'>\n");
    fprintf (*html_file, "<title>TREE Dumps</title>\n");
    fprintf (*html_file, "<style>\n");

    fprintf (*html_file, "body { background-color: #001f29; color: #ffffff; }\n");

    fprintf (*html_file, "table { border-collapse: collapse; margin: 10px; }\n");
    fprintf (*html_file, "th, td { border: 1px solid #0077a3; padding: 5px; }\n");
    fprintf (*html_file, "th { background-color: #00415a; }\n");

    fprintf (*html_file, "img { max-width: 100%%; height: auto; margin: 10px; }\n");

    fprintf (*html_file, ".dump { border: 2px solid #0099cc; padding: 15px; margin: 10px; }\n");

    fprintf (*html_file, "h1 { color: #00ccff; }\n");
    fprintf (*html_file, "h2 { color: #00b8e6; }\n");
    fprintf (*html_file, "h3 { color: #66d9ff; }\n");

    fprintf (*html_file, "pre { color: #b3ecff; background-color: #001a21; padding: 10px; }\n");

    fprintf(*html_file, ".parsing-section { margin: 20px 0; padding: 15px; background: #002b36; border: 1px solid #005a73; }\n");
    fprintf(*html_file, ".buffer-display { font-family: 'Courier New', monospace; font-size: 14px; background: #001f29; padding: 10px; border-radius: 5px; }\n");
    fprintf(*html_file, ".getted-text { color: #888888; }\n");
    fprintf(*html_file, ".cursor { color: #ff4444; font-weight: bold; }\n");
    fprintf(*html_file, ".notgetted-text { color: #ffffff; }\n");
    fprintf(*html_file, ".buffer-stats { margin-top: 10px; color: #66d9ff; font-size: 12px; }\n");

    fprintf (*html_file, "</style>\n");
    fprintf (*html_file, "</head>\n");
    fprintf (*html_file, "<body>\n");
    fprintf (*html_file, "<h1 style='color: red;'>ALL LIST DUMPS</h1>\n");
    printf ("HTML file created successfully\n");
}

void EscapeHtml(const char* input, char* output, size_t output_size)
{
    const char* src = input;
    char* dest = output;
    size_t remaining = output_size - 1;

    while (*src && remaining > 1)
    {
        switch (*src)
        {
            case '&':
                if (remaining > 5) { strncpy(dest, "&amp;", 6); dest += 5; remaining -= 5; }
                break;
            case '<':
                if (remaining > 4) { strncpy(dest, "&lt;", 5); dest += 4; remaining -= 4; }
                break;
            case '>':
                if (remaining > 4) { strncpy(dest, "&gt;", 5); dest += 4; remaining -= 4; }
                break;
            case '"':
                if (remaining > 6) { strncpy(dest, "&quot;", 7); dest += 6; remaining -= 6; }
                break;
            case '\'':
                if (remaining > 6) { strncpy(dest, "&apos;", 7); dest += 6; remaining -= 6; }
                break;
            default:
                *dest++ = *src;
                remaining--;
                break;
        }
        src++;
    }
    *dest = '\0';
}

void PrintBuffer (FILE* html_file, const char* buffer, size_t position)
{
    fprintf(html_file, "<div class='parsing-section'>\n");
    fprintf(html_file, "<h4>Buffer:</h4>\n");
    fprintf(html_file, "<div class='buffer-display'>\n");
    fprintf(html_file, "<span class = 'getted-text'>%.*s</span>", (int)position, buffer);
    fprintf(html_file, "<span class = 'cursor'>|</span>");
    fprintf(html_file, "<span class = 'notgetted-text'>%s</span>", buffer + position);
    fprintf(html_file, "</div>\n");

    size_t len = strlen(buffer);
    if (len > 0)
    {
        int percent = (int)((position * 100) / len);
        fprintf(html_file, "<div class='buffer-stats'>Position: %zu/%zu (%d%%)</div>\n", position, len, percent);
    }

    fprintf(html_file, "</div>\n");
}
