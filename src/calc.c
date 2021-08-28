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

void PRINT(bool *flow) {
  if(!stack) {
    *flow = false;
    return;
  }
  PrintPoly(&stack->poly);
  printf("\n");
}

Poly ProcessPoly(char **line, bool *valid, char* last);

bool is_zero(char *line, char character, char* last) {
  if(line && (*line == character || *line == '\n' || line == last))
    return false;
  while(line && *line == '0')
    line++;
  if(line && (*line == character || *line == '\n' || line == last))
    return true;
  return false;
}

// ( p , exp )
Mono ProcessMono(char **line, bool *valid, char* last) {
  //printf("zaczynam processowac mono\n");
  Mono mono;
  mono.p = PolyZero();
  mono.exp = 0;
  if(!*valid) return mono;
  // (
  if(*line && **line != '(') {
    *valid = false;
    return mono;
  }
  (*line)++; 
  //if(**line == '\n' || **line == 0)
  // p
  mono.p = ProcessPoly(line, valid, last);
  /*if(**line == '(') {
    mono.p = ProcessPoly(line, valid, last); 
  } else {
    mono.p = ProcessPoly(line, valid, last);
    //if(**line == 0 || **line == '\n')
    //  *valid = false;
    //else
    //printf("przed wejsciem do process poly\n");
    poly_coeff_t coeff = atol(*line);
    mono.p = PolyFromCoeff(coeff);
    if(coeff == 0 && !is_zero(*line, ',', last))
      *valid = false;
    while(*line && **line != ',') 
      (*line)++;
  }*/
  // ,
  if(**line != ',') {
    *valid = false;
    return mono;
  }
  (*line)++; 
  // exp
  //printf("przed exp\n");
  char* endptr;
  mono.exp = strtol(*line, &endptr, 10);
  if(errno == ERANGE) *valid = false;
  if(mono.exp == 0 && !is_zero(*line, ')', last))
    *valid = false;
  //printf("po exp\n");
  while('0' <= **line && **line <= '9') (*line)++;
  // )
  if(**line != ')') {
    *valid = false;
    return mono;
  }
  (*line)++;
  //if(!(**line == '+' || **line == '\n' || **line == 0)) *valid = false;
  //printf("returnuje mono\n");
  return mono;
}

// coeff
// or
// mono [ + mono]
Poly ProcessPoly(char **line, bool *valid, char* last) {
  //printf("  zaczynam processowac poly\n");
  Poly poly = PolyZero();
  if(!*valid) return poly;
  if(**line == '(') { //proper poly

    size_t size = STARTING_SIZE;
    size_t i = 1;
    Mono *monos = malloc(size * sizeof(Mono));

    //printf("  przed processowaniem pierwszego mono\n");
    monos[0] = ProcessMono(line, valid, last); //(mono)
    //printf("  po processowaniu pierwszego mono\n");
    if(**line == 0 && *line != last - 1) *valid = false;
    while(**line == '+' && !(*(*line + 1) == '\n' || *(*line + 1) == 0) && *valid) {
      if(i == size) {
        size *= 2;
        monos = realloc(monos, size * sizeof(Mono));//sprawdzic poprawnosc realloca
      }
      (*line)++; // +
      monos[i] = ProcessMono(line, valid, last); // (mono)
      i++;
    }
    if(**line == 0 && *line != last - 1) *valid = false;
    if(!(**line == ',' || **line == '\n' || **line == 0)) *valid = false;
    poly = PolyAddMonos(i, monos);
    free(monos);

  } else if(**line == '+' || **line == '-' || ('0' <= **line && **line <= '9')) {  //unproper 
    //poly_coeff_t coeff = atol(*line);
    //printf("  poly unproper\n");
    char* endptr;
    poly_coeff_t coeff = strtol(*line, &endptr, 10);
    if(errno == ERANGE) *valid = false;
    if(**line == '+' || **line == '-') (*line)++;
    if(coeff == 0 && !is_zero(*line, ',', last)) // not sure about that ','
      *valid = false;
    poly = PolyFromCoeff(coeff);
    while('0' <= **line && **line <= '9') (*line)++;
    //while(*line && **line != ',' && **line != '\n' && **line != 0) //jest endptr może uzyc niego a nie to?
    //  (*line)++;
    if(**line == 0 && *line != last - 1) *valid = false;
    if(!(**line == ',' || **line == '\n' || **line == 0)) *valid = false;
  } else {
    *valid = false;
    //printf("  false poly\n");
  }
  //printf("  returnuje poly\n");
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

void AT(char *line, long int no, char *last) {
  if(!(*(line + 2) == ' ' || *(line + 2) == '\n' || *(line + 2) == '\t')) {
    fprintf(stderr,"ERROR %ld WRONG COMMAND\n", no);
    return;
  }
  if(*(line + 2) != ' ' || *(line + 2) == 0 || *(line + 3) == '\n') {
    fprintf(stderr,"ERROR %ld AT WRONG VALUE\n", no);
    return;
  }
  line += 3;
  char *endptr;
  poly_coeff_t x = strtol(line, &endptr, 10);
  if(errno == ERANGE) {
    fprintf(stderr, "ERROR %ld AT WRONG VALUE\n", no);
    return;
  }
  if(*line == '+' || *line == '-') line++;
  if(x == 0 && !is_zero(line, '\n', last)) {
    fprintf(stderr, "ERROR %ld AT WRONG VALUE\n", no);
    return;
  }
  while('0' <= *line && *line <= '9') line++;
  if(!(*line == '\n' || line == last)) {
    fprintf(stderr, "ERROR %ld AT WRONG VALUE\n", no);
    return;
  } // bledny argument
  
  if(!stack) {
    fprintf(stderr, "ERROR %ld STACK UNDERFLOW\n", no);
    return;
  }

  Poly new = PolyAt(&stack->poly, x);
  bool flow; // blank bc we already checked that
  POP(&flow);
  PolyPush(new);
}

void DEG_BY(char *line, long int no, char* last) {
  if(!(*(line + 6) == ' ' || *(line + 6) == '\n')  || *(line + 6) == '\t') {
    fprintf(stderr,"ERROR %ld WRONG COMMAND\n", no);
    return;
  }
  if(*(line + 6) == '\n' || *(line + 7) == 0 || *(line + 7) == '\n') {
    fprintf(stderr,"ERROR %ld DEG BY WRONG VARIABLE\n", no);
    return;
  }
  line += 7;
  char* endptr;
  size_t idx = strtoul(line, &endptr, 10); // 10? idk what that does
  //printf("%ld\n", idx);
  if(errno == ERANGE) {
    fprintf(stderr, "ERROR %ld DEG BY WRONG VARIABLE\n", no);
    return;
  }
  while('0' <= *line && *line <= '9')
    line++;
  if(!(*line == '\n' || line == last)) {
    fprintf(stderr, "ERROR %ld DEG BY WRONG VARIABLE\n", no);
    return;
  } // bledny argument
  if(!stack) {
    fprintf(stderr, "ERROR %ld STACK UNDERFLOW\n", no);
    return;
  }
  //printf("%ld\n", idx);
  printf("%d\n", PolyDegBy(&(stack->poly), idx));
}

void CLONE(bool *flow) {
  if(stack)
    PolyPush(PolyClone(&stack->poly));
  else
    *flow = false;
}

void IS_COEFF(bool *flow) {
  if(stack)
    printf("%d\n", PolyIsCoeff(&stack->poly));
  else
    *flow = false;
}
void IS_ZERO(bool *flow) {
  if(stack)
    printf("%d\n", PolyIsZero(&stack->poly));
  else
    *flow = false;
}

void ProcessCommand(char *line, long int no, char* last) {
  char print[] = "PRINT\n", print2[] = "PRINT", pop[] = "POP\n", pop2[] = "POP", deg[] = "DEG\n", deg2[] = "DEG", is_eq[] = "IS_EQ\n", is_eq2[] = "IS_EQ";
  char sub[] = "SUB\n", neg[] = "NEG\n", mul[] = "MUL\n", add[] = "ADD\n", zero[] = "ZERO\n";
  char sub2[] = "SUB", neg2[] = "NEG", mul2[] = "MUL", add2[] = "ADD", zero2[] = "ZERO";
  char clone[] = "CLONE\n", is_zero[] = "IS_ZERO\n", is_coeff[] = "IS_COEFF\n";
  char clone2[] = "CLONE", is_zero2[] = "IS_ZERO", is_coeff2[] = "IS_COEFF";
  bool flow = true;

  char *line_temp = line;
  while(*line_temp != 0) 
    line_temp++;
  if(line_temp != last) {
    fprintf(stderr, "ERROR %ld WRONG COMMAND\n", no);
    return;
  }

  if(strcmp(line, print) == 0 || strcmp(line, print2) == 0)
    PRINT(&flow);
  else if(strcmp(line, pop) == 0 || strcmp(line, pop2) == 0)
    POP(&flow);
  else if(strcmp(line, deg) == 0 || strcmp(line, deg2) == 0)
    DEG(&flow);
  else if(strcmp(line, is_eq) == 0 || strcmp(line, is_eq2) == 0)
    IS_EQ(&flow);
  else if(strcmp(line, sub) == 0 || strcmp(line, sub2) == 0)
    SUB(&flow);    
  else if(strcmp(line, neg) == 0 || strcmp(line, neg2) == 0)
    NEG(&flow);
  else if(strcmp(line, mul) == 0 || strcmp(line, mul2) == 0)
    MUL(&flow);
  else if(strcmp(line, add) == 0 || strcmp(line, add2) == 0)
    ADD(&flow);
  else if(strcmp(line, zero) == 0 || strcmp(line, zero2) == 0)
    PolyPush(PolyZero());
  else if(strcmp(line, clone) == 0 || strcmp(line, clone2) == 0)
    CLONE(&flow);
  else if(strcmp(line, is_zero) == 0 || strcmp(line, is_zero2) == 0)
    IS_ZERO(&flow);
  else if(strcmp(line, is_coeff) == 0 || strcmp(line, is_coeff2) == 0)
    IS_COEFF(&flow);
  else if(line && *line == 'A' && 
          *(line + 1) == 'T')// &&
          //*(line + 2) == ' ')
    AT(line, no, last);
  else if(line && *line == 'D' && 
          *(line + 1) == 'E' &&
          *(line + 2) == 'G' &&
          *(line + 3) == '_' &&
          *(line + 4) == 'B' &&
          *(line + 5) == 'Y') //&&
          //*(line + 6) == ' ')
    DEG_BY(line, no, last);
  else
    fprintf(stderr, "ERROR %ld WRONG COMMAND\n", no);

  if(!flow) 
    fprintf(stderr, "ERROR %ld STACK UNDERFLOW\n", no);

}

void ProcessLine(char **line, long int no, char* last) {
  if(**line == '#' || **line == '\n') //ignoring
    return;
  if(('A' <= **line && **line <= 'Z') || ('a' <= **line && **line <= 'z'))
    ProcessCommand(*line, no, last);
  else {//(('0' <= **line && **line <= '9') || **line == '(') {
    bool valid = true;
    Poly poly = ProcessPoly(line, &valid, last);
    if(!(**line == 0 || **line == '\n')) valid = false;
    if(valid)
      PolyPush(poly);
    else 
      fprintf(stderr, "ERROR %ld WRONG POLY\n", no);
  }
    

}


int main() {
  stack = NULL;
  int read;
  char* last;
  char* line = NULL;
  char* save_line;
  long int no = 1;
  size_t size = STARTING_SIZE;

  while((read = getline(&line, &size, stdin)) != -1) {
    if(errno  == ENOMEM) exit(1);
    save_line = line;
    last = line + read;// - 1;
    ProcessLine(&save_line, no, last);
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

* obsluga pustego pliku

*/