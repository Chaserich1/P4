#ifndef CODEGEN_H
#define CODEGEN_H

#include <stdio.h>
#include "node.h"

extern FILE* outFile;

void generateCode(Node* node);

#endif
