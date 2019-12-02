#include <string>
#include <iostream>
#include <fstream>
#include "codeGen.h"

FILE* outFile;

int varCounter = 0;
char newVar[9];

int labelCounter = 0;
char newLabel[9];

char* createdVars[1000];
int createdVarsCounter = 0;

void createVar(char* code);
char* createTempVar();
char* createLabel();

void printVarsToFile();

void generateProgram (Node * node);
void generateBlock (Node * node);
void generateVars (Node * node);
void generateExpr (Node * node);
void generateA (Node * node);
void generateM (Node * node);
void generateR (Node * node);
void generateStats (Node * node);
void generateMStat (Node * node);
void generateStat (Node * node);
void generateIn (Node * node);
void generateOut (Node * node);
void generateIfGram (Node * node);
void generateLoop (Node * node);
void generateAssign (Node * node);

void printNode (Node * node);

void generateCode(Node* node) {
        std::cout << "Starting Code Generation:\n";
        if (node == NULL) {
            return; 
        }
        generateProgram(node);
        fprintf(outFile, "STOP\n");
        printVarsToFile();
}

void generateProgram (Node * node){
    if (node == NULL) {
        return; 
    }

    generateVars(node->firstChild);
    generateBlock(node->secondChild);
    return;
}

void generateBlock (Node * node){
    if (node == NULL) {
        return; 
    }
    
    generateVars(node->firstChild);
    generateStats(node->secondChild);
    return;
}

void generateVars (Node * node){
    if (node == NULL) {
        return; 
    }
    createVar(node->firstToken.identiToken);
    generateVars(node->firstChild);
    return;
}

void generateExpr (Node * node){
    if (node == NULL) {
        return; 
    }

    if (node->secondChild != NULL) {
        generateExpr(node->secondChild);
        char* tempVar = createTempVar();
        fprintf(outFile, "STORE %s\n", tempVar);
        generateA(node->firstChild);
        if (strcmp(node->firstToken.identiToken, (char*)"/") == 0) {
            fprintf(outFile, "DIV %s\n", tempVar);
            return;
        } else {
            fprintf(outFile, "MULT %s\n", tempVar);
            return;
        }
    } else {
        generateA(node->firstChild);
        return;
    }
}

void generateA (Node * node) {
    if (node == NULL) {
        return; 
    }

    if (node->secondChild != NULL) {
        generateA(node->secondChild);
        char* tempVar = createTempVar();
        fprintf(outFile, "STORE %s\n", tempVar);
        generateM(node->firstChild);
        if (strcmp(node->firstToken.identiToken, (char*)"+") == 0) {
            fprintf(outFile, "ADD %s\n", tempVar);
            return;
        } else {
            fprintf(outFile, "SUB %s\n", tempVar);
            return;
        }
    } else {
        generateM(node->firstChild);
        return;
    }

}

void generateM (Node * node){
    if (node == NULL) {
        return; 
    }
    
    if(node->firstChild->nodeType == eR){
        generateR(node->firstChild);
        return;
    } else {
        generateM(node->firstChild);
        fprintf(outFile, "MULT -1\n");
        return;
    }
}

void generateR (Node * node){
    if (node == NULL) {
        return; 
    }
    if(node->firstChild != NULL){
        generateExpr(node->firstChild);
        return;
    } else if (node->firstToken.tokenId == idTk) {
        fprintf(outFile, "LOAD %s\n", node->firstToken.identiToken);
        return;
    } else if (node->firstToken.tokenId == intTk) {
        fprintf(outFile, "LOAD %s\n", node->firstToken.identiToken);
        return;
    }
}

void generateStats (Node * node){
    if (node == NULL) {
        return; 
    }
    
    generateStat(node->firstChild);
    generateMStat(node->secondChild);
    return;
}

void generateMStat (Node * node){
    if (node == NULL) {
        return; 
    }

    if (node->firstChild != NULL){
        generateStat(node->firstChild);
        generateMStat(node->secondChild);
        return;
    }
    return;
}

void generateStat (Node * node){
    if (node == NULL) {
        return; 
    }
    if (node->firstChild->nodeType == eIn){
        generateIn(node->firstChild);
        return;
    } else if (node->firstChild->nodeType == eOut){
        generateOut(node->firstChild);
        return;
    } else if (node->firstChild->nodeType == eBlock){
        generateBlock(node->firstChild);
        return;
    } else if (node->firstChild->nodeType == eIfGram){
        generateIfGram(node->firstChild);
        return;
    } else if (node->firstChild->nodeType == eLoop){
        generateLoop(node->firstChild);
        return;
    } else if (node->firstChild->nodeType == eAssign){
        generateAssign(node->firstChild);
        return;
    }
}

void generateIn (Node * node){
    if (node == NULL) {
        return; 
    }
    fprintf(outFile, "READ %s\n", node->firstToken.identiToken);
    return;
}

void generateOut (Node * node){
    if (node == NULL) {
        return; 
    }
    generateExpr(node->firstChild);
    char* tempVar = createTempVar();
    fprintf(outFile, "STORE %s\n", tempVar);
    fprintf(outFile, "WRITE %s\n", tempVar);
    return;
}

void generateIfGram (Node * node){
    if (node == NULL) {
        return; 
    }
    generateExpr(node->thirdChild);
    char* tempVar = createTempVar();
    fprintf(outFile, "STORE %s\n", tempVar);
    generateExpr(node->firstChild);
    fprintf(outFile, "SUB %s\n", tempVar);
    char* label = createLabel();
    if ((strcmp(node->secondChild->firstToken.identiToken, "<") == 0) && (strcmp(node->secondChild->secondToken.identiToken, "=") == 0)){
        fprintf(outFile, "BRPOS %s\n", label);
    } else if ((strcmp(node->secondChild->firstToken.identiToken,  ">") == 0) && (strcmp(node->secondChild->secondToken.identiToken, "=") == 0)){
        fprintf(outFile, "BRNEG %s\n", label);
    }  else if ((strcmp(node->secondChild->firstToken.identiToken, "=") == 0) && (strcmp(node->secondChild->secondToken.identiToken, "=") == 0)){
        fprintf(outFile, "BRZERO %s\n", label);
    } else if (strcmp(node->secondChild->firstToken.identiToken, "<") == 0){
        fprintf(outFile, "BRZPOS %s\n", label);
    } else if (strcmp(node->secondChild->firstToken.identiToken,  ">") == 0){
        fprintf(outFile, "BRZNEG %s\n", label);
    }  else if (strcmp(node->secondChild->firstToken.identiToken, "=") == 0){ 
        fprintf(outFile, "BRNEG %s\n", label);
        fprintf(outFile, "BRPOS %s\n", label);
    } 
    generateStat(node->fourthChild);
    fprintf(outFile, "%s: NOOP\n", label);
    return;
}

void generateLoop (Node * node){
    if (node == NULL) {
        return; 
    }
    char* label = createLabel();
    fprintf(outFile, "%s: NOOP\n", label);
    generateExpr(node->thirdChild);
    char* tempVar = createTempVar();
    fprintf(outFile, "STORE %s\n", tempVar);
    generateExpr(node->firstChild);
    fprintf(outFile, "SUB %s\n", tempVar);
    char* label2 = createLabel();
    if ((strcmp(node->secondChild->firstToken.identiToken, "<") == 0) && (strcmp(node->secondChild->secondToken.identiToken, "=") == 0)){
        fprintf(outFile, "BRZNEG %s\n", label2);
    } else if ((strcmp(node->secondChild->firstToken.identiToken,  ">") == 0) && (strcmp(node->secondChild->secondToken.identiToken, "=") == 0)){
        fprintf(outFile, "BRPOS %s\n", label2);
    }  else if ((strcmp(node->secondChild->firstToken.identiToken, "=") == 0) && (strcmp(node->secondChild->secondToken.identiToken, "=") == 0)){
        fprintf(outFile, "BRNEG %s\n", label2);
        fprintf(outFile, "BRPOS %s\n", label2);
    }else if (strcmp(node->secondChild->firstToken.identiToken, "<") == 0){
        fprintf(outFile, "BRNEG %s\n", label2);
    } else if (strcmp(node->secondChild->firstToken.identiToken,  ">") == 0){
        fprintf(outFile, "BRPOS %s\n", label2);
    }  else if (strcmp(node->secondChild->firstToken.identiToken, "=") == 0){
        fprintf(outFile, "BRZERO %s\n", label2);
    }
    generateStat(node->fourthChild);
    fprintf(outFile, "BR %s\n", label);
    fprintf(outFile, "%s: NOOP\n", label2);
    return;
}

void generateAssign (Node * node){
    if (node == NULL) {
        return; 
    }
    generateExpr(node->firstChild);
    fprintf(outFile, "STORE %s\n", node->firstToken.identiToken);
    return;
}

char* createTempVar(){
    sprintf(newVar, "v%d", varCounter);
    varCounter++;
    return newVar;
}

char* createLabel(){
    sprintf(newLabel, "l%d", labelCounter);
    labelCounter++;
    return newLabel;
}

void createVar(char* code){
    createdVars[createdVarsCounter] = code;
    createdVarsCounter++;
    return;
}

void printVarsToFile(){
    int i;
    for(i = 0; i < createdVarsCounter; i++){
        fprintf(outFile, "%s", createdVars[i]);
        fprintf(outFile, ": 0\n");
    }
    for(i = 0; i < varCounter; i++){
        sprintf(newVar, "v%d", i);
        fprintf(outFile, "%s", newVar);
        fprintf(outFile, ": 0\n");
    }
    // for(i = 0; i < labelCounter; i++){
    //     //     sprintf(newLabel, "l%d", i);
    //         //     fprintf(outFile, "%s", newLabel);
    //             //     fprintf(outFile, "\n");
    //                 // }
}
    //
    //                 void printNode (Node * node){
    //                     std::cout << getNodeTokenName(node->nodeType)
    //                         << ": " << getTokenName(node->firstToken.tokenId) 
    //                             << "/" << node->firstToken.identiToken
    //                                 << ": " << getTokenName(node->secondToken.tokenId) 
    //                                     << "/" << node->secondToken.identiToken
    //                                         << ": " << getTokenName(node->token2.tokenId) 
    //                                             << "/" << node->token2.identiToken
    //                                                 << "\n";
    //                                                     return;
    //                                                     }
