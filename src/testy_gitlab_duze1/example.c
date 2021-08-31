#ifdef NDEBUG
#undef NDEBUG
#endif

#include "helper_functions_from_example.h"
#include "example.h"

static bool SimpleAddTest(void) {
	bool res = true;
	// Różne przypadki wielomian/współczynnik
	res &= TestAdd(C(1),
			C(2),
			C(3));
	res &= TestAdd(P(C(1), 1),
			C(2),
			P(C(2), 0, C(1), 1));
	res &= TestAdd(C(1),
			P(C(2), 2),
			P(C(1), 0, C(2), 2));
	res &= TestAdd(P(C(1), 1),
			P(C(2), 2),
			P(C(1), 1, C(2), 2));
	res &= TestAdd(C(0),
			P(C(1), 1),
			P(C(1), 1));
	// Upraszczanie wielomianu
	res &= TestAdd(P(C(1), 1),
			P(C(-1), 1),
			C(0));
	res &= TestAdd(P(C(1), 1, C(2), 2),
			P(C(-1), 1),
			P(C(2), 2));
	res &= TestAdd(P(C(2), 0, C(1), 1),
			P(C(-1), 1),
			C(2));
	// Dodawanie współczynnika i upraszczanie
	res &= TestAdd(C(1),
			P(C(-1), 0, C(1), 1),
			P(C(1), 1));
	res &= TestAdd(C(1),
			P(P(C(-1), 0, C(1), 1), 0),
			P(P(C(1), 1), 0));
	res &= TestAdd(C(1),
			P(C(1), 0, C(2), 2),
			P(C(2), 0, C(2), 2));
	res &= TestAdd(C(1),
			P(P(C(1), 0, C(1), 1), 0, C(2), 2),
			P(P(C(2), 0, C(1), 1), 0, C(2), 2));
	res &= TestAdd(C(1),
			P(P(C(-1), 0, C(1), 1), 0, C(2), 2),
			P(P(C(1), 1), 0, C(2), 2));
	// Dodawanie wielomianów wielu zmiennych
	res &= TestAdd(P(P(C(1), 2), 0, P(C(2), 1), 1, C(1), 2),
			P(P(C(1), 2), 0, P(C(-2), 1), 1, C(1), 2),
			P(P(C(2), 2), 0, C(2), 2));
	res &= TestAdd(P(P(C(1), 2), 0, P(C(2), 1), 1, C(1), 2),
			P(P(C(-1), 2), 0, P(C(1), 0, C(2), 1, C(1), 2), 1, C(-1), 2),
			P(P(C(1), 0, C(4), 1, C(1), 2), 1));
	// Redukcja do współczynnika
	res &= TestAdd(P(P(C(1), 0, C(1), 1), 0, C(1), 1),
			P(P(C(1), 0, C(-1), 1), 0, C(-1), 1),
			C(2));
	res &= TestAdd(P(P(P(C(1), 0, C(1), 1), 0, C(1), 1), 1),
			P(P(P(C(1), 0, C(-1), 1), 0, C(-1), 1), 1),
			P(C(2), 1));
	// Dodawanie wielomianu do siebie
	Poly a = P(C(1), 1);
	Poly b = PolyAdd(&a, &a);
	Poly c = P(C(2), 1);
	res &= PolyIsEq(&b, &c);
	PolyDestroy(&a);
	PolyDestroy(&b);
	PolyDestroy(&c);
	return res;
}

static bool SimpleAddMonosTest(void) {
	bool res = true;
	{
		Mono m[] = {M(C(-1), 0), M(C(1), 0)};
		res &= TestAddMonos(2, m, C(0));
	}
	{
		Mono m[] = {M(C(-1), 1), M(C(1), 1)};
		res &= TestAddMonos(2, m, C(0));
	}
	{
		Mono m[] = {M(C(1), 0), M(C(1), 0)};
		res &= TestAddMonos(2, m, C(2));
	}
	{
		Mono m[] = {M(C(1), 1), M(C(1), 1)};
		res &= TestAddMonos(2, m, P(C(2), 1));
	}
	{
		Mono m[] = {M(P(C(-1), 1), 0), M(P(C(1), 1), 0)};
		res &= TestAddMonos(2, m, C(0));
	}
	{
		Mono m[] = {M(P(C(-1), 0), 1),
				M(P(C(1), 0), 1),
				M(C(2), 0),
				M(C(1), 1),
				M(P(C(2), 1), 2),
				M(P(C(2), 2), 2)};
		res &= TestAddMonos(6, m, P(C(2), 0, C(1), 1, P(C(2), 1, C(2), 2), 2));
	}
	return res;
}

static bool SimpleMulTest(void) {
	bool res = true;
	res &= TestMul(C(2),
			C(3),
			C(6));
	res &= TestMul(P(C(1), 1),
			C(2),
			P(C(2), 1));
	res &= TestMul(C(3),
			P(C(2), 2),
			P(C(6), 2));
	res &= TestMul(P(C(1), 1),
			P(C(2), 2),
			P(C(2), 3));
	res &= TestMul(P(C(-1), 0, C(1), 1),
			P(C(1), 0, C(1), 1),
			P(C(-1), 0, C(1), 2));
	res &= TestMul(P(P(C(1), 2), 0, P(C(1), 1), 1, C(1), 2),
			P(P(C(1), 2), 0, P(C(-1), 1), 1, C(1), 2),
			P(P(C(1), 4), 0, P(C(1), 2), 2, C(1), 4));
	return res;
}

static bool SimpleNegTest(void) {
	Poly a = P(P(C(1), 0, C(2), 2), 0, P(C(1), 1), 1, C(1), 2);
	Poly b = PolyNeg(&a);
	Poly c = P(P(C(-1), 0, C(-2), 2), 0, P(C(-1), 1), 1, C(-1), 2);
	bool is_eq = PolyIsEq(&b, &c);
	PolyDestroy(&a);
	PolyDestroy(&b);
	PolyDestroy(&c);
	return is_eq;
}

static bool SimpleSubTest(void) {
	return TestSub(P(P(C(1), 2), 0, P(C(2), 1), 1, C(1), 2),
			P(P(C(1), 2), 0, P(C(-1), 0, C(-2), 1, C(-1), 2), 1, C(1), 2),
			P(P(C(1), 0, C(4), 1, C(1), 2), 1));
}

#define POLY_P P(P(C(1), 3), 0, P(C(1), 2), 2, C(1), 3)

static bool SimpleDegByTest(void) {
	bool res = true;
	res &= TestDegBy(C(0), 1, -1);
	res &= TestDegBy(C(1), 0, 0);
	res &= TestDegBy(P(C(1), 1), 1, 0);
	res &= TestDegBy(POLY_P, 0, 3);
	res &= TestDegBy(POLY_P, 1, 3);
	return res;
}

static bool SimpleDegTest(void) {
	bool res = true;
	res &= TestDeg(C(0), -1);
	res &= TestDeg(C(1), 0);
	res &= TestDeg(P(C(1), 1), 1);
	res &= TestDeg(POLY_P, 4);
	return res;
}

static bool SimpleIsEqTest(void) {
	bool res = true;
	res &= TestEq(C(0), C(0), true);
	res &= TestEq(C(0), C(1), false);
	res &= TestEq(C(1), C(1), true);
	res &= TestEq(P(C(1), 1), C(1), false);
	res &= TestEq(P(C(1), 1), P(C(1), 1), true);
	res &= TestEq(P(C(1), 1), P(C(1), 2), false);
	res &= TestEq(P(C(1), 1), P(C(2), 1), false);
	res &= TestEq(POLY_P, POLY_P, true);
	Poly a = C(1);
	Poly b = C(2);
	Poly p = POLY_P;
	res &= TestEq(PolyAdd(&p, &a), PolyAdd(&p, &b), false);
	PolyDestroy(&a);
	PolyDestroy(&b);
	PolyDestroy(&p);
	return res;
}

static bool SimpleAtTest(void) {
	bool res = true;
	res &= TestAt(C(2), 1, C(2));
	res &= TestAt(P(C(1), 0, C(1), 18), 10, C(1000000000000000001L));
	res &= TestAt(P(C(3), 1, C(2), 3, C(1), 5), 10, C(102030));
	res &= TestAt(P(P(C(1), 4), 0, P(C(1), 2), 2, C(1), 3), 2,
			P(C(8), 0, C(4), 2, C(1), 4));
	return res;
}

static bool OverflowTest(void) {
	bool res = true;
	res &= TestMul(P(C(1L << 32), 1), C(1L << 32), C(0));
	res &= TestAt(P(C(1), 64), 2, C(0));
	res &= TestAt(P(C(1), 0, C(1), 64), 2, C(1));
	res &= TestAt(P(P(C(1), 1), 64), 2, C(0));
	return res;
}

void Example() {
	assert(SimpleAddTest());
	assert(SimpleAddMonosTest());
	assert(SimpleMulTest());
	assert(SimpleNegTest());
	assert(SimpleSubTest());
	assert(SimpleDegByTest());
	assert(SimpleDegTest());
	assert(SimpleIsEqTest());
	assert(SimpleAtTest());
	assert(OverflowTest());
}
