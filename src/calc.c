/** @file
  Klasa wielomianów rzadkich wielu zmiennych

  @authors Joanna Dagil <jd417531@students.mimuw.edu.pl>
  @date 2021
*/

#define _GNU_SOURCE

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "poly.h"

#define DELIMITERS " \n"
#define STARTING_SIZE 64

typedef struct Stack {
  Poly poly;
  struct Stack *next;
} Stack;

Stack *stack;

void PolyPush(Poly p) {
  Stack *new = malloc(sizeof(Stack));
  new->next = stack;
  new->poly = p;
  stack = new;
}

void StackFree() {
  while(stack != NULL) {
    PolyDestroy(&(stack->poly));
    stack = stack->next;
  }
}

void PrintPoly(Poly *poly);

void MonoPrint(Mono *mono) {
  printf("(");
  PrintPoly(&(mono->p));
  printf(",");
  printf("%d", mono->exp);
  printf(")");
}

/**
 * wypisuje na standardowe wyjście wielomian z wierzchołka stosu
 */
void PrintPoly(Poly *poly) {
  if(!poly->arr)
    printf("%ld", poly->coeff);
  else {
    //printf("(");
    for(size_t i = 0; i < poly->size - 1; i++) {
      MonoPrint(&((poly->arr)[i]));
      printf("+");
    }
    MonoPrint(&((poly->arr)[poly->size - 1]));
    //printf(")");
  }
}

void PRINT(Poly *poly) {
  PrintPoly(poly);
  printf("\n");
}

Poly ProcessPoly(char **line);

Mono ProcessMono(char **line) {
  Mono mono;
  (*line)++; // (
  if(**line == '(') {
    mono.p = ProcessPoly(line); // p
  } else {
    mono.p = PolyFromCoeff(atol(*line));
    while(*line && **line != ',') // p
      (*line)++;
  }
  (*line)++; // ,
  mono.exp = atoi(*line);
  while(**line && **line != ')') // exp
      (*line)++;
  (*line)++; // )
  return mono;
}

Poly ProcessPoly(char **line) {
  Poly poly;
  if(**line == '(') {
    size_t size = STARTING_SIZE;
    size_t i = 1;
    Mono *monos = malloc(size * sizeof(Mono));
    monos[0] = ProcessMono(line); //(mono)
    while(**line && **line == '+') {
      if(i == size) {
        size *= 2;
        monos = realloc(monos, size * sizeof(Mono));
      }
      (*line)++; // +
      monos[i] = ProcessMono(line); // (mono)
      i++;
    }
    monos = realloc(monos, i * sizeof(Mono));
    poly = PolyAddMonos(i, monos);
  } else {
    poly = PolyFromCoeff(atol(*line));
    while(**line && **line != ',') 
      (*line)++;
  }
  return poly;
}

void POP() {
  if(stack) {
    Stack *old_stack = stack;
    stack = stack->next;
    PolyDestroy(&stack->poly);
    free(old_stack);
  } else
    printf("ERROR w STACK UNDERFLOW\n");
}

void ADD() {
  if(stack && stack->next) {
    Poly new = PolyAdd(&stack->poly, &stack->next->poly);
    POP(); POP();
    PolyPush(new);
  } else
    printf("ERROR w STACK UNDERFLOW\n");
}

void MUL() {
  if(stack && stack->next) {
    Poly new = PolyMul(&stack->poly, &stack->next->poly);
    POP(); POP();
    PolyPush(new);
  } else
    printf("ERROR w STACK UNDERFLOW\n");
}

void NEG() {
  if(stack) {
    Poly new = PolyNeg(&stack->poly);
    POP();
    PolyPush(new);
  } else
    printf("ERROR w STACK UNDERFLOW\n");
}

void SUB() {
  if(stack && stack->next) {
    Poly new = PolySub(&stack->poly, &stack->next->poly);
    POP(); POP();
    PolyPush(new);
  } else
    printf("ERROR w STACK UNDERFLOW\n");
}

void IS_EQ() {
  if(stack && stack->next)
    printf("%d\n", PolyIsEq(&stack->poly, &stack->next->poly));
  else
    printf("ERROR w STACK UNDERFLOW\n");
}

void DEG() {
  if(stack) {
    printf("%d\n", PolyDeg(&stack->poly));
  } else
    printf("ERROR w STACK UNDERFLOW\n");
}

void ProcessCommand(char **line) {
  char print[] = "PRINT", pop[] = "POP", deg[] = "DEG", is_eq[] = "IS_EQ";
  char sub[] = "SUB", neg[] = "NEG", mul[] = "MUL", add[] = "ADD", zero[] = "ZERO";
  char clone[] = "CLONE", is_zero[] = "IS_ZERO", is_coeff[] = "IS_COEFF";
  if(strcmp(*line, print) == 0){
    PRINT(&stack->poly);
    printf("prited\n");
  }
  else if(strcmp(*line, pop) == 0)
    POP();
  else if(strcmp(*line, deg) == 0)
    DEG();
  else if(strcmp(*line, is_eq) == 0)
    IS_EQ();
  else if(strcmp(*line, sub) == 0)
    SUB();    
  else if(strcmp(*line, neg) == 0)
    NEG();
  else if(strcmp(*line, mul) == 0)
    MUL();
  else if(strcmp(*line, add) == 0)
    ADD();
  else if(strcmp(*line, zero) == 0)
    PolyPush(PolyZero());
  else if(strcmp(*line, clone) == 0)
    PolyPush(PolyClone(&stack->poly));
  else if(strcmp(*line, is_zero) == 0)
    printf("%d\n", PolyIsZero(&stack->poly));
  else if(strcmp(*line, is_coeff) == 0)
    printf("%d\n", PolyIsCoeff(&stack->poly));
  printf("%s\n", *line);
}

int ProcessLine(char **line) {
  //char* word = strtok(line, DELIMITERS);
  if('A' <= **line && **line <= 'Z') {
    ProcessCommand(line);
  }
  else 
    PolyPush(ProcessPoly(line));
  return 1;
}


int main() {
  stack = NULL;
  int read;
  char* line = NULL;
  char* line_saver;
  size_t size;// = STARTING_SIZE;
  while((read = getline(&line, &size, stdin)) != -1) {
    if(errno  == ENOMEM) exit(1);
    line_saver = line;
    int valid = ProcessLine(&line);
    valid++;
    line = line_saver;
  }
  free(line);
  PRINT(&(stack->poly));


  return 0;
}