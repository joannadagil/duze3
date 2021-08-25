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

Stack *stack = NULL;

void PolyPush(Poly p) {
  Stack *new = malloc(sizeof(Stack));
  new->next = stack;
  new->poly = p;
  stack = new;
}

void StackFree() {
  while(stack) {
    PolyDestroy(&(stack->poly));
    Stack *old = stack;
    stack = stack->next;
    free(old);
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

void PRINT() {
  PrintPoly(&stack->poly);
  printf("\n");
}

Poly ProcessPoly(char **line, bool *valid);

bool is_zero(char *line, char last) {
  if(line && (*line == last || *line == '\n'))
    return false;
  while(line && *line == '0')
    line++;
  if(line && (*line == last || *line == '\n'))
    return true;
  return false;
}

// ( p , exp )
Mono ProcessMono(char **line, bool *valid) {
  Mono mono;
  // (
  if(*line && **line != '(')
    *valid = false;
  (*line)++; 
  // p
  if(**line == '(') {
    mono.p = ProcessPoly(line, valid); 
  } else {
    poly_coeff_t coeff = atol(*line);
    mono.p = PolyFromCoeff(coeff);
    if(coeff == 0 && !is_zero(*line, ')'))
      *valid = false;
    while(*line && **line != ',') 
      (*line)++;
  }
  // ,
  if(**line != ',')
    *valid = false;
  (*line)++; 
  // exp
  mono.exp = atoi(*line);
  if(mono.exp == 0 && !is_zero(*line, ')'))
    *valid = false;

  while(**line != ')') 
      (*line)++;
  // )
  if(**line != ')')
    *valid = false;
  (*line)++;
  return mono;
}

// coeff
// or
// mono [ + mono]
Poly ProcessPoly(char **line, bool *valid) {
  Poly poly;
  if(**line == '(') { //proper poly
    size_t size = STARTING_SIZE;
    size_t i = 1;
    Mono *monos = malloc(size * sizeof(Mono));
    //printf("will start processing mono\n");
    monos[0] = ProcessMono(line, valid); //(mono)
    while(/**line && */**line == '+') {
      if(i == size) {
        size *= 2;
        monos = realloc(monos, size * sizeof(Mono));//sprawdzic poprawnosc realloca
      }
      (*line)++; // +
      monos[i] = ProcessMono(line, valid); // (mono)
      i++;
    }
    monos = realloc(monos, i * sizeof(Mono));//sprawdzic popr realloca
    poly = PolyAddMonos(i, monos);
    for(size_t j = 0; j < i; j++)
      MonoDestroy(&(monos[j]));
    free(monos);
  } else {  //unproper 
    poly_coeff_t coeff = atol(*line);
    if(coeff == 0 && (!is_zero(*line, ',') || !is_zero(*line, '\n') || !is_zero(*line, 0))) // not sure about that ','
      *valid = false;
    poly = PolyFromCoeff(coeff);
    while(*line && **line != ',' && **line != '\n' && **line != 0) 
      (*line)++;
  }
  return poly;
}

void POP(bool *flow) {
  if(stack) {
    Stack *old_stack = stack;
    stack = stack->next;
    PolyDestroy(&(old_stack->poly));
    free(old_stack);
  } else
    *flow = false;
}

void ADD(bool *flow) {
  if(stack && stack->next) {
    Poly new = PolyAdd(&stack->poly, &stack->next->poly);
    POP(flow); POP(flow);
    PolyPush(new);
  } else
    *flow = false;
}

void MUL(bool *flow) {
  if(stack && stack->next) {
    Poly new = PolyMul(&stack->poly, &stack->next->poly);
    POP(flow); POP(flow);
    PolyPush(new);
  } else
    *flow = false;
}

void NEG(bool *flow) {
  if(stack) {
    Poly new = PolyNeg(&stack->poly);
    POP(flow);
    PolyPush(new);
  } else
    *flow = false;
}

void SUB(bool *flow) {
  if(stack && stack->next) {
    Poly new = PolySub(&stack->poly, &stack->next->poly);
    POP(flow); POP(flow);
    PolyPush(new);
  } else
    *flow = false;
}

void IS_EQ(bool *flow) {
  if(stack && stack->next)
    printf("%d\n", PolyIsEq(&stack->poly, &stack->next->poly));
  else
    *flow = false;
}

void DEG(bool *flow) {
  if(stack) {
    printf("%d\n", PolyDeg(&stack->poly));
  } else
    *flow = false;
}

void AT(char *line, long int no) {
  if(!stack) {
    fprintf(stderr, "ERROR %ld STACK UNDERFLOW\n", no);
    return;
  }
  line += 3;
  poly_coeff_t x = atol(line);
  //przeskipować cyfry i sprawdzić czy enter
  if(*line == '\n') {
    fprintf(stderr, "ERROR %ld AT WRONG VALUE\n", no);
    return;
  } // brak argumentu
  while('0' <= *line && *line <= '9')
    line++;
  if(*line != '\n') {
    fprintf(stderr, "ERROR %ld AT WRONG VALUE\n", no);
    return;
  } // bledny argument
  if(x == 0 && !is_zero(line, '\n')) {
    fprintf(stderr, "ERROR %ld AT WRONG VALUE\n", no);
    return;
  }

  Poly new = PolyAt(&stack->poly, x);
  bool flow; // blank bc we already checked that
  POP(&flow);
  PolyPush(new);
}

void DEG_BY(char *line, long int no) {
  *line += 7;
  size_t idx = strtoul(line, 0, 10); // 10? idk what that does
  if(*line == '\n') {
    fprintf(stderr, "ERROR %ld DEG BY WRONG VARIABLE\n", no);
    return;
  } // brak argumentu
  while('0' <= *line && *line <= '9')
    line++;
  if(*line != '\n') {
    fprintf(stderr, "ERROR %ld DEG BY WRONG VARIABLE\n", no);
    return;
  } // bledny argument
  if(idx == 0 && !is_zero(line, '\n')) {
    fprintf(stderr, "ERROR %ld DEG BY WRONG VARIABLE\n", no);
    return;
  }
  printf("%d", PolyDegBy(&stack->poly, idx));
}

void ProcessCommand(char *line, long int no) {
  char print[] = "PRINT\n", pop[] = "POP\n", deg[] = "DEG\n", is_eq[] = "IS_EQ\n";
  char sub[] = "SUB\n", neg[] = "NEG\n", mul[] = "MUL\n", add[] = "ADD\n", zero[] = "ZERO\n";
  char clone[] = "CLONE\n", is_zero[] = "IS_ZERO\n", is_coeff[] = "IS_COEFF\n";
  bool flow = true;

  if(strcmp(line, print) == 0)
    PRINT();
  else if(strcmp(line, pop) == 0)
    POP(&flow);
  else if(strcmp(line, deg) == 0)
    DEG(&flow);
  else if(strcmp(line, is_eq) == 0)
    IS_EQ(&flow);
  else if(strcmp(line, sub) == 0)
    SUB(&flow);    
  else if(strcmp(line, neg) == 0)
    NEG(&flow);
  else if(strcmp(line, mul) == 0)
    MUL(&flow);
  else if(strcmp(line, add) == 0)
    ADD(&flow);
  else if(strcmp(line, zero) == 0)
    PolyPush(PolyZero());
  else if(strcmp(line, clone) == 0)
    PolyPush(PolyClone(&stack->poly));
  else if(strcmp(line, is_zero) == 0)
    printf("%d\n", PolyIsZero(&stack->poly));
  else if(strcmp(line, is_coeff) == 0)
    printf("%d\n", PolyIsCoeff(&stack->poly));
  else if(line && *line == 'A' && 
          *(line + 1) == 'T' &&
          *(line + 2) == ' ')
    AT(line, no);
  else if(line && *line == 'D' && 
          *(line + 1) == 'E' &&
          *(line + 2) == 'G' &&
          *(line + 3) == '_' &&
          *(line + 4) == 'B' &&
          *(line + 5) == 'Y' &&
          *(line + 6) == ' ')
    DEG_BY(line, no);
  else
    fprintf(stderr, "ERROR %ld WRONG COMMAND\n", no);

  if(!flow) 
    fprintf(stderr, "ERROR %ld STACK UNDERFLOW\n", no);

}

void ProcessLine(char **line, long int no) {
  if('A' <= **line && **line <= 'Z')
    ProcessCommand(*line, no);
  else if(**line == '#' || **line == '\n') //ignoring
    return;
  else {
    bool valid = true;
    Poly poly = ProcessPoly(line, &valid);
    if(valid)
      PolyPush(poly);
    else 
      fprintf(stderr, "ERROR %ld WRONG POLY\n", no);
  }
}


int main() {
  stack = NULL;
  int read;
  char* line = NULL;
  char* save_line;
  long int no = 1;
  size_t size = STARTING_SIZE;

  while((read = getline(&line, &size, stdin)) != -1) {
    if(errno  == ENOMEM) exit(1);
    save_line = line;

    ProcessLine(&save_line, no);
    no++;
  }

  free(line);
  StackFree();
  return 0;
}



/*
TODO

* atol i atoi nie mogą zawierać + i - a te funkcje to umożliwiaja

* process_poly jesli line wejsiowe to null

*/