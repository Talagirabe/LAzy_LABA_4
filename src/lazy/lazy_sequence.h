#ifndef LABA4_LAZY_SEQUENCE_H
#define LABA4_LAZY_SEQUENCE_H

#include <stdexcept>
#include <algorithm>
#include "../../LABA_2/sequence.h"
#include "../../LABA_2/mutableArraySequence.h"
#include "../generators/generator.h"
#include "../generators/concat_generator.h"
#include "../generators/map_generator.h"
#include "../generators/filter_generator.h"
#include "../generators/take_generator.h"
#include "../generators/zip_generator.h"
#include "../generators/append_generator.h"
#include "../generators/insert_at_generator.h"
#include "../generators/prepend_generator.h"
#include "../generators/sub_generator.h"
#include "../utils/pair.h"
#include "../utils/windowed_cache.h"

// Предварительное объявление основного класса
template<class T> class LazySequence;

template<class T>
class AbstractGeneratorBuilder {
public:
    virtual Generator<T>* build() const = 0;
    virtual ~AbstractGeneratorBuilder() = default;
};

template<class T>
class AppendBuilder : public AbstractGeneratorBuilder<T> {
    LazySequence<T>* parent;
    T item;
public:
    AppendBuilder(LazySequence<T>* p, const T& i) : parent(p), item(i) {}
    Generator<T>* build() const override { return new AppendGenerator<T>(parent, item); }
};

template<class T>
class PrependBuilder : public AbstractGeneratorBuilder<T> {
    LazySequence<T>* parent;
    T item;
public:
    PrependBuilder(LazySequence<T>* p, const T& i) : parent(p), item(i) {}
    Generator<T>* build() const override { return new PrependGenerator<T>(parent, item); }
};

template<class T>
class InsertAtBuilder : public AbstractGeneratorBuilder<T> {
    LazySequence<T>* parent;
    T item;
    int index;
public:
    InsertAtBuilder(LazySequence<T>* p, const T& i, int idx) : parent(p), item(i), index(idx) {}
    Generator<T>* build() const override { return new InsertAtGenerator<T>(parent, item, index); }
};

// Замена std::function на строгий указатель на функцию: T (*transform)(const T&)
template<class T>
class MapBuilder : public AbstractGeneratorBuilder<T> {
    LazySequence<T>* parent;
    T (*transform)(const T&);
public:
    MapBuilder(LazySequence<T>* p, T (*t)(const T&)) : parent(p), transform(t) {}
    Generator<T>* build() const override { return new MapGenerator<T>(parent, transform); }
};

// Замена std::function на строгий указатель на функцию: bool (*predicate)(const T&)
template<class T>
class FilterBuilder : public AbstractGeneratorBuilder<T> {
    LazySequence<T>* parent;
    bool (*predicate)(const T&);
public:
    FilterBuilder(LazySequence<T>* p, bool (*pred)(const T&)) : parent(p), predicate(pred) {}
    Generator<T>* build() const override { return new FilterGenerator<T>(parent, predicate); }
};

template<class T>
class TakeBuilder : public AbstractGeneratorBuilder<T> {
    LazySequence<T>* parent;
    int count;
public:
    TakeBuilder(LazySequence<T>* p, int c) : parent(p), count(c) {}
    Generator<T>* build() const override { return new TakeGenerator<T>(parent, count); }
};

template<class T>
class ConcatBuilder : public AbstractGeneratorBuilder<T> {
    LazySequence<T>* parent;
    Sequence<T>* other;
public:
    ConcatBuilder(LazySequence<T>* p, Sequence<T>* o) : parent(p), other(o) {}
    Generator<T>* build() const override { return new ConcatGenerator<T>(parent, other); }
};

template<class T, class T2>
class ZipBuilder : public AbstractGeneratorBuilder<Pair<T, T2>> {
    LazySequence<T>* parent;
    Sequence<T2>* other;
public:
    ZipBuilder(LazySequence<T>* p, Sequence<T2>* o) : parent(p), other(o) {}
    Generator<Pair<T, T2>>* build() const override { return new ZipGenerator<T, T2>(parent, other); }
};

template<class T>
class SubsequenceBuilder : public AbstractGeneratorBuilder<T> {
    LazySequence<T>* parent;
    int start;
    int length;
public:
    SubsequenceBuilder(LazySequence<T>* p, int s, int l) : parent(p), start(s), length(l) {}
    Generator<T>* build() const override { return new SubsequenceGenerator<T>(parent, start, length); }
};

// ============================================================================
// ОСНОВНОЙ КЛАСС LAZY SEQUENCE
// ============================================================================

template<class T>
class LazySequence : public Sequence<T> {
private:
    WindowedCache<T>* memory_window;
    Generator<T>* engine;
    Cardinal seq_length;

    int max_window_size;
    int latest_generated_index;

    // FIXED: теперь через
    AbstractGeneratorBuilder<T>* engine_builder;

    void validate_bounds(int index) const {
        if (index < 0) {
            throw std::out_of_range("LazySequence: Index cannot be negative");
        }
        if (!seq_length.is_infinite() && index >= seq_length.get_offset()) {
            throw std::out_of_range("LazySequence: Index out of sequence bounds");
        }
    }

public:
    // Конструктор по умолчанию
    LazySequence()
        : memory_window(new WindowedCache<T>(100)), engine(nullptr), engine_builder(nullptr),
          latest_generated_index(-1), seq_length(Cardinal::infinity()), max_window_size(100) {}

    // Основной конструктор
    LazySequence(AbstractGeneratorBuilder<T>* builder, const Cardinal& len, int window_size = 100)
        : memory_window(new WindowedCache<T>(window_size)), engine_builder(builder),
          engine(builder ? builder->build() : nullptr), latest_generated_index(-1), seq_length(len), max_window_size(window_size) {}

    ~LazySequence() override {
        delete engine;
        delete memory_window;
        delete engine_builder; // Очищаем
    }

    const T& get(int index) override {
        validate_bounds(index);

        if (memory_window->has_index(index)) {
            return memory_window->retrieve(index);
        }

        // идем назад (ИСПРАВЛЕНО: вызов через интерфейс)
        if (index < latest_generated_index) {
            memory_window->reset();
            delete engine;
            engine = engine_builder ? engine_builder->build() : nullptr;
            latest_generated_index = -1;
        }

        while (latest_generated_index < index) {
            if (!engine || !engine->has_next()) {
                throw std::out_of_range("LazySequence: Generator unexpectedly exhausted");
            }
            T next_element = engine->get_next();
            memory_window->add(next_element, latest_generated_index + 1);
            latest_generated_index++;
        }

        return memory_window->retrieve(index);
    }

    const T& get(const Cardinal& index) {
        if (index.get_omega_count() == 0) {
            return get(index.get_offset());
        }

        static T value;
        value = engine->get(index);
        return value;
    }

    const T& get_first() override {
        return get(0);
    }

    const T& get_last() override {
        if (seq_length.is_infinite()) {
            throw std::logic_error("LazySequence: Infinite sequence has no last element");
        }
        return get(seq_length.get_offset() - 1);
    }

    Cardinal get_length() const override {
        return seq_length;
    }

    int get_materialized_count() const {
        return memory_window->get_size();
    }

    // ================== ИТЕРАТОР ==================

    class LazyEnumerator : public IEnumerator<T> {
    private:
        LazySequence<T>* parent_seq; //указатель на род последовательность
        int current_pos; // курсор

    public:
        LazyEnumerator(LazySequence<T>* seq) : parent_seq(seq), current_pos(0) {}

        bool has_more_elements() override {
            Cardinal len = parent_seq->get_length();
            if (len.is_infinite()) return true;
            return current_pos < len.get_offset();
        }

        const T& next() override {
            return parent_seq->get(current_pos++);
        }
    };

    IEnumerator<T>* get_enumerator() const override {
        return new LazyEnumerator(const_cast<LazySequence<T>*>(this));
    }

    // ================== ФУНКЦИИ И АЛГЕБРА ==================

    LazySequence<T>* append(const T& item) override {
        auto builder = new AppendBuilder<T>(this, item);
        Cardinal new_len(seq_length.get_omega_count(), seq_length.get_offset() + 1);
        return new LazySequence<T>(builder, new_len, max_window_size);
    }

    LazySequence<T>* prepend(const T& item) override {
        auto builder = new PrependBuilder<T>(this, item);
        Cardinal new_len(seq_length.get_omega_count(), seq_length.get_offset() + 1);
        return new LazySequence<T>(builder, new_len, max_window_size);
    }

    LazySequence<T>* insert_at(const T& item, int index) override {
        if (index < 0) throw std::out_of_range("Negative insert index");
        if (!seq_length.is_infinite() && index > seq_length.get_offset()) {
            throw std::out_of_range("Insert index out of bounds");
        }

        auto builder = new InsertAtBuilder<T>(this, item, index);
        Cardinal new_len(seq_length.get_omega_count(), seq_length.get_offset() + 1);
        return new LazySequence<T>(builder, new_len, max_window_size);
    }

    // FIXED: Принимает указатель на функцию
    LazySequence<T>* map(T (*transform)(const T&)) {
        auto builder = new MapBuilder<T>(this, transform);
        return new LazySequence<T>(builder, get_length(), max_window_size);
    }

    // FIXED: Принимает указатель на функцию
    LazySequence<T>* where(bool (*predicate)(const T&)) {
        auto builder = new FilterBuilder<T>(this, predicate);
        return new LazySequence<T>(builder, Cardinal::infinity(), max_window_size);
    }

    // FIXED: Принимает указатель на функцию
    T reduce(T (*reducer)(const T&, const T&), T initial) {
        if (seq_length.is_infinite()) {
            throw std::logic_error("Cannot reduce an infinite sequence");
        }

        T result = initial;
        auto it = get_enumerator();

        while (it->has_more_elements()) {
            result = reducer(result, it->next());
        }

        delete it;
        return result;
    }

    LazySequence<T>* take(int count) {
        if (count < 0) throw std::out_of_range("Negative count for take");
        auto builder = new TakeBuilder<T>(this, count);
        return new LazySequence<T>(builder, Cardinal(count), max_window_size);
    }

    LazySequence<T>* concat(Sequence<T>* other) {
        auto builder = new ConcatBuilder<T>(this, other);
        return new LazySequence<T>(builder, get_length() + other->get_length(), max_window_size);
    }

    template<class T2>
    LazySequence<Pair<T, T2>>* zip(Sequence<T2>* other) {
        auto builder = new ZipBuilder<T, T2>(this, other);

        Cardinal len1 = get_length();
        Cardinal len2 = other->get_length();
        Cardinal new_len;

        if (len1.is_infinite() && len2.is_infinite()) new_len = Cardinal::infinity();
        else if (len1.is_infinite()) new_len = len2;
        else if (len2.is_infinite()) new_len = len1;
        else new_len = Cardinal(std::min(len1.get_offset(), len2.get_offset()));

        return new LazySequence<Pair<T, T2>>(builder, new_len, max_window_size);
    }

    LazySequence<T>* get_subsequence(int start_index, int end_index) {
        if (start_index < 0 || end_index < start_index) {
            throw std::out_of_range("Invalid bounds for sub sequence");
        }
        validate_bounds(end_index);

        int result_size = end_index - start_index + 1;

        auto builder = new SubsequenceBuilder<T>(this, start_index, result_size);
        return new LazySequence<T>(builder, Cardinal(result_size), max_window_size);
    }

protected:
    void append_internal(const T&) override { throw std::logic_error("Internal mutation not supported"); }
    void prepend_internal(const T&) override { throw std::logic_error("Internal mutation not supported"); }
    void insert_at_internal(const T&, int) override { throw std::logic_error("Internal mutation not supported"); }
    void remove_at_internal(int) override { throw std::logic_error("Internal mutation not supported"); }

    Sequence<T>* instance() override { throw std::logic_error("Direct instance copying not supported"); }
    Sequence<T>* create_empty_sequence() const override { return new MutableArraySequence<T>(); }
};

#endif //LABA4_LAZY_SEQUENCE_H