#include <iostream>
#include <cassert>

#include "../LABA_2/arraySequence.h"
#include "../../LAzy_LABA_4/src/lazy/lazy_sequence.h"
#include "../src/utils/cardinal.h"
#include "../src/generators/generator.h"

using namespace std;

// ==============================================================================
// ГЕНЕРАТОРЫ
// ==============================================================================

// Геометрическая прогрессия
class GeomGenerator : public Generator<int> {
    int value, start, step;
public:
    GeomGenerator(int start, int step) : start(start), step(step), value(start) {}
    int get_next() override { int res = value; value *= step; return res; }
    int get(const Cardinal &index) override {
        int target = start;
        for (int i = 1; i <= index.get_offset(); i++) target *= step;
        return target;
    }
    bool has_next() const override { return true; }
};

class GeomBuilder : public AbstractGeneratorBuilder<int> {
    int start, step;
public:
    GeomBuilder(int start, int step) : start(start), step(step) {}
    Generator<int>* build() const override { return new GeomGenerator(start, step); }
};

// Арифметическая прогрессия
class ArifGenerator : public Generator<int> {
    int current, step;
public:
    ArifGenerator(int start, int step) : current(start), step(step) {}
    int get_next() override { int res = current; current += step; return res; }
    int get(const Cardinal &index) override { return current + step * index.get_offset(); }
    bool has_next() const override { return true; }
};

class ArifBuilder : public AbstractGeneratorBuilder<int> {
    int start, step;
public:
    ArifBuilder(int s, int st) : start(s), step(st) {}
    Generator<int>* build() const override { return new ArifGenerator(start, step); }
};

// Фибоначчи
class FibonaGenerator : public Generator<int> {
private:
    int a = 0; // F(n-2)
    int b = 1; // F(n-1)
    int current_idx = 0;
public:
    int get_next() override {
        if (current_idx == 0) { current_idx++; return 0; }
        if (current_idx == 1) { current_idx++; return 1; }

        int next = a + b;
        a = b;
        b = next;
        current_idx++;
        return next;
    }

    int get(const Cardinal &index) override {
        int target = index.get_offset();
        if (target == 0) return 0;
        if (target == 1) return 1;

        int n0 = 0, n1 = 1, next = 0;
        for (int i = 2; i <= target; i++) {
            next = n0 + n1;
            n0 = n1;
            n1 = next;
        }
        return n1;
    }
    bool has_next() const override { return true; }
};

class FibonaBuilder : public AbstractGeneratorBuilder<int> {
public:
    Generator<int>* build() const override { return new FibonaGenerator(); }
};

// ==============================================================================
// ФУНКЦИИ-ОБРАБОТЧИКИ (ЗАМЕНА ЛЯМБДАМ)
// ==============================================================================

int func_map_double(const int& x) { return x * 2; }
bool func_filter_even(const int& x) { return x % 2 == 0; }

// ==============================================================================
// ТЕСТЫ
// ==============================================================================

void TestTripleConcat() {
    cout << "[TEST] Running Triple Infinite Concat..." << endl;

    // A: 1, 5, 25... (Geom), B: 0, 1, 2... (Arif), C: 42, 42, 42... (Arif)
    LazySequence<int>* A = new LazySequence(new GeomBuilder(1, 5), Cardinal::infinity(), 100);
    LazySequence<int>* B = new LazySequence(new ArifBuilder(0, 1), Cardinal::infinity(), 100);
    LazySequence<int>* C = new LazySequence(new ArifBuilder(42, 0), Cardinal::infinity(), 100);

    Sequence<int>* res = A->concat(B)->concat(C);

    // Доступ через Кардиналы:
    assert(res->get(Cardinal(0, 2)) == 25);  // A[2]
    assert(res->get(Cardinal(1, 5)) == 5);   // B[5]
    assert(res->get(Cardinal(2, 0)) == 42);  // C[0]

    cout << "  -> Triple Concat Passed!" << endl;
    delete res; delete A; delete B; delete C;
}

void TestMapAndFilter() {
    cout << "[TEST] Running Map & Filter..." << endl;

    LazySequence<int>* seq = new LazySequence(new ArifBuilder(1, 1), Cardinal::infinity(), 100);

    // Test Map
    Sequence<int>* mapped = seq->map(func_map_double);
    assert(mapped->get(Cardinal(0, 0)) == 2);
    assert(mapped->get(Cardinal(0, 1)) == 4);

    // Test Filter (Where)
    Sequence<int>* filtered = seq->where(func_filter_even);
    assert(filtered->get(Cardinal(0, 0)) == 2);
    assert(filtered->get(Cardinal(0, 1)) == 4);

    cout << "  -> Map & Filter Passed!" << endl;
    delete mapped; delete filtered; delete seq;
}

void TestFibonacci() {
    cout << "[TEST] Running Fibonacci..." << endl;
    LazySequence<int>* fib = new LazySequence(new FibonaBuilder(), Cardinal::infinity(), 100);

    assert(fib->get(Cardinal(0, 0)) == 0);
    assert(fib->get(Cardinal(0, 5)) == 5);
    assert(fib->get(Cardinal(0, 10)) == 55);

    cout << "  -> Fibonacci Passed!" << endl;
    delete fib;
}

int main() {
    TestTripleConcat();
    TestMapAndFilter();
    TestFibonacci();

    cout << "\nALL MANUAL TESTS PASSED!" << endl;
    return 0;
}