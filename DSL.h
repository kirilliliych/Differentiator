#ifndef DSL_H
#define DSL_H

#define DL DifTree(node->left)

#define DR DifTree(node->right)

#define CL CopyTree(node->left)

#define CR CopyTree(node->right)

#define NUM(value) NewNode(NodeType::TYPE_NUMBER, value, nullptr, nullptr)

#define OP(operation, left, right) NewNode(NodeType::TYPE_OPERATOR, operation, left, right)

#endif