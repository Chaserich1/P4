#include <string>
#include <iostream>
#include <fstream>
#include "codeGen.h"

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
void generateN(Node* node);
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

//void printNode (Node * node);

void generateCode(Node* node) {
   if(node == NULL){
      return; 
   }
   generateProgram(node);
   outFile << "STOP \n";
   printVarsToFile();
}

void generateProgram(Node* node){
   if(node == NULL){
      return; 
   }
   generateVars(node->firstChild);
   generateBlock(node->secondChild);
   return;
}

void generateBlock(Node* node){
   if(node == NULL){
      return; 
   }
   generateVars(node->firstChild);
   generateStats(node->secondChild);
   return;
}

void generateVars(Node* node){
   if(node == NULL){
      return; 
   }
   createVar(node->firstToken.tokenInstance);
   generateVars(node->firstChild);
   return;
}

void generateExpr(Node* node){
   if(node == NULL){
      return; 
   }

   if (node->secondChild != NULL) {
      generateExpr(node->secondChild);
      char* tempVar = createTempVar();
      outFile << "STORE " << tempVar << "\n";
      generateA(node->firstChild);
      if (node -> firstToken.identiToken == PlusTk) {
         outFile << "ADD " << tempVar << "\n";
         return;
      }
   }else{
      generateA(node->firstChild);
      return;
   }
}

void generateA(Node* node){
   if(node == NULL){
      return;
   }
   if(node -> secondChild != NULL){
      generateA(node -> secondChild);
      char* tempVar = createTempVar();
      outFile << "STORE " << tempVar << "\n";
      generateN(node -> firstChild);
      if(node -> firstToken.identiToken == MinusTk){
         outFile << "SUB " << tempVar << "\n";
         return;
      }
   }else{
      generateN(node -> firstChild);
      return;
   }
}

void generateN (Node* node) {
    if (node == NULL) {
        return; 
    }

    if (node->secondChild != NULL) {
        generateN(node->secondChild);
        char* tempVar = createTempVar();
        outFile << "STORE " << tempVar << "\n";
        generateM(node -> firstChild);
        if (node -> firstToken.identiToken == DivideTk){
            outFile << "DIV " << tempVar << "\n";
            return;
        } else {
            outFile << "MULT " << tempVar << "\n";
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
    
    if(node -> firstToken.identiToken == MinusTk){
      generateM(node -> firstChild);
      outFile << "MULT -1\n";
      return;
    }else{
      generateR(node -> firstChild);
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
    } else if (node->firstToken.identiToken == IdTk) {
         outFile << "LOAD " << node -> firstToken.stringToken << "\n";
        return;
    } else if (node->firstToken.identiToken == IntTk) {
         outFile << "LOAD " << node -> firstToken.stringToken << "\n";
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
    if (node->firstChild->nonTerminal == "<in>"){
        generateIn(node->firstChild);
        return;
    } else if (node->firstChild->nonTerminal == "<out>"){
        generateOut(node->firstChild);
        return;
    } else if (node->firstChild->nonTerminal == "<block>"){
        generateBlock(node->firstChild);
        return;
    } else if (node->firstChild->nonTerminal == "<if>"){
        generateIfGram(node->firstChild);
        return;
    } else if (node->firstChild->nonTerminal == "<loop>"){
        generateLoop(node->firstChild);
        return;
    } else if (node->firstChild->nonTerminal == "<assign>"){
        generateAssign(node->firstChild);
        return;
    }
}

void generateIn (Node * node){
    if (node == NULL) {
        return; 
    }
    outFile << "READ " << node-> firstToken.stringToken << "\n";
    return;
}

void generateOut (Node * node){
    if (node == NULL) {
        return; 
    }
    generateExpr(node->firstChild);
    char* tempVar = createTempVar();
    outFile << "STORE " << tempVar << "\n";
    outFile << "WRITE " << tempVar << "\n";
    return;
}


void generateIfGram(Node* node){
   if (node == NULL) {
      return; 
   }
   generateExpr(node -> thirdChild);
   char* tempVar = createTempVar();
   outFile << "STORE " << tempVar << "\n";
   generateExpr(node->firstChild);
   outFile << "SUB " << tempVar << "\n";
   char* label = createLabel();
   if((node -> secondChild -> firstToken.identiToken == LessThanTk) && (node -> secondChild -> secondToken.identiToken == LessThanTk)){
      outFile << "BRPOS " << label  << "\n";
   }
   else if((node -> secondChild -> firstToken.identiToken == GreaterThanTk) && (node -> secondChild -> secondToken.identiToken == GreaterThanTk)){
      outFile << "BRNEG " << label << "\n";
   }
   else if((node -> secondChild -> firstToken.identiToken == LessThanTk) && (node -> secondChild -> secondToken.identiToken == GreaterThanTk)){
      outFile << "BRZERO " << label << "\n";
   }
   else if(node -> secondChild -> firstToken.identiToken == LessThanTk){
      outFile << "BRZPOS " << label << "\n";
   }
   else if(node -> secondChild -> firstToken.identiToken == GreaterThanTk){
      outFile << "BRZNEG " << label << "\n";
   }
   else if(node -> secondChild -> firstToken.identiToken == EqualsTk){
      outFile << "BRNEG " << label << "\n";
      outFile << "BRPOS " << label << "\n";
   }
   generateStat(node -> fourthChild);
   outFile << label << ": NOOP\n";
   return;
}

void generateLoop (Node * node){
   if (node == NULL) {
      return; 
   }
   char* label = createLabel();
   outFile << label << ": NOOP\n";
   generateExpr(node->thirdChild);
   char* tempVar = createTempVar();
   outFile << "STORE " << tempVar << "\n";
   generateExpr(node->firstChild);
   outFile << "SUB " << tempVar << "\n";
   char* label2 = createLabel();
   if((node -> secondChild -> firstToken.identiToken == LessThanTk) && (node -> secondChild -> secondToken.identiToken == LessThanTk)){
      outFile << "BRZPOS " << label2 << "\n";
   }
   else if((node -> secondChild -> firstToken.identiToken == GreaterThanTk) && (node -> secondChild -> secondToken.identiToken == GreaterThanTk)){
      outFile << "BRZNEG " << label2 << "\n";
   }
   else if((node -> secondChild -> firstToken.identiToken == LessThanTk) && (node -> secondChild -> secondToken.identiToken == GreaterThanTk)){
      outFile << "BRZERO " << label2 << "\n";
   }
   else if(node -> secondChild -> firstToken.identiToken == LessThanTk){
      outFile << "BRPOS " << label2 << "\n";
   }                                                                                                   else if(node -> secondChild -> firstToken.identiToken == GreaterThanTk){
      outFile << "BRNEG " << label2 << "\n";
   }
   else if(node -> secondChild -> firstToken.identiToken == EqualsTk){
      outFile << "BRPOS " << label2 << "\n";
      outFile << "BRNEG " << label2 << "\n";
   }
   
   generateStat(node -> fourthChild);
   outFile << "BR " << label << "\n";
   outFile << label2 << ": NOOP\n";
   return;
}

void generateAssign (Node * node){
   if (node == NULL) {
      return; 
   }
   generateExpr(node->firstChild);
   outFile << "STORE " << node -> firstToken.stringToken << "\n";
   return;
}

char* createTempVar(){
   sprintf(newVar, "V%d", varCounter);
   varCounter++;
   return newVar;
}

char* createLabel(){
   sprintf(newLabel, "L%d", labelCounter);
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
      outFile << createdVars[i] << " 0\n";
   }
   for(i = 0; i < varCounter; i++){
      sprintf(newVar, "V%d", i);
      outFile << newVar << " 0\n";
   }
}
                                           
