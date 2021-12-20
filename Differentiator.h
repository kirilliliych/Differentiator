#ifndef DIFFERENTIATOR_H
#define DIFFERENTIATOR_H

#include <assert.h>
#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Stack.h"
#include "DSL.h"

#define _WRONG_EXPRESSION                                                                                               \
    printf("Expression is wrong: error in place %d\n", cur_symb - prog->expression.buffer);                             \
    printf("LINE: %d\n", __LINE__);                                                                                     \
    return WRONG_EXPRESSION;


const int POISON = -666;
const int DESTRUCTED_POISON = -228;
const int MAX_GRAPH_COMMAND_LENGTH = 100; 
const int MAX_EXPRESSION_LENGTH = 512;
const int MAX_TREE_DEPTH = 200;
const int EXPONENTE = 2.71828;
const int MAX_FILE_NAME_LENGTH = 200;

const char TEX_FILE_NAME[MAX_FILE_NAME_LENGTH] = "TakenDerivative.tex"; 

FILE *tex_file = fopen(TEX_FILE_NAME, "w");

enum Errors
{
    OK = 0,
    WRONG_EXPRESSION
};

struct Tree
{
    Node *root = nullptr; 
};

struct Expression
{
    int size = 0;
    char *buffer = nullptr;
};

struct Prog
{
    Tree tree = {};
    Expression expression = {};
};

void ProgDtor(Prog *prog);

int TreeCtor(Prog *prog);

void TreeDtor(Node *node);

Node *NewNode(NodeType node_type, int value, Node *left, Node *right);

Node *NodeCtor();

void NodeDtor(Node *node);

void ReadExpression(Prog *prog);

void DifferentiatorDoGraph(Node *node);

void GraphNode(Node *node, FILE *graph_file);

bool IsFunction(char *cur_symb);

Node *DifTree(Node *node);

Node *DifMul(Node *node);

Node *CopyTree(Node *node);

Node *NewOperation(int value, Node *left, Node *right);

bool SearchX(Node *node);

void Simplify(Node **node);

void Simp1(Node *node, bool *if_changes);

void Simp2(Node **node, Stack_t *stack, bool *if_changes);

Priority GetPriority(Node *node);

void TexInit(FILE *tex_file);

void TexBeginning(FILE *tex_file, Node *node);

void TexAfterDif(FILE *tex_file, Node *node);

void DumpStr(FILE *tex_file, Node *node);

void DumpStrWithSpace(FILE *tex_file, Node *node);

void TexEnd(FILE *tex_file);

void Tex(FILE *tex_file, Node *node);

#endif