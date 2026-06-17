#ifndef LABA4_ZIP_GENERATOR_H
#define LABA4_ZIP_GENERATOR_H

#include "generator.h"
#include "../utils/pair.h"

// Склеивает элементы двух списков в пары
template<class T1, class T2>
class ZipGenerator : public Generator<Pair<T1, T2>> {
private:
    Sequence<T1>* first;
    Sequence<T2>* second;
    int current_index;

public:
    ZipGenerator(Sequence<T1>* first, Sequence<T2>* second)
        : first(first), second(second), current_index(0) {}

    Pair<T1, T2> get_next() override {
        Pair<T1, T2> result(first->get(current_index), second->get(current_index));
        current_index++;
        return result;
    }

    bool has_next() const override {
        Cardinal len1 = first->get_length();
        Cardinal len2 = second->get_length();

        // Если оба бесконечны — идем дальше
        if (len1.is_infinite() && len2.is_infinite()) return true;

        // Если хотя бы один список закончился — Zip тоже заканчивается
        if (!len1.is_infinite() && current_index >= len1.get_offset()) return false;
        if (!len2.is_infinite() && current_index >= len2.get_offset()) return false;

        return true;
    }

    Pair<T1, T2> get(const Cardinal& index) override {
        // Прямой доступ к элементам под одним индексом
        return Pair<T1, T2>(first->get(index), second->get(index));
    }
};

#endif //LABA4_ZIP_GENERATOR_H