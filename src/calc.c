/** @file
  Klasa wielomianów rzadkich wielu zmiennych

  @authors Joanna Dagil <jd417531@students.mimuw.edu.pl>
  @date 2021
*/

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>

#include "poly.h"
#include "stack.c"

#define DELIMITERS " \n"
#define STARTING_SIZE 64

typedef struct Stack {
  Poly poly;
  struct Stack *next;
} Stack;

Stack* PolyPush(Poly p, Stack *s) {
  Stack *new = malloc(sizeof(Stack));
  new->which = 0;
  new->next = s;
  new->poly = p;
  return new;
}

void StackFree() {
  while(stack != NULL) {
    PolyDestroy(stack->poly);
    stack = stack->next;
  }
}

Stack *stack;

/**
 * wstawia na wierzchołek stosu wielomian tożsamościowo równy zeru
 */
void ZERO() {
  stack = PolyPush(PolyZero(), stack);
}

void PolyPrint(Poly *poly);

void MonoPrint(Mono *mono) {
  printf("(");
  PolyPrint(&(m->p));
  printf(",");
  printf("%d", m->exp);
  printf(")");
}

/**
 * wypisuje na standardowe wyjście wielomian z wierzchołka stosu
 */
void PRINT(Poly *poly) {
  if(!poly->arr)
    printf("%ld", p->coeff);
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

Mono ProcessMono(char **line) {
  Mono mono;
  (*line)++;
  if((*line) == '(') {
    mono.p = ProcessPoly(line);
  } else {
    mono.p = PolyFromCoeff(atol(*line));
    while(*line && **line != ',') 
      (*line)++;
  }
  (*line)++; // ,
  mono.exp = atoi(*line);
  (*line)++;
}

Poly ProcessPoly(char **line) {
  Poly poly;
  if(**line == '(') {
    size_t size = STARTING_SIZE;
    size_t i = 1;
    Mono *monos = malloc(size * sizeof(Mono));
    monos[0] = ProcessMono(line); //(mono)
    while(**line != ',') {
      if(i == size) {
        size *= 2;
        monos = realloc(size * sizeof(Mono));
      }
      (*line)++; // +
      monos[i] = ProcessMono(line); // (mono)
      i++;
    }
    poly = PolyAddMonos(i, monos);
  } else {
    poly = PolyFromCoeff(atol(*line));
    while(*line && **line != ',') 
      (*line)++;
  }
  return poly;
}

int ProcessLine(char *line) {
  //char* word = strtok(line, DELIMITERS);
  if('A' <= *line && *line <= 'Z')
    ProcessCommand(line);
  else {
    Poly poly = ProcessPoly(line);
    stack = PolyPush(poly);
  }
}


int main() {
  stack = null;
  int read;
  char* line;
  size_t size = STARTING_SIZE;
  while((read = getline(&line, &size, stdin)) != -1) {
    // if(errno  == ENOMEM) exit(1);
    valid = ProcessLine(line);
  }
  free(line);
  PRINT(&(stack->poly));


  return 0;
}