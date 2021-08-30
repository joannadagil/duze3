/** @file
  Klasa wielomianów rzadkich wielu zmiennych

  @authors Joanna Dagil <jd417531@students.mimuw.edu.pl>
  @date 2021
*/

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#include <stdio.h> //usunac to ......................................

#include "poly.h"

static inline Mono* SafeMalloc(size_t size){
  Mono *ptr;
  ptr = malloc(size * sizeof(Mono));
  if(ptr == NULL) exit(1);
  return ptr;
}

static inline Poly* SafeMallocPoly(){
  Poly *ptr;
  ptr = malloc(sizeof(Poly));
  if(ptr == NULL) exit(1);
  return ptr;
}

void PolyDestroy(Poly *p){
  if(p->arr){
    for(size_t i = 0; i < p->size; i++){
      MonoDestroy(&((p->arr)[i]));
    }
    free(p->arr);
  }
}

Poly PolyClone(const Poly *p){
  Poly clone;
  if(p->arr){
    clone.size = p->size;
    clone.arr = SafeMalloc(p->size);
    for(size_t i = 0; i < p->size; i++){
      (clone.arr)[i] = MonoClone(&((p->arr)[i]));
    }
  }else{
    clone.coeff = p->coeff;
    clone.arr = NULL;
  }
  return clone;
}

Poly UnproperPoly(Poly *p){
  if(p->arr && p->size == 1 && !p->arr[0].p.arr){
    if(p->arr[0].exp == 0){
      Poly temp = p->arr[0].p;
      PolyDestroy(p);
      return temp;
    }
  }
  return *p;
}

Poly ProperPoly(const Poly *p){
  Poly proper = (Poly) {.size = 1, .arr = SafeMalloc(1)};
  (proper.arr)[0] = MonoFromPoly(p, 0);
  return proper;
}

Poly AddUnproperPolys(const Poly *p, const Poly *q){
  assert(!p->arr || !q->arr);
  if(!p->arr && !q->arr)
    return PolyFromCoeff(p->coeff + q->coeff);
  if(!p->arr){
    Poly temp = ProperPoly(p);
    Poly sum = PolyAdd(&temp, q);
    PolyDestroy(&temp);
    return sum;
  }
  return AddUnproperPolys(q, p);
}

Poly PolyAdd(const Poly *p, const Poly *q){
  if(!p->arr || !q->arr) return AddUnproperPolys(p, q);
  Poly sum;
  sum.arr = SafeMalloc(p->size + q->size);
  size_t ip = 0, iq = 0, isum = 0;
  while(ip < p->size || iq < q->size){
    if(ip < p->size && iq < q->size && (p->arr)[ip].exp == (q->arr)[iq].exp){
      (sum.arr)[isum] = (Mono) {.p = PolyAdd(&((p->arr)[ip].p), &((q->arr)[iq].p)), .exp = (p->arr)[ip].exp};
      if(!PolyIsZero(&((sum.arr)[isum].p))) isum++;
      ip++; iq++;
    }else if(iq == q->size || (ip < p->size && iq < q->size && (p->arr)[ip].exp < (q->arr)[iq].exp)){
      if(!PolyIsZero(&((p->arr)[ip].p))){
        (sum.arr)[isum] = MonoClone(&((p->arr)[ip]));
        isum++;
      }
      ip++;
    }else{ //ip == p->size || (ip < p->size && iq < q->size && (p->arr)[ip].exp > (q->arr)[iq].exp)
      if(!PolyIsZero(&((q->arr)[iq].p))){
        (sum.arr)[isum] = MonoClone(&((q->arr)[iq]));
        isum++;
      }
      iq++;
    }
  }
  sum.arr = realloc(sum.arr, isum * sizeof(struct Mono));
  sum.size = isum;
  sum = UnproperPoly(&sum);
  return sum;
}

static inline void MonosSwap(Mono *a, Mono *b){
  Mono temp = *a;
  *a = *b;
  *b = temp;
}

void MonosSort(Mono monos[], size_t min, size_t max){
  if(min < max){
    size_t mid = min;
    for(size_t i = min; i < max; i++){
      if((monos[i]).exp < (monos[max]).exp){
        MonosSwap(&(monos[mid]), &(monos[i]));
        mid++;
      }
    }
    MonosSwap(&(monos[mid]), &(monos[max]));
    MonosSort(monos, mid + 1, max);
    if(mid == 0) mid++; //zabezpieczenie przez wejściem na ujemne
    MonosSort(monos, min, mid - 1);
  }
}

Poly AddLast(size_t isum, size_t count, Mono last, Poly *sum){
  if(isum > 0 || count == 1 || !PolyIsZero(&(last.p))){
    if(!PolyIsZero(&(last.p))){
      (sum->arr)[isum] = last;
      isum++;
    }
    sum->arr = realloc(sum->arr, isum * sizeof(struct Mono));
    sum->size = isum;
  }else{
    free(sum->arr);
    sum->arr = NULL;
    sum->coeff = 0;
  }
  return *sum;
}

void freeing(size_t count, Mono monos2[], const Mono monos[]){
  for(size_t i = 0; i < count; i++){
    MonoDestroy(&(monos2[i]));
    if(monos[i].p.arr){
      for(size_t j = 0; j < monos[i].p.size; j++){
        MonoDestroy(&(monos[i].p.arr[j]));
      }
      free(monos[i].p.arr);
    }
  }
}

Poly PolyAddMonos(size_t count, const Mono monos[]){
  if(count == 0) return PolyZero();
  Mono monos2[count];
  for(size_t i = 0; i < count; i++)
    monos2[i] = MonoClone(&(monos[i]));
  MonosSort(monos2, 0, count - 1);
  Poly sum;
  sum.arr = SafeMalloc(count);
  size_t imonos = 1, isum = 0;
  Mono last = MonoClone(&(monos2[0]));
  while(imonos < count){
    if(last.exp == monos2[imonos].exp){
      Poly temp = PolyAdd(&(last.p), &(monos2[imonos].p));
      PolyDestroy(&(last.p));
      last.p = temp;
    }else{
      if(!PolyIsZero(&(last.p))){
        (sum.arr)[isum] = last;
        isum++;
      }
      last = MonoClone(&(monos2[imonos]));
    }
    imonos++;
  }
  sum = AddLast(isum, count, last, &sum);
  freeing(count, monos2, monos);
  sum = UnproperPoly(&sum);
  return sum;
}

Poly MulUnproperPolys(const Poly *p, const Poly *q){
  if(!p->arr && !q->arr) return PolyFromCoeff(p->coeff * q->coeff);
  if(!p->arr){
    Poly temp = ProperPoly(p);
    Poly mul = PolyMul(&temp, q);
    PolyDestroy(&temp);
    return mul;
  }
  assert(!q->arr);
  return MulUnproperPolys(q, p);
}

Poly PolyMul(const Poly *p, const Poly *q){
  if(!p->arr || !q->arr) return MulUnproperPolys(p, q);
  Mono monos[p->size * q->size];
  size_t imonos = 0;
  for(size_t ip = 0; ip < p->size; ip++){
    for(size_t iq = 0; iq < q->size; iq++){
      monos[imonos] = (Mono) {.p = PolyMul(&((p->arr)[ip].p), &((q->arr)[iq].p)), .exp = (p->arr[ip].exp + q->arr[iq].exp)};
      imonos++;
    }
  }
  Poly mul = PolyAddMonos(p->size * q->size, monos);
  return mul;
}

Poly PolyNeg(const Poly *p){
  Poly minus = (Poly) {.arr = NULL, .coeff = -1};
  Poly neg = PolyMul(&minus, p);
  PolyDestroy(&minus);
  return neg;
}

Poly PolySub(const Poly *p, const Poly *q){
  Poly neg = PolyNeg(q);
  Poly sub = PolyAdd(p, &neg);
  PolyDestroy(&neg);
  return sub;
}

static inline poly_coeff_t max(poly_coeff_t a, poly_coeff_t b){
  if(a > b) return a;
  return b;
}

poly_exp_t PolyDegBy(const Poly *p, size_t var_idx){
  if(PolyIsZero(p)) return -1;
  if(var_idx > 0){
    poly_coeff_t temp = -1;
    if(p->arr == NULL) return 0; //here changed
    for(size_t i = 0; i < p->size; i++)
      temp = max(temp, PolyDegBy(&(p->arr[i].p), var_idx - 1));
    return temp;
  }
  if(p->arr == NULL) return 0;
  return p->arr[p->size - 1].exp; //bo są posortowane rosnąco
}

poly_exp_t PolyDeg(const Poly *p){
  if(PolyIsZero(p)) return -1;
  if(p->arr == NULL) return 0;
  poly_coeff_t deg = 0;
  for(size_t i = 0; i < p->size; i++)
    deg = max(deg, p->arr[i].exp + PolyDeg(&(p->arr[i].p)));
  return deg;
}

bool MonoIsEq(const Mono *m, const Mono *n){
  return (m->exp == n->exp && PolyIsEq(&(m->p), &(n->p)));
}

bool PolyIsEq(const Poly *p, const Poly *q){
  if(!(p->arr) && !(q->arr) && p->coeff == q->coeff) return true;
  if(p->arr && q->arr && p->size == q->size){
    for(size_t i = 0; i < p->size; i++){
      if(!MonoIsEq(&((p->arr)[i]),&((q->arr)[i])))
        return false;
    }
    return true;
  }
  return false;
}

poly_coeff_t pow2(poly_coeff_t x, poly_exp_t exp){
  if(exp == 0) return 1;
  if(exp == 1) return x;
  poly_coeff_t res = pow2(x, exp / 2);
  if(exp % 2 == 1)
    return res * res * x;
  return res * res;
}

Poly PolyAt(const Poly *p, poly_coeff_t x){
  if(p->arr == NULL) return PolyClone(p);
  Poly res = PolyZero();
  for(size_t i = 0; i < p->size; i++){
    Poly temp0 = PolyFromCoeff(pow2(x, p->arr[i].exp));
    Poly temp1 = PolyMul(&(p->arr[i].p), &temp0);
    Poly temp2 = PolyAdd(&res, &temp1);
    PolyDestroy(&res);
    PolyDestroy(&temp0);
    PolyDestroy(&temp1);
    res = temp2;
  }
  return res;
}

Poly PolyMonosInside(size_t count, Mono monos[]);

/*
void freeingOwn(size_t count, Mono monos[]) {
  for(size_t i = 0; i < count; i++){
    MonoDestroy(&(monos[i]));
  }
  free(monos);
}*/

Poly PolyOwnMonos(size_t count, Mono *monos){ 
  if(count == 0 || monos == NULL) return PolyZero();
  
  Poly sum = PolyMonosInside(count, monos);
  /*
  MonosSort(monos, 0, count - 1);
  Poly sum;
  sum.arr = SafeMalloc(count);
  size_t imonos = 1, isum = 0;
  Mono last = MonoClone(&(monos[0]));
  while(imonos < count){
    if(last.exp == monos[imonos].exp){
      Poly temp = PolyAdd(&(last.p), &(monos[imonos].p));
      PolyDestroy(&(last.p));
      last.p = temp;
    }else{
      if(!PolyIsZero(&(last.p))){
        (sum.arr)[isum] = last;
        isum++;
      }
      last = MonoClone(&(monos[imonos]));
    }
    imonos++;
  }
  sum = AddLast(isum, count, last, &sum);
  sum = UnproperPoly(&sum);
  */
  for(size_t i = 0; i < count; i++)
    MonoDestroy(&(monos[i]));
  free(monos);
  return sum;
}

/*
void freeingClone(size_t count, Mono monos[]) {
  for(size_t i = 0; i < count; i++){
    MonoDestroy(&(monos[i]));
  }
}
*/

Poly PolyCloneMonos(size_t count, const Mono monos[]){
  if(count == 0 || monos == NULL) return PolyZero();
  Mono monos2[count];
  for(size_t i = 0; i < count; i++)
    monos2[i] = MonoClone(&(monos[i]));
  Poly sum = PolyMonosInside(count, monos2);
  /*
  MonosSort(monos2, 0, count - 1);
  Poly sum;
  sum.arr = SafeMalloc(count);
  size_t imonos = 1, isum = 0;
  Mono last = MonoClone(&(monos2[0]));
  while(imonos < count){
    if(last.exp == monos2[imonos].exp){
      Poly temp = PolyAdd(&(last.p), &(monos2[imonos].p));
      PolyDestroy(&(last.p));
      last.p = temp;
    }else{
      if(!PolyIsZero(&(last.p))){
        (sum.arr)[isum] = last;
        isum++;
      }
      last = MonoClone(&(monos2[imonos]));
    }
    imonos++;
  }
  sum = AddLast(isum, count, last, &sum);
  sum = UnproperPoly(&sum);*/
  //freeingClone(count, monos2);
  for(size_t i = 0; i < count; i++)
    MonoDestroy(&(monos2[i]));
  return sum;
}

Poly PolyMonosInside(size_t count, Mono monos[]){
  
  MonosSort(monos, 0, count - 1);
  Poly sum;
  sum.arr = SafeMalloc(count);
  size_t imonos = 1, isum = 0;
  Mono last = MonoClone(&(monos[0]));
  while(imonos < count){
    if(last.exp == monos[imonos].exp){
      Poly temp = PolyAdd(&(last.p), &(monos[imonos].p));
      PolyDestroy(&(last.p));
      last.p = temp;
    }else{
      if(!PolyIsZero(&(last.p))){
        (sum.arr)[isum] = last;
        isum++;
      }
      last = MonoClone(&(monos[imonos]));
    }
    imonos++;
  }
  sum = AddLast(isum, count, last, &sum);
  sum = UnproperPoly(&sum);

  return sum;
}

/*

Poly MonoAtPoly(Mono *m, Poly *q){
  Poly result = PolyClone(q);
  // m->p * q ^ exp
  for(int i = 0; i < m->exp; i++){ //szybkie potegownie zamiast tego
    Poly old_result = result;
    result = PolyMul(&result, q);
    PolyDestroy(&old_result);
  }
  Poly old_result = result;
  result = PolyMul(&result, &(m->p));
  PolyDestroy(&old_result);
  return result;
}

Poly PolyAtPoly(Poly *p, Poly *q){
  if(!p->arr) return PolyClone(p); //sth here
  Poly result = PolyZero();
  for(size_t i = 0; i < p->size; i++){
    Poly old_result = result;
    Poly next = MonoAtPoly(&(p->arr)[i], q);
    result = PolyAdd(&result, &next);
    PolyDestroy(&old_result);
    PolyDestroy(&next);
  }
  return result;
}

Poly PolyCompose(const Poly *p, size_t k, const Poly q[]){
  poly_exp_t deg = PolyDeg(p);
  poly_exp_t i = 0;
  Poly result = PolyClone(p);
  while(i < min(deg, k)){
    Poly old_result = result;
    result = PolyAtPoly(&result, &(q[i]));
    PolyDestroy(&old_result);
    i++;
  }
  Poly zero = PolyZero();
  while(i < deg){
    Poly old_result = result;
    result = PolyAtPoly(&result, &zero);
    PolyDestroy(&old_result);
    i++;
  }
  
}*/



void PrintPoly2(Poly *poly);

void MonoPrint2(Mono *mono) {
  printf("(");
  PrintPoly2(&(mono->p));
  printf(",");
  printf("%d", mono->exp);
  printf(")");
}

/**
 * wypisuje na standardowe wyjście wielomian z wierzchołka stosu
 */
void PrintPoly2(Poly *poly) {
  if(!poly->arr)
    printf("%ld", poly->coeff);
  else {
    //printf("(");
    for(size_t i = 0; i < poly->size - 1; i++) {
      MonoPrint2(&((poly->arr)[i]));
      printf("+");
    }
    MonoPrint2(&((poly->arr)[poly->size - 1]));
    //printf(")");
  }
}



Poly PolyPow(const Poly *p, poly_exp_t exp){ //szybkie potegownie poly
  if(exp == 0) return PolyFromCoeff(1);
  if(exp == 1) return PolyClone(p);
  Poly res = PolyPow(p, exp / 2);
  Poly res2 = PolyMul(&res, &res);
  if(exp % 2 == 0){
    PolyDestroy(&res);
    return res2;
  } else {
  Poly res3 = PolyMul(&res2, p);
  PolyDestroy(&res);
  PolyDestroy(&res2);
  return res3;
  }
}

Poly PolyCompose(const Poly *p, size_t k, const Poly q[]){
  if(!p->arr) return PolyClone(p);
  Poly result = PolyZero();
  for(size_t i = 0; i < p->size; i++){
    //printf("  in PolyCompose i = %ld\n", i);
    Poly q_pow;
    Poly p_composed;
    if(k > 0){
      //printf("  q    = "); PrintPoly2((q)); printf("\n");
      //printf("  exp  = %d\n", p->arr[i].exp);
      q_pow = PolyPow(q, p->arr[i].exp);
      //printf("  q_pow = "); PrintPoly2(&(q_pow)); printf("\n");
      p_composed = PolyCompose(&((p->arr)[i]).p, k - 1, &q[1]);
      //printf("  p_com = "); PrintPoly2(&(p_composed)); printf("\n");
    }else{
      if(p->arr[i].exp == 0) q_pow = PolyFromCoeff(1);
      else q_pow = PolyZero();
      p_composed = PolyCompose(&((p->arr)[i]).p, k, q);
    }
    Poly addend = PolyMul(&q_pow, &p_composed);
    Poly old_result = result;
    result = PolyAdd(&result, &addend);
    PolyDestroy(&old_result);
    PolyDestroy(&addend);
    PolyDestroy(&p_composed);
    PolyDestroy(&q_pow);
  }
  return result;
}
