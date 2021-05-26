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

Stack* PolyPush(Poly p, Stack *s) {
  Stack *new = malloc(sizeof(Stack));
  new->next = s;
  new->poly = p;
  return new;
}

void StackFree() {
  while(stack != NULL) {
    PolyDestroy(&(stack->poly));
    stack = stack->next;
  }
}



/**
 * wstawia na wierzchołek stosu wielomian tożsamościowo równy zeru
 */
void ZERO() {
  stack = PolyPush(PolyZero(), stack);
}

void PRINT(Poly *poly);

void MonoPrint(Mono *mono) {
  printf("(");
  PRINT(&(mono->p));
  printf(",");
  printf("%d", mono->exp);
  printf(")");
}

/**
 * wypisuje na standardowe wyjście wielomian z wierzchołka stosu
 */
void PRINT(Poly *poly) {
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

Poly ProcessPoly(char **line);

Mono ProcessMono(char **line) {
  Mono mono;
  (*line)++;
  if(**line == '(') {
    mono.p = ProcessPoly(line);
  } else {
    mono.p = PolyFromCoeff(atol(*line));
    while(*line && **line != ',') 
      (*line)++;
  }
  (*line)++; // ,
  mono.exp = atoi(*line);
  (*line)++;
  return mono;
}

Poly ProcessPoly(char **line) {
  printf("in ProcessPoly\n");
  Poly poly;
  if(**line == '(') {
    printf("monos in ProcessPoly\n");
    size_t size = STARTING_SIZE;
    size_t i = 1;
    Mono *monos = malloc(size * sizeof(Mono));
    monos[0] = ProcessMono(line); //(mono)
    while(**line != ',') {
      if(i == size) {
        size *= 2;
        monos = realloc(monos, size * sizeof(Mono));
      }
      (*line)++; // +
      monos[i] = ProcessMono(line); // (mono)
      i++;
    }
    monos = realloc(monos, (i + 1) * sizeof(Mono));
    poly = PolyAddMonos(i, monos);
  } else {
    printf("Coeff in ProcessPoly\n");
    printf("%ld\n", atol(*line));
    poly = PolyFromCoeff(atol(*line));
    while(*line && **line != ',') 
      (*line)++;
  }
  printf("done\n");
  return poly;
}

int ProcessLine(char **line) {
  printf("opened ProcessLine\n");
  //char* word = strtok(line, DELIMITERS);
  if('A' <= **line && **line <= 'Z') {
    printf("a\n");
    //ProcessCommand(line);
  }
  else {
    printf("gonna go into ProessPoly\n");
    Poly poly = ProcessPoly(line);
    printf("ProcessPoly done\n");
    stack = PolyPush(poly, stack);
  }
  return 1;
}


int main() {
  printf("start\n");
  stack = NULL;
  int read;
  char* line;
  char* line_saver;
  size_t size = STARTING_SIZE;
  printf("just before getline\n");
  while((read = getline(&line, &size, stdin)) != -1) {
    if(errno  == ENOMEM) exit(1);
    printf("took getline\n");
    line_saver = line;
    int valid = ProcessLine(&line);
    valid++;
    line = line_saver;
  }
  //free(line);
  PRINT(&(stack->poly));


  return 0;
}