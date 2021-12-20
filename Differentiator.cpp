#include "Differentiator.h"

const char *GRAPH_NAME  = "DifferentiatorGraph.dot";
const char *IMG_NAME    = "DifferentiatorImg.png";   

#define test printf("%d\n", __LINE__);

int main()
{
    Prog prog = {};
    ReadExpression(&prog);
    TreeCtor(&prog);
    TexInit(tex_file);
    TexBeginning(tex_file, prog.tree.root);
    prog.tree.root = DifTree(prog.tree.root);
    TexAfterDif(tex_file, prog.tree.root);                               
    Simplify(&prog.tree.root);
    DifferentiatorDoGraph(prog.tree.root);
    TexEnd(tex_file);
    return OK;
}

void ProgDtor(Prog *prog)
{
    assert(prog != nullptr);

    TreeDtor(prog->tree.root);

    free(prog->expression.buffer);
    prog->expression.size = 0;
    prog->expression.buffer = nullptr;
}

int TreeCtor(Prog *prog)
{
    assert(prog != nullptr);

    char *cur_symb = prog->expression.buffer;
    
    if (*cur_symb != '(')
    {
        _WRONG_EXPRESSION;
    }
    ++cur_symb;

    Stack_t stack = {};
    StackCtor(&stack, MAX_TREE_DEPTH);
    StackPush(&stack, nullptr);

    prog->tree.root = NodeCtor();
    Node *cur_node  = prog->tree.root;

    while (cur_symb - prog->expression.buffer < prog->expression.size)
    { 
        switch (*cur_symb)
        {
            case '(':
                StackPush(&stack, cur_node);
                if (cur_node->left == nullptr)
                {
                    cur_node->left = NodeCtor();
                    cur_node = cur_node->left;
                }
                else
                {
                    if (cur_node->right != nullptr)
                    {
                        _WRONG_EXPRESSION;
                    }
                    cur_node->right = NodeCtor();
                    cur_node = cur_node->right;
                }

                ++cur_symb;
            break;

            case ')':
                if (stack.size == 0)
                {
                    if (cur_symb - prog->expression.buffer != prog->expression.size - 1)
                    {
                        _WRONG_EXPRESSION;    
                    }

                    break;
                }
                cur_node = StackPop(&stack);

                ++cur_symb;
            break;

            case '+':
                if (*(cur_symb + 1) != '(')
                {
                    _WRONG_EXPRESSION;
                }
                cur_node->value = Operators::ADD;
                cur_node->type  = NodeType::TYPE_OPERATOR;

                ++cur_symb;
            break;

            case '-':
                if (*(cur_symb + 1) != '(')
                {
                    _WRONG_EXPRESSION;
                }
                cur_node->value = Operators::SUB;
                cur_node->type  = NodeType::TYPE_OPERATOR;

                ++cur_symb;
            break;

            case '*':
                if (*(cur_symb + 1) != '(')
                {
                    _WRONG_EXPRESSION;
                }
                cur_node->value = Operators::MUL;
                cur_node->type  = NodeType::TYPE_OPERATOR;

                ++cur_symb;
            break;

            case '/':
                if (*(cur_symb + 1) != '(')
                {
                    _WRONG_EXPRESSION;
                }
                cur_node->value = Operators::DIV;
                cur_node->type  = NodeType::TYPE_OPERATOR;

                ++cur_symb;
            break;

            case '^':
                if (*(cur_symb + 1) != '(')
                {
                    _WRONG_EXPRESSION;
                }
                cur_node->value = Operators::POW;
                cur_node->type = NodeType::TYPE_OPERATOR;

                ++cur_symb;
            break;
        }

        if (isalpha(*cur_symb))
        {   
            if ((*(cur_symb + 1) != '(') && (*(cur_symb + 1) != ')'))
            {   
                _WRONG_EXPRESSION;    
            }

            if (*(cur_symb + 1) == '(')
            {
                switch (*cur_symb)
                {
                    case 's':
                        cur_node->value = Operators::SIN;
                    break;

                    case 'c':
                        cur_node->value = Operators::COS;
                    break;

                    case 't':
                        cur_node->value = Operators::TAN;
                    break;

                    case 'C':
                        cur_node->value = Operators::COT;
                    break;

                    case 'l':
                        cur_node->value = Operators::LN;
                    break;

                    default:
                        _WRONG_EXPRESSION;
                    break;
                }
                ++cur_symb;
            }

            cur_node->type = NodeType::TYPE_OPERATOR;

            if (*(cur_symb + 1) == ')')                                         
            {
                cur_node->value = *cur_symb;
                cur_node->type  = NodeType::TYPE_VARIABLE;    

                ++cur_symb;
            }
        }

        if (isdigit(*cur_symb))                                             
        {
            char *digit_beginning = cur_symb;
    
            while (isdigit(*cur_symb))
            {
                ++cur_symb;
            }
    
            cur_node->type = NodeType::TYPE_NUMBER;            

            char temp_symb = *cur_symb;
            if (temp_symb != ')')
            {
                _WRONG_EXPRESSION;
            }
            *cur_symb = '\0';
            cur_node->value = atoi(digit_beginning);

            *cur_symb = temp_symb;
        }
    }

    return OK;
}

void TreeDtor(Node *node)
{
    assert(node != nullptr);
    
    if (node->left != nullptr)
    {
        TreeDtor(node->left);
    }

    if (node->right != nullptr)
    {
        TreeDtor(node->right);
    }
}

Node *NewNode(NodeType node_type, int value, Node *left, Node *right)
{
    Node *node = (Node *) calloc(1, sizeof(Node));

    node->left  = left;
    node->right = right;
    node->value = value;
    node->type  = node_type;

    return node;
}

Node *NodeCtor()
{
    Node *node = (Node *) calloc(1, sizeof(Node));
    assert(node != nullptr);

    node->left  = nullptr;
    node->right = nullptr;
    node->value = POISON;
    node->type  = NodeType::TYPE_UNKNOWN;

    return node;
}

void NodeDtor(Node *node)
{
    assert(node != nullptr);

    node->left  = nullptr;
    node->right = nullptr;
    node->value = DESTRUCTED_POISON;
    node->type  = NodeType::TYPE_UNKNOWN;

    free(node);
}

void ReadExpression(Prog *prog)
{
    assert(prog != nullptr);

    prog->expression.buffer = (char *) calloc(MAX_EXPRESSION_LENGTH, sizeof(char));

    printf("Enter expression you want to take a derivative of. Its length should not be more than 512 symbols: ");
    scanf("%511s", prog->expression.buffer);

    prog->expression.size = strlen(prog->expression.buffer);
}

void DifferentiatorDoGraph(Node *node)
{
    assert (node != nullptr);
    
    FILE *graph_file = fopen(GRAPH_NAME, "w");
    assert(graph_file != nullptr);

    fprintf(graph_file, "digraph G{\n"
                        "    ");
    
    GraphNode(node, graph_file);
    
    fprintf(graph_file, "}");

    fclose(graph_file);

    char do_graph[MAX_GRAPH_COMMAND_LENGTH] = {};
    sprintf(do_graph, "dot -Tpng %s -o %s", GRAPH_NAME, IMG_NAME);
    system(do_graph);
}

void GraphNode(Node *node, FILE *graph_file)
{
    assert(node != nullptr);
    assert(graph_file != nullptr);
    
    if (node->left != nullptr)
    {
        GraphNode(node->left, graph_file);
    }
    
    if ((node->left != nullptr) && (node->right != nullptr))
    {
        fprintf(graph_file, "P%p[color=\"green\",shape=record, label=\"left \\n %p | %c \\n %p | type \\n %d | right \\n %p\"];\n    ", node, node->left, node->value, node, node->type, node->right);                      //operator
    }
    else if ((node->left != nullptr) && (node->right == nullptr))
    {
        fprintf(graph_file, "P%p[color=\"yellow\",shape=record, label=\"left \\n %p | %c \\n %p | type \\n %d | right \\n nill\"];\n    ", node, node->left, node->value, node, node->type);                                 //function
    }
    else if ((node->left == nullptr) && (node->right == nullptr))
    {
        if (node->type == NodeType::TYPE_VARIABLE)
        {
            fprintf(graph_file, "P%p[color=\"red\",shape=record, label=\"left \\n nill | %c \\n %p | type \\n %d | right \\n nill\"];\n    ", node, node->value, node, node->type);                                                   //variable
        }
        else
        {
            fprintf(graph_file, "P%p[color=\"blue\",shape=record, label=\"left \\n nill | %d \\n %p | type \\n %d | right \\n nill\"];\n    ", node, node->value, node, node->type);                                       //number kosyak
        }
    }

    if (node->right != nullptr)
    {
        GraphNode(node->right, graph_file);
    }

    if (node->left != nullptr)
    {
        fprintf(graph_file, "P%p->P%p[label=\"L\"];\n", node, node->left);
    }

    if (node->right != nullptr)
    {
        fprintf(graph_file, "P%p->P%p[label=\"R\"];\n", node, node->right);   
    }                                                                                       
}

Node *DifTree(Node *node)
{
    assert(node != nullptr);
    
    switch (node->type)
    {
        case NodeType::TYPE_NUMBER:
            return NUM(0);
        break;
        
        case NodeType::TYPE_VARIABLE:
            return NUM(1);
        break;

        case NodeType::TYPE_OPERATOR:
            switch (node->value)
            {
                case Operators::ADD:
                    return OP(Operators::ADD, DL, DR);       
                break;
                
                case Operators::SUB:
                    return OP(Operators::SUB, DL, DR);
                break;
                
                case Operators::MUL:
                    return OP(Operators::ADD, OP(Operators::MUL, DL, CR), 
                                              OP(Operators::MUL, CL, DR));
                break;

                case Operators::DIV:
                {
                    return OP(Operators::DIV, OP(Operators::POW, CR, NUM(2)), OP(Operators::SUB, 
                                              OP(Operators::MUL, DL, CR), OP(Operators::MUL, CL, DR)));
                }
                break;

                case Operators::POW:
                {
                    bool if_x_base   = SearchX(node->left);
                    bool if_x_degree = SearchX(node->right);
                   
                    if ((if_x_base == false) && (if_x_degree == false))
                    {
                        return NUM(0);
                    }

                    if ((if_x_base == true) && (if_x_degree == false))
                    {  
                        return OP(Operators::MUL, OP(Operators::MUL, OP(Operators::POW, CL, 
                                                  OP(Operators::SUB, CR, NUM(1))), 
                                                  CR), DL);
                    }

                    if ((if_x_base == false) && (if_x_degree == true))
                    {
                        return OP(Operators::MUL, OP(Operators::MUL, CopyTree(node), OP(Operators::LN, CR, nullptr)), DR);
                    }

                    if ((if_x_base == true) && (if_x_degree == true))
                    {
                        return OP(Operators::MUL, OP(Operators::POW, NUM(EXPONENTE), 
                                                  OP(Operators::MUL, CR, OP(Operators::LN, CL, nullptr))), 
                                                  OP(Operators::ADD, OP(Operators::MUL,  DR, OP(Operators::LN, CL, nullptr)), 
                                                  OP(Operators::MUL, CR, OP(Operators::MUL, OP(Operators::DIV, NUM(1), CL), DL))));
                    }
                }
                break;

                case Operators::SIN:
                    return OP(Operators::MUL, OP(Operators::COS, CL, nullptr), DL);
                break;

                case Operators::COS:
                    return OP(Operators::MUL, OP(Operators::MUL, OP(Operators::SIN, CL, nullptr), DL), NUM(-1));
                break;
                
                case Operators::TAN:
                    return OP(Operators::MUL, OP(Operators::DIV, NUM(1), OP(Operators::POW, OP(Operators::COS, CL, nullptr), NUM(2))), DL);
                break;

                case Operators::COT:
                    return OP(Operators::MUL, OP(Operators::MUL, OP(Operators::DIV, NUM(1), OP(Operators::POW, OP(Operators::SIN, CL, nullptr), NUM(2))), DL), NUM(-1));
                break;

                case Operators::LN:
                    return OP(Operators::MUL, OP(Operators::DIV, NUM(1), CL), DL);
                break;
            }
        break;
    }
}

bool SearchX(Node *node)
{
    assert(node != nullptr);

    if (node->type == NodeType::TYPE_VARIABLE)
    {
        return true;
    }
    
    if (node->left != nullptr)
    {
        if (SearchX(node->left))
        {
            return true;
        }
    }

    if (node->right != nullptr)
    {
        if (SearchX(node->right))
        {
            return false;
        }
    }

    return false;
}

Node *DifMul(Node *node)
{
    assert (node != nullptr);

    return OP(Operators::ADD, OP(Operators::MUL, DL, CR), 
                              OP(Operators::MUL, CL, DR));
}

Node *CopyTree(Node *node)
{
    assert(node != nullptr);

    Node *new_node  = NewNode(node->type, node->value, nullptr, nullptr);
    if (node->left  != nullptr)
    {
        new_node->left  = CopyTree(node->left);
    }   

    if (node->right != nullptr)
    {   
        new_node->right = CopyTree(node->right);
    }

    return new_node;
}

Node *NewOperation(int value, Node *left, Node *right)
{
    assert(left  != nullptr);
    assert(right != nullptr);
    
    Node *node = NewNode(NodeType::TYPE_OPERATOR, value, left, right);
    
    return node;
}

void Simplify(Node **node)
{
    assert(node != nullptr);

    bool if_changes = false;
    Stack_t stack = {};
    StackCtor(&stack, MAX_TREE_DEPTH);
    
    int phrases_counter = 1;
    do
    {
        StackDtor(&stack);
        StackCtor(&stack, MAX_TREE_DEPTH);
        if_changes = false;

        Simp1(*node, &if_changes);
        Simp2(node, &stack, &if_changes);

        if (if_changes)
        {
            if (phrases_counter == 1)
            {
                fprintf(tex_file, "Упростим:\\\\ \n");
                phrases_counter = 2;
            }
            else if (phrases_counter == 2)
            {
                fprintf(tex_file, "И еще упростим: \\\\ \n");
                phrases_counter = 3;
            }
            else if (phrases_counter == 3)
            {
                fprintf(tex_file, "Ну, это упрощение вообще не шокирует: \\\\ \n");
                phrases_counter = 1;
            }

            DumpStrWithSpace(tex_file, *node);
        }
    } 
    while (if_changes);
    
}

void Simp1(Node *node, bool *if_changes)
{
    assert(node != nullptr);
    assert(if_changes != nullptr);

    if (node->left != nullptr)
    {
        Simp1(node->left, if_changes); 
    }
    
    if ((node->left != nullptr) && (node->right != nullptr) && (node->left->type == NodeType::TYPE_NUMBER) && (node->right->type == NodeType::TYPE_NUMBER))            
    {
        *if_changes = true;
        
        node->type = NodeType::TYPE_NUMBER;
        
        switch (node->value)
        {
            case Operators::ADD:
                node->value = node->left->value + node->right->value;
            break; 

            case Operators::SUB:
                node->value = node->left->value - node->right->value;
            break;
            
            case Operators::MUL:
                node->value = node->left->value * node->right->value;
            break;

            case Operators::DIV:
                node->value = node->left->value / node->right->value;
            break;

            case Operators::POW:
                node->value = pow(node->left->value, node->right->value);
            break;
        }

        NodeDtor(node->left);
        NodeDtor(node->right);

        node->left  = nullptr;
        node->right = nullptr;
    }

    if ((node->type == NodeType::TYPE_OPERATOR) && (isalpha(node->value)) && (node->left != nullptr) && (node->left->type == NodeType::TYPE_NUMBER))
    {
        *if_changes = true;
        
        node->type = NodeType::TYPE_NUMBER;
        
        switch (node->left->value)
        {
            case Operators::SIN:
                node->value = sin(node->left->value);
            break;

            case Operators::COS:
                node->value = cos(node->left->value);
            break;

            case Operators::TAN:
                node->value = tan(node->left->value);
            break;

            case Operators::COT:
                node->value = 1 / tan(node->left->value);
            break;

            case Operators::LN:
                node->value = logf(node->right->value) / logf(node->left->value);
            break;
        }

        NodeDtor(node->left);

        node->left = nullptr;
    }
    
    if (node->right != nullptr)
    {
        Simp1(node->right, if_changes); 
    }

    return;
}

void Simp2(Node **node, Stack_t *stack, bool *if_changes)
{
    assert(node != nullptr);
    assert(stack != nullptr);
    assert(if_changes != nullptr);

    StackPush(stack, *node);

    if ((*node)->left != nullptr)
    {
        Simp2(&((*node)->left), stack, if_changes);
    }

    if ((*node)->type == NodeType::TYPE_OPERATOR)
    {
        if (((*node)->value == Operators::ADD) || ((*node)->value == Operators::SUB))                             // add & sub
        {
            if (((*node)->left != nullptr) && ((*node)->right != nullptr))
            {
                if (((*node)->left->type == NodeType::TYPE_NUMBER) && ((*node)->left->value == 0))
                {
                    *if_changes = true;

                    if ((*node)->value == Operators::ADD)
                    {   
                        if (stack->size == 1)
                        {
                            TreeDtor((*node)->left);

                            *node = (*node)->right;

                            return;
                        }
                        else if (stack->data[stack->size - 2]->left  == *node)
                        {   
                            stack->data[stack->size - 2]->left  = CopyTree((*node)->right);
                            
                        }
                        else if (stack->data[stack->size - 2]->right == *node)
                        {
                            stack->data[stack->size - 2]->right = CopyTree((*node)->right);
                        }
                    
                        TreeDtor(*node);
                        
                    }
                    else
                    {
                        (*node)->value = Operators::MUL;
                        (*node)->left->value = -1;
                    }
                }
                else if (((*node)->right->type == NodeType::TYPE_NUMBER) && ((*node)->right->value == 0))
                {
                    *if_changes = true;
                    
                    if (stack->size == 1)
                    {
                        TreeDtor((*node)->right);

                        *node = (*node)->left;

                        return;
                    }
                    else if (stack->data[stack->size - 2]->left  == *node)
                    {
                        stack->data[stack->size - 2]->left = CopyTree((*node)->left);
                    }
                    else if (stack->data[stack->size - 2]->right == *node)
                    {
                        stack->data[stack->size - 2]->right = CopyTree((*node)->left);
                    }

                    TreeDtor(*node);
                }
            }
        }

        if ((*node)->value == Operators::MUL)                                                                                                                                                               // mul
        {
            if (((*node)->left != nullptr) && ((*node)->right != nullptr))
            {
                if ((((*node)->left->type == NodeType::TYPE_NUMBER) && ((*node)->left->value == 0)) || (((*node)->right->type == NodeType::TYPE_NUMBER) && ((*node)->right->value == 0)))                   // *0
                {
                    *if_changes = true;
                    
                    TreeDtor(*node);
                    
                    if (stack->size == 1)
                    {
                        *node = NUM(0);

                        return;
                    }
                    else if (stack->data[stack->size - 2]->left  == *node)
                    {
                        stack->data[stack->size - 2]->left = NUM(0);
                    }
                    else if (stack->data[stack->size - 2]->right == *node)
                    {
                        stack->data[stack->size - 2]->right = NUM(0);    
                    }
                }
                else if (((*node)->left->type == NodeType::TYPE_NUMBER) && ((*node)->left->value == 1))                                                                                               // *1
                {
                    *if_changes = true;
                    
                    if (stack->size == 1)
                    {
                        TreeDtor((*node)->left);

                        *node = (*node)->right;

                        return;
                    }
                    else if (stack->data[stack->size - 2]->left  == *node)
                    {
                        stack->data[stack->size - 2]->left = CopyTree((*node)->right);
                    }
                    else if (stack->data[stack->size - 2]->right == *node)
                    {
                        stack->data[stack->size - 2]->right = CopyTree((*node)->right);
                    }

                    TreeDtor(*node);
                }
                else if (((*node)->right->type == NodeType::TYPE_NUMBER) && ((*node)->right->value == 1))
                {
                    *if_changes = true;
                    
                    if (stack->size == 1)
                    {   
                        TreeDtor((*node)->right);

                        *node = (*node)->left;

                        return;
                    }
                    else if (stack->data[stack->size - 2]->left  == *node)
                    {
                        stack->data[stack->size - 2]->left = CopyTree((*node)->left);
                    }
                    else if (stack->data[stack->size - 2]->right == *node)
                    {
                        stack->data[stack->size - 2]->right = CopyTree((*node)->left);
                    }
                    
                    TreeDtor(*node);
                }
            }            
        }

        if ((*node)->value == Operators::POW)                                                                                                                                  // pow
        {
            if (((*node)->left != nullptr) && ((*node)->right != nullptr))
            {
                if ((*node)->right->type == NodeType::TYPE_NUMBER)
                {   
                    if ((*node)->right->value == 0)
                    {
                        *if_changes = true;
                        
                        TreeDtor(*node);

                        if (stack->size == 1)
                        {
                            *node = NUM(1);

                            return;
                        }
                        else if (stack->data[stack->size - 2]->left == *node)
                        {
                            stack->data[stack->size - 2]->left = NUM(1);
                        }
                        else if (stack->data[stack->size - 2]->right == *node)
                        {
                            stack->data[stack->size - 2]->right = NUM(1);
                        }
                    }
                    else if ((*node)->right->value == 1)
                    {
                        *if_changes = true;
                        
                        if (stack->size == 1)
                        {
                            TreeDtor((*node)->right);

                            *node = CopyTree((*node)->left);

                            return;
                        }
                        else if (stack->data[stack->size - 2]->left == *node)
                        {
                            stack->data[stack->size - 2]->left = CopyTree((*node)->left);
                        }
                        else if (stack->data[stack->size - 2]->right == *node)
                        {
                            stack->data[stack->size - 2]->right = CopyTree((*node)->left);
                        }

                        TreeDtor(*node);
                    }
                }
            }
        }
    }

    if ((*node)->right != nullptr)
    {
        Simp2(&((*node)->right), stack, if_changes);
    }

    StackPop(stack);
}

void TexInit(FILE *tex_file)
{
    fprintf(tex_file, "\\documentclass [a4paper, 12x `pt]{article}\n"
                      "\\usepackage [utf8] {inputenc}\n"
                      "\\usepackage [T2A] {fontenc}\n"
                      "\\usepackage [russian] {babel}\n"
                      "\\usepackage {amsmath, amsfonts, amssymb, amsthm, mathtools, textcomp, stmaryrd}\n\n");
    
    fprintf(tex_file, "\\author{kirilliliych}\n"
                      "\\title{Производная}\n\n");

    fprintf(tex_file, "\\begin{document}\n\n");
}

void TexBeginning(FILE *tex_file, Node *node)
{
    assert(tex_file != nullptr);
    assert(node != nullptr);

    fprintf(tex_file, "Исходное выражение: \n");
    DumpStrWithSpace(tex_file, node);
}

void TexAfterDif(FILE *tex_file, Node *node)
{
    assert(tex_file != nullptr);
    assert(node != nullptr);

    fprintf(tex_file, "Производная исходного выражения: \n");
    DumpStrWithSpace(tex_file, node);
}

Priority GetPriority(Node *node)
{
    assert(node != nullptr);

    if ((node->type == NodeType::TYPE_NUMBER) || (node->type == NodeType::TYPE_VARIABLE))
    {
        return Priority::VAR_CONST; 
    }

    if (node->type == NodeType::TYPE_OPERATOR)
    {
        if ((node->value == Operators::SIN) || (node->value == Operators::COS) || (node->value == Operators::TAN) || 
            (node->value == Operators::COT) || (node->value == Operators::LN))
        {
            return Priority::FUNC;
        }

        if (node->value == Operators::POW)
        {
            return Priority::POW_OP;
        } 

        if ((node->value == Operators::MUL) || (node->value == Operators::DIV))
        {
            return Priority::MUL_DIV;
        }

        if ((node->value == Operators::ADD) || (node->value == Operators::SUB))
        {
            return Priority::ADD_SUB;
        }
    }
}

void DumpStr(FILE *tex_file, Node *node)
{
    assert(tex_file != nullptr);
    assert(node != nullptr);
    
    int node_priority = GetPriority(node);

    if ((node->type == NodeType::TYPE_OPERATOR) && (node_priority == Priority::FUNC))
    {
        switch (node->value)
        {
            case Operators::SIN:
                fprintf(tex_file, "sin");
            break;

            case Operators::COS:
                fprintf(tex_file, "cos");
            break;

            case Operators::TAN:
                fprintf(tex_file, "tan");
            break;

            case Operators::COT:
                fprintf(tex_file, "cot");
            break;

            case Operators::LN:
                fprintf(tex_file, "ln");
            break;
        }
    }

    if (node->left != nullptr)
    {
        if (node_priority > GetPriority(node->left))
        {
            if (node_priority == Priority::FUNC)
            {
                fprintf(tex_file, "(");
            }
            else
            {
                fprintf(tex_file, " (");
            }
        }

        DumpStr(tex_file, node->left);
    }
    
    if (node_priority != Priority::FUNC)
    {
        if (node->type == NodeType::TYPE_OPERATOR)
        {
            fprintf(tex_file, " %c ", node->value);

            if (node->value == '^')
            {
                fprintf(tex_file, "{");
            }
        }
        else if (node->type == NodeType::TYPE_VARIABLE)
        {
            fprintf(tex_file, "%c", node->value);
        }
        else if (node->type == NodeType::TYPE_NUMBER)
        {
            fprintf(tex_file, "%d", node->value);
        }
    }
    
    if (node->left != nullptr)
    {
        if (node_priority > GetPriority(node->left))
        {
            if (node_priority == Priority::FUNC)
            {
                fprintf(tex_file, ")");
            }
            else
            {
                fprintf(tex_file, ") ");
            }
        }
    }

    if (node->right != nullptr)
    {
        if (node_priority > GetPriority(node->right))
        {
            if (node_priority == Priority::FUNC)
            {
                fprintf(tex_file, "(");
            }
            else
            {
                fprintf(tex_file, " (");
            }
        }

        DumpStr(tex_file, node->right);
    }

    if (node->right != nullptr)
    {
        if (node_priority > GetPriority(node->right))
        {
            if (node_priority == Priority::FUNC)
            {
                fprintf(tex_file, ")");
            }
            else
            {
                fprintf(tex_file, ") ");
            }
        }

        if (node->value == '^')
        {
            fprintf(tex_file, "}");
        }
    }
}

void DumpStrWithSpace(FILE *tex_file, Node *node)
{
    fprintf(tex_file, "$");
    DumpStr(tex_file, node);
    fprintf(tex_file, "$");
    fprintf(tex_file, "\\\\");
    fprintf(tex_file, "\n\n");
}

void TexEnd(FILE *tex_file)
{
    assert(tex_file != nullptr);

    fprintf(tex_file, "\\end{document}\n\n");

    fclose(tex_file);
}

void Tex(FILE *tex_file, Node *node)
{
    assert(tex_file != nullptr);
    assert(node != nullptr);

    TexInit(tex_file);
    DumpStrWithSpace(tex_file, node);
    TexEnd(tex_file);
}
