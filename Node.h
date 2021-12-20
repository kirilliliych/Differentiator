#ifndef NODE_H
#define NODE_H

enum NodeType
{
    TYPE_UNKNOWN  = 0,
    TYPE_NUMBER   = 1,
    TYPE_VARIABLE = 2,
    TYPE_OPERATOR = 3
};

enum Operators 
{
    ADD = '+',
    SUB = '-',
    MUL = '*',
    DIV = '/',
    POW = '^',
    SIN = 's',
    COS = 'c',
    TAN = 't',
    COT = 'C',
    LN  = 'l'
};

enum Priority
{
    ADD_SUB   = 1,
    MUL_DIV   = 2,
    POW_OP    = 3,
    VAR_CONST = 4,
    FUNC = 5
};

struct Node
{
    Node *left  = nullptr;
    Node *right = nullptr;
    int value   = -1488;
    NodeType type  = TYPE_UNKNOWN;
};

#endif