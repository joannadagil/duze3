#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#include "poly.h"

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
    clone.arr = malloc(p->size * sizeof(struct Mono));
    for(size_t i = 0; i < p->size; i++){
      (clone.arr)[i] = MonoClone(&((p->arr)[i]));
    }
  }else{
    clone.coeff = p->coeff;
    clone.arr = NULL;
  }
  return clone;
}

/**
 * Zmienia wielomian postaci p na wielomian postaci p(x_1)^0
 */
static inline Poly ProperPoly(const Poly *p){
  Poly proper = (Poly) {.size = 1, .arr = malloc(sizeof(Mono))};
  (proper.arr)[0] = MonoFromPoly(p, 0);
  return proper;
}

/**
 * Dodaje dwa wielomiany, z których conajmniej jeden jest wielomianem prostym.
 * @param[in] p : wielomian @f$p@f$
 * @param[in] q : wielomian @f$q@f$
 * @return @f$p + q@f$
 */
static inline Poly AddUnproperPolys(const Poly *p, const Poly *q){
  //assert(!p->arr || !q->arr);
  if(!p->arr && !q->arr)
    return PolyFromCoeff(p->coeff + q->coeff);
  if(!p->arr){
    //Poly temp = (Poly) {.size = 1, .arr = malloc(sizeof(Mono))};
    //(temp.arr)[0] = MonoFromPoly(p, 0);
    Poly temp = ProperPoly(p);
    Poly sum = PolyAdd(&temp, q);
    PolyDestroy(&temp);
    return sum;
  }//if(!q->arr)
  return AddUnproperPolys(q, p);
}

Poly PolyAdd(const Poly *p, const Poly *q){
  if(!p->arr || !q->arr) return AddUnproperPolys(p, q);
  Poly sum;
  sum.arr = malloc((p->size + q->size) * sizeof(struct Mono));
  size_t ip = 0, iq = 0, isum = 0;
  while(ip < p->size || iq < q->size){
    if(ip < p->size && iq < q->size && (p->arr)[ip].exp == (q->arr)[iq].exp){
      (sum.arr)[isum] = (Mono) {.p = PolyAdd(&((p->arr)[ip].p), &((q->arr)[iq].p)), .exp = (p->arr)[ip].exp};
      Poly zero = PolyZero();
      if (!PolyIsEq(&((sum.arr)[isum].p), &zero)) isum++; //jeśli się wyzerowały to nie zapamiętujemy tej pamięci
      ip++; iq++;
    }else if(iq == q->size || (ip < p->size && iq < q->size && (p->arr)[ip].exp < (q->arr)[iq].exp)){
      (sum.arr)[isum] = MonoClone(&((p->arr)[ip]));
      ip++; isum++;
    }else{ //ip == p->size || (ip < p->size && iq < q->size && (p->arr)[ip].exp > (q->arr)[iq].exp)
      (sum.arr)[isum] = MonoClone(&((q->arr)[iq]));
      iq++; isum++;
    }
  }
  sum.arr = realloc(sum.arr, isum * sizeof(struct Mono));
  sum.size = isum; //o dziwo działa też dla zerujących się wielomianów
  return sum;
}

static inline void MonosSwap(Mono *a, Mono *b){
  Mono temp = *a;
  *a = *b;
  *b = temp;
}

static inline void MonosSort(Mono monos[], size_t min, size_t max){
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

Poly PolyAddMonos(size_t count, const Mono monos[]){
  
  if(count == 0) return PolyZero();

  //posortowanie tablicy
  Mono monos2[count];
  for(size_t i = 0; i < count; i++){
    monos2[i] = MonoClone(&(monos[i]));
  }

  MonosSort(monos2, 0, count - 1); //tutaj to coś inaczej musi być bo monos jest const

  for(size_t i = 0; i < count; i++){
      MonoPrint(&(monos2[i]));printf("\n");
  }

  //skrocenie tablicy
  Poly sum;
  sum.arr = malloc(count * sizeof(Mono));
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
  //dopisać resztę z last
  if(isum > 0){
    if(!PolyIsZero(&(last.p))){
      (sum.arr)[isum] = last;
      isum++;
    }
    sum.arr = realloc(sum.arr, isum * sizeof(struct Mono));
    sum.size = isum;
  }else{
    free(sum.arr);
    sum.arr = NULL;
    sum.coeff = 0;
  }
  //uwolnić tablicę monos2 ?i monos z tablicy monos?
  for(size_t i = 0; i < count; i++){
    MonoDestroy(&(monos2[i]));
    //MonoDestroy(&(monos[i]));
  }
  return sum;
}

Poly MulUnproperPolys(const Poly *p, const Poly *q){
  if(!p->arr && !q->arr) return PolyFromCoeff(p->coeff * q->coeff);
  if(!p->arr){
    Poly temp = ProperPoly(p);
    Poly mul = PolyMul(&temp, q);
    PolyDestroy(&temp);
    return mul;
  }//if(!q->arr)
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
  return PolyAddMonos(p->size * q->size, monos);
}

Poly PolyNeg(const Poly *p){
  Poly *minus = malloc(sizeof(Poly));
  minus->arr = NULL;
  minus->coeff = -1;
  Poly neg = PolyMul(minus, p);
  free(minus);
  return neg;
}


Poly PolySub(const Poly *p, const Poly *q){
  Poly neg = PolyNeg(q);
  return PolyAdd(p, &neg); //tutaj może poprawić bo nie wiem jak bardzo te wskazniki dzialają
}

/**
 * Zwraca stopień wielomianu ze względu na zadaną zmienną (-1 dla wielomianu
 * tożsamościowo równego zeru). Zmienne indeksowane są od 0.
 * Zmienna o indeksie 0 oznacza zmienną główną tego wielomianu.
 * Większe indeksy oznaczają zmienne wielomianów znajdujących się
 * we współczynnikach.
 * @param[in] p : wielomian
 * @param[in] var_idx : indeks zmiennej
 * @return stopień wielomianu @p p z względu na zmienną o indeksie @p var_idx
 */
poly_exp_t PolyDegBy(const Poly *p, size_t var_idx);

/**
 * Zwraca stopień wielomianu (-1 dla wielomianu tożsamościowo równego zeru).
 * @param[in] p : wielomian
 * @return stopień wielomianu @p p
 */
poly_exp_t PolyDeg(const Poly *p);

/**
 * Sprawdza równość dwóch jednomianów.
 * @param[in] m : jednomian @f$m@f$
 * @param[in] n : jednomian @f$n@f$
 * @return @f$m = n@f$
 */
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

/**
 * Wylicza wartość wielomianu w punkcie @p x.
 * Wstawia pod pierwszą zmienną wielomianu wartość @p x.
 * W wyniku może powstać wielomian, jeśli współczynniki są wielomianami.
 * Wtedy zmniejszane są o jeden indeksy zmiennych w takim wielomianie.
 * Formalnie dla wielomianu @f$p(x_0, x_1, x_2, \ldots)@f$ wynikiem jest
 * wielomian @f$p(x, x_0, x_1, \ldots)@f$.
 * @param[in] p : wielomian @f$p@f$
 * @param[in] x : wartość argumentu @f$x@f$
 * @return @f$p(x, x_0, x_1, \ldots)@f$
 */
Poly PolyAt(const Poly *p, poly_coeff_t x);