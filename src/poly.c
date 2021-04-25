#include <assert.h>
#include <stdbool.h>
#include <stddef.h>

#include "poly.h"

#define STARTING_ARRAY 100

void PolyDestroy(Poly *p){
  if(p->arr){
    for(int i = 0; i < p->size; i++){
      MonoDestroy(&((p->arr)[i]));
    }
  }
}

Poly PolyClone(const Poly *p){
  Poly clone;
  if(p->arr){
    clone.size = p->size;
    clone.arr = malloc(p->size * sizeof(struct Mono));
    for(int i = 0; i < p->size; i++){
      (clone.arr)[i] = MonoClone(&((p->arr)[i]));
    }
  }else
    clone.coeff = p->coeff;
  return clone;
}

static inline Poly AddUnproperPolys(const Poly *p, const Poly *q){
  //assert(!p->arr || !q->arr);
  if(!p->arr && !q->arr)
    return PolyFromCoeff(p->coeff + q->coeff);
  if(!p->arr){
    Poly temp = (Poly) {.size = 1, .arr = malloc(sizeof(Mono))};
    (temp.arr)[0] = MonoFromPoly(p->coeff, 0);
    Poly sum = PolyAdd(q, &temp);
    PolyDestroy(&temp);
    return sum;
  }    
  //if(!q->arr)
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
  sum.size = isum;
  return sum;
}


/**
 * Sumuje listę jednomianów i tworzy z nich wielomian.
 * Przejmuje na własność zawartość tablicy @p monos.
 * @param[in] count : liczba jednomianów
 * @param[in] monos : tablica jednomianów
 * @return wielomian będący sumą jednomianów
 */
Poly PolyAddMonos(size_t count, const Mono monos[]);

/**
 * Mnoży dwa wielomiany.
 * @param[in] p : wielomian @f$p@f$
 * @param[in] q : wielomian @f$q@f$
 * @return @f$p * q@f$
 */
Poly PolyMul(const Poly *p, const Poly *q);

/**
 * Zwraca przeciwny wielomian.
 * @param[in] p : wielomian @f$p@f$
 * @return @f$-p@f$
 */
Poly PolyNeg(const Poly *p);

/**
 * Odejmuje wielomian od wielomianu.
 * @param[in] p : wielomian @f$p@f$
 * @param[in] q : wielomian @f$q@f$
 * @return @f$p - q@f$
 */
Poly PolySub(const Poly *p, const Poly *q);

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
 * Sprawdza równość dwóch wielomianów.
 * @param[in] p : wielomian @f$p@f$
 * @param[in] q : wielomian @f$q@f$
 * @return @f$p = q@f$
 */
bool PolyIsEq(const Poly *p, const Poly *q);

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