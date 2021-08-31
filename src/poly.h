/** @file
  Interfejs klasy wielomianów rzadkich wielu zmiennych

  @authors Jakub Pawlewicz <pan@mimuw.edu.pl>, Marcin Peczarski <marpe@mimuw.edu.pl>
  @copyright Uniwersytet Warszawski
  @date 2021
*/

#ifndef __POLY_H__
#define __POLY_H__

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>

/** To jest typ reprezentujący współczynniki. */
typedef long poly_coeff_t;

/** To jest typ reprezentujący wykładniki. */
typedef int poly_exp_t;

struct Mono;

/**
 * To jest struktura przechowująca wielomian.
 * Wielomian jest albo liczbą całkowitą, czyli wielomianem stałym
 * (wtedy `arr == NULL`), albo niepustą listą jednomianów (wtedy `arr != NULL`).
 */
typedef struct Poly {
  /**
  * To jest unia przechowująca współczynnik wielomianu lub
  * liczbę jednomianów w wielomianie.
  * Jeżeli `arr == NULL`, wtedy jest to współczynnik będący liczbą całkowitą.
  * W przeciwnym przypadku jest to niepusta lista jednomianów.
  */
  union {
    poly_coeff_t coeff; ///< współczynnik
    size_t       size; ///< rozmiar wielomianu, liczba jednomianów
  };
  /** To jest tablica przechowująca listę jednomianów. */
  struct Mono *arr;
} Poly;

/**
 * To jest struktura przechowująca jednomian.
 * Jednomian ma postać @f$px_i^n@f$.
 * Współczynnik @f$p@f$ może też być
 * wielomianem nad kolejną zmienną @f$x_{i+1}@f$.
 */
typedef struct Mono {
  Poly p; ///< współczynnik
  poly_exp_t exp; ///< wykładnik
} Mono;

//static inline Mono* SafeMalloc(size_t size);
//static inline Poly* SafeMallocPoly();

/**
 * Daje wartość wykładnika jendomianu.
 * @param[in] m : jednomian
 * @return wartość wykładnika jednomianu
 */
static inline poly_exp_t MonoGetExp(const Mono *m) {
  return m->exp;
}

/**
 * Tworzy wielomian, który jest współczynnikiem (wielomian stały).
 * @param[in] c : wartość współczynnika
 * @return wielomian
 */
static inline Poly PolyFromCoeff(poly_coeff_t c) {
  return (Poly) {.coeff = c, .arr = NULL};
}

/**
 * Tworzy wielomian tożsamościowo równy zeru.
 * @return wielomian
 */
static inline Poly PolyZero(void) {
  return PolyFromCoeff(0);
}

static inline bool PolyIsZero(const Poly *p);

/**
 * Tworzy jednomian @f$px_i^n@f$.
 * Przejmuje na własność zawartość struktury wskazywanej przez @p p.
 * @param[in] p : wielomian - współczynnik jednomianu
 * @param[in] n : wykładnik
 * @return jednomian @f$px_i^n@f$
 */
static inline Mono MonoFromPoly(const Poly *p, poly_exp_t n) {
  assert(n == 0 || !PolyIsZero(p));
  return (Mono) {.p = *p, .exp = n};
}

/**
 * Sprawdza, czy wielomian jest współczynnikiem (czy jest to wielomian stały).
 * @param[in] p : wielomian
 * @return Czy wielomian jest współczynnikiem?
 */
static inline bool PolyIsCoeff(const Poly *p) {
  return p->arr == NULL;
}

/**
 * Sprawdza, czy wielomian jest tożsamościowo równy zeru.
 * @param[in] p : wielomian
 * @return Czy wielomian jest równy zeru?
 */
static inline bool PolyIsZero(const Poly *p) {
  return PolyIsCoeff(p) && p->coeff == 0;
}

/**
 * Usuwa wielomian z pamięci.
 * @param[in] p : wielomian
 */
void PolyDestroy(Poly *p);

/**
 * Usuwa jednomian z pamięci.
 * @param[in] m : jednomian
 */
static inline void MonoDestroy(Mono *m) {
  PolyDestroy(&m->p);
}

/**
 * Robi pełną, głęboką kopię wielomianu.
 * @param[in] p : wielomian
 * @return skopiowany wielomian
 */
Poly PolyClone(const Poly *p);

/**
 * Robi pełną, głęboką kopię jednomianu.
 * @param[in] m : jednomian
 * @return skopiowany jednomian
 */
static inline Mono MonoClone(const Mono *m) {
  return (Mono) {.p = PolyClone(&m->p), .exp = m->exp};
}

/**
 * Zamienia wielomian postaci p(x_0)^0 na wielomian postaci p, jeśli będzie równoważny 
 * @param[in] p : wielomian 
 * @return skrócony wielomian
 */
Poly UnproperPoly(Poly *p);

/**
 * Zmienia wielomian postaci p na wielomian postaci p(x_0)^0
 * @param[in] p : wielomian @f$q@f$
 * @return @f$p * x^0@f$
 */
Poly ProperPoly(const Poly *p);

/**
 * Dodaje dwa wielomiany, z których conajmniej jeden jest wielomianem prostym.
 * @param[in] p : wielomian @f$p@f$
 * @param[in] q : wielomian @f$q@f$
 * @return @f$p + q@f$
 */
Poly AddUnproperPolys(const Poly *p, const Poly *q);

/**
 * Dodaje dwa wielomiany.
 * @param[in] p : wielomian @f$p@f$
 * @param[in] q : wielomian @f$q@f$
 * @return @f$p + q@f$
 */
Poly PolyAdd(const Poly *p, const Poly *q);

/**
 * Sortuje tablicę jednomianów.
 * @param[in] monos : tablica jednomianów
 * @param[in] min : indeks, od którego sortujemy 
 * @param[in] max : indeks, do którego sortujemy 
 */
void MonosSort(Mono monos[], size_t min, size_t max);

/**
 * Dodaje do wielomianu @p sum ostatni jednomian @p last, jeśli nie jest zerowy
 * @param[in] isum : ilość wypełnionych komórek
 * @param[in] count : liczba jednomianów startowych
 * @param[in] last : ostatni jednomian
 * @param[in] sum :  wielomian
 * @return pełny wielomian
 */
Poly AddLast(size_t isum, size_t count, Mono last, Poly *sum);


/**
 * Sumuje listę jednomianów i tworzy z nich wielomian.
 * Przejmuje na własność zawartość tablicy @p monos.
 * @param[in] count : liczba jednomianów
 * @param[in] monos : tablica jednomianów
 * @return wielomian będący sumą jednomianów
 */
Poly PolyAddMonos(size_t count, const Mono monos[]);

/**
 * Mnoży dwa wielomiany, z których conajmniej jeden jest wielomianem prostym.
 * @param[in] p : wielomian @f$p@f$
 * @param[in] q : wielomian @f$q@f$
 * @return @f$p * q@f$
 */
Poly MulUnproperPolys(const Poly *p, const Poly *q);

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
 * Sprawdza równość dwóch jednomianów.
 * @param[in] m : jednomian @f$m@f$
 * @param[in] n : jednomian @f$n@f$
 * @return @f$m = n@f$
 */
bool MonoIsEq(const Mono *m, const Mono *n);

/**
 * Sprawdza równość dwóch wielomianów.
 * @param[in] p : wielomian @f$p@f$
 * @param[in] q : wielomian @f$q@f$
 * @return @f$p = q@f$
 */
bool PolyIsEq(const Poly *p, const Poly *q);

/**
 * Podnosi @p x do potęgi @p exp.
 * @param[in] x : baza @f$p@f$
 * @param[in] exp : wykładnik @f$exp@f$
 * @return @f$x^exp@f$
 */
poly_coeff_t pow2(poly_coeff_t x, poly_exp_t exp);

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

/**
 * Sumuje listę jednomianów i tworzy z nich wielomian. Przejmuje na własność 
 * pamięć wskazywaną przez @p monos i jej zawartość. Może dowolnie modyfikować 
 * zawartość tej pamięci. Zakładamy, że pamięć wskazywana przez @p monos 
 * została zaalokowana na stercie. Jeśli @p count lub @p monos jest równe zeru 
 * (NULL), tworzy wielomian tożsamościowo równy zeru.
 * @param[in] count : liczba jednomianów
 * @param[in] monos : tablica jednomianów
 * @return wielomian będący sumą jednomianów
 */
Poly PolyOwnMonos(size_t count, Mono *monos);

/**
 * Sumuje listę jednomianów i tworzy z nich wielomian. Nie modyfikuje zawartości
 * tablicy @p monos. Jeśli jest to wymagane, to wykonuje pełne kopie jednomianów
 * z tablicy @p monos. Jeśli @p count lub @p monos jest równe zeru (NULL), 
 * tworzy wielomian tożsamościowo równy zeru.
 * @param[in] count : liczba jednomianów
 * @param[in] monos : tablica jednomianów
 * @return wielomian będący sumą jednomianów
 */
Poly PolyCloneMonos(size_t count, const Mono monos[]);

/**
 * Podnosi wielomian @p p do potęgi @p exp.
 * @param[in] p : wielomian będący bazą @f$p@f$
 * @param[in] exp : wykładnik @f$exp@f$
 * @return @f$p^exp@f$
 */
Poly PolyPow(const Poly *p, poly_exp_t exp); //szybkie potegownie poly

/**
 * Składa wielomiany. Dany jest wielomian @p p oraz @p k wielomianów q0, q1, q2, …, qk−1. 
 * Niech l oznacza liczbę zmiennych wielomianu p i niech te zmienne są oznaczone 
 * odpowiednio x0, x1, x2, …, xl−1. Wynikiem złożenia jest wielomian p(q0, q1, q2, …), 
 * czyli wielomian powstający przez podstawienie w wielomianie p pod zmienną xi 
 * wielomianu qi dla i=0,1,2,…,min(k,l)−1. Jeśli k<l, to pod zmienne xk, …, xl−1 
 * podstawiamy zera. Na przykład, jeśli k=0, to wynikiem złożenia jest liczba p(0, 0, 0, …).
 * @param[in] p : wielomian @f$p@f$
 * @param[in] k : liczba wielomianów
 * @param[in] q : tablica wielomianów
 * @return wielomian będący złożeniem wielomianów
 */
Poly PolyCompose(const Poly *p, size_t k, const Poly q[]);

#endif /* __POLY_H__ */
