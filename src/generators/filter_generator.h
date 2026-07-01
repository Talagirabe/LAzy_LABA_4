#ifndef LABA4_FILTER_GENERATOR_H
#define LABA4_FILTER_GENERATOR_H

#include "generator.h"
#include <stdexcept>

// Оставляет только те элементы, которые удовлетворяют предикату
template<class T>
class FilterGenerator : public Generator<T> {
private:
    Sequence<T>* source;
    bool (*predicate)(const T&); // указатель на функцию
    int current_index;

public:
    // В конструктор передаем указатель на функцию
    FilterGenerator(Sequence<T>* source, bool (*predicate)(const T&))
        : source(source), predicate(predicate), current_index(0) {}

    T get_next() override {
        Cardinal len = source->get_length();

        //пока не найдем элемент или не закончится
        while (len.is_infinite() || current_index < len.get_offset()) {
            T value = source->get(current_index++);
            if (predicate(value)) {
                return value;
            }
        }
        throw std::out_of_range("FilterGenerator: reached end of sequence");
    }

    bool has_next() const override {
        if (source->get_length().is_infinite()) return true;
        return current_index < source->get_length().get_offset();
    }

    T get(const Cardinal& index) override {
        if (index.get_omega_count() != 0)
            throw std::logic_error("FilterGenerator does not support omega indices");

        int target = index.get_offset();
        int found = -1; // счётчик
        int source_index = 0;
        Cardinal len = source->get_length();

        // Вычисляем заново. Сложность O(N).
        while (len.is_infinite() || source_index < len.get_offset()) {
            T value = source->get(source_index++);
            if (predicate(value)) {
                found++;
                if (found == target) return value;
            }
        }
        throw std::out_of_range("FilterGenerator: element not found");
    }
};

#endif //LABA4_FILTER_GENERATOR_H