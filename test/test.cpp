#include <iostream>
#include <cassert>
#include <string>

#include "../LABA_2/arraySequence.h"
#include "../../LAzy_LABA_4/src/lazy/lazy_sequence.h"
#include "../src/streams/sequence_streams.h"
#include "../src/fsm/state_machine.h"
#include "../src/generators/rule_generator.h"
#include "../LABA_2/mutableArraySequence.h"

using namespace std;

class ArithmeticGenerator : public Generator<int> {
private:
    int current_val;
    int step_val;
    int initial_val;
public:
    ArithmeticGenerator(int start, int step) : current_val(start), step_val(step), initial_val(start) {}

    int get_next() override {
        int res = current_val;
        current_val += step_val;
        return res;
    }

    bool has_next() const override { return true; }

    int get(const Cardinal& index) override {
        return initial_val + step_val * index.get_offset();
    }
};

// Билдер для арифметической прогрессии
class ArithmeticBuilder : public AbstractGeneratorBuilder<int> {
private:
    int start;
    int step;
public:
    ArithmeticBuilder(int start_val, int step_val) : start(start_val), step(step_val) {}
    Generator<int>* build() const override {
        return new ArithmeticGenerator(start, step);
    }
};

// Генератор Фибоначчи
class FibonacciGenerator : public Generator<int> {
private:
    int prev1, prev2;
    int current_index;
public:
    FibonacciGenerator() : prev1(0), prev2(1), current_index(0) {}

    int get_next() override {
        if (current_index == 0) { current_index++; return 0; }
        if (current_index == 1) { current_index++; return 1; }

        int next = prev1 + prev2;
        prev1 = prev2;
        prev2 = next;
        current_index++;
        return next;
    }

    bool has_next() const override { return true; }

    int get(const Cardinal& index) override {
        int a = 0, b = 1;
        int target = index.get_offset();
        if (target == 0) return a;
        if (target == 1) return b;
        for (int i = 2; i <= target; i++) {
            int next = a + b;
            a = b;
            b = next;
        }
        return b;
    }
};

// Билдер для Фибоначчи
class FibonacciBuilder : public AbstractGeneratorBuilder<int> {
public:
    Generator<int>* build() const override {
        return new FibonacciGenerator();
    }
};


// ==============================================================================
// 2. ТЕСТЫ
// ==============================================================================

void TestConcatenation() {
    cout << "[TEST] Running Concat Tests..." << endl;

    // Создаем билдеры для трех наших прогрессий
    ArithmeticBuilder* b1 = new ArithmeticBuilder(0, 1);   // 0, 1, 2, 3...
    ArithmeticBuilder* b2 = new ArithmeticBuilder(0, -1);  // 0, -1, -2, -3...
    ArithmeticBuilder* b3 = new ArithmeticBuilder(42, 0);  // 42, 42, 42...

    // Передаем билдеры в ленивую последовательность
    LazySequence<int> seq1(b1, Cardinal::infinity());
    LazySequence<int> seq2(b2, Cardinal::infinity());
    LazySequence<int> seq3(b3, Cardinal::infinity());

    // Склеиваем
    Sequence<int>* result_seq = seq1.concat(&seq2)->concat(&seq3);

    // Проверяем математику трансфинитных индексов
    assert(result_seq->get(Cardinal(0, 1)) == 1);
    assert(result_seq->get(Cardinal(1, 1)) == -1);
    assert(result_seq->get(Cardinal(2, 0)) == 42);

    cout << "  -> State Concat Tests Passed!" << endl;
}

void TestLazySequence() {
    cout << "[TEST] Running LazySequence Tests..." << endl;

    // Создаем билдер Фибоначчи и отдаем его последовательности
    FibonacciBuilder* builder = new FibonacciBuilder();
    LazySequence<int> fib(builder, Cardinal::infinity());

    assert(fib.get_materialized_count() == 0);
    assert(fib.get(10) == 55);
    assert(fib.get_materialized_count() == 11);
    assert(fib.get(4) == 3);
    assert(fib.get_materialized_count() == 11);

    cout << "  -> LazySequence Tests Passed!" << endl;
}

void TestStream() {
    cout << "[TEST] Running Stream Tests..." << endl;
    int data[] = {10, 20, 30, 40};
    Sequence<int>* seq = new MutableArraySequence<int>(data, 4);

    SequenceInputStream<int> stream(seq);

    bool exception_thrown = false;
    try {
        stream.read();
    } catch (const logic_error&) {
        exception_thrown = true;
    }
    assert(exception_thrown);

    stream.open();
    assert(stream.get_position() == 0);
    assert(stream.read() == 10);
    assert(stream.read() == 20);
    assert(stream.get_position() == 2);
    assert(stream.can_seek() == true);

    stream.seek(0);
    assert(stream.read() == 10);

    stream.close();
    delete seq;
    cout << "  -> Stream Tests Passed!" << endl;
}

void TestStateMachine() {
    cout << "[TEST] Running State Machine Tests..." << endl;
    fsm::StateMachine<char> machine;
    machine.add_state("START", false);
    machine.add_state("BIN_READING", true);
    machine.set_initial_state("START");

    machine.add_transition("START", "BIN_READING", [](const char& c) { return c == '0' || c == '1'; });
    machine.add_transition("BIN_READING", "BIN_READING", [](const char& c) { return c == '0' || c == '1'; });

    char valid_data[] = {'1', '0', '1'};
    Sequence<char>* valid_seq = new MutableArraySequence<char>(valid_data, 3);

    SequenceInputStream<char> valid_stream(valid_seq);
    valid_stream.open();

    assert(machine.process(&valid_stream) == true);
    valid_stream.close();
    delete valid_seq;

    char invalid_data[] = {'1', '0', '2', '1'};
    Sequence<char>* invalid_seq = new MutableArraySequence<char>(invalid_data, 4);

    SequenceInputStream<char> invalid_stream(invalid_seq);
    invalid_stream.open();

    assert(machine.process(&invalid_stream) == false);
    invalid_stream.close();
    delete invalid_seq;

    cout << "  -> State Machine Tests Passed!" << endl;
}

int main() {
    cout << "========================================" << endl;
    cout << " Starting Automated Tests..." << endl;
    cout << "========================================" << endl;

    TestConcatenation();
    TestLazySequence();
    TestStream();
    TestStateMachine();

    cout << "========================================" << endl;
    cout << " ALL TESTS PASSED SUCCESSFULLY! " << endl;
    cout << "========================================" << endl;
    return 0;
}