#ifndef LABA4_MAP_GENERATOR_H
#define LABA4_MAP_GENERATOR_H

#include "generator.h"
#include "../utils/cardinal.h"

template<class T>
class MapGenerator : public Generator<T> {
public:
    MapGenerator(Sequence<T>* source, T (*transform)(const T&));

    T get_next() override;
    T get(const Cardinal& index) override;
    bool has_next() const override;

private:
    Sequence<T>* source;                  // Исходная последовательность
    T (*transform)(const T&);             // указатель на функцию
    int current_index;                    // Курсор
};

template<class T>
MapGenerator<T>::MapGenerator(Sequence<T>* source, T (*transform)(const T&))
    : source(source), transform(transform), current_index(0) {}

template<class T>
T MapGenerator<T>::get_next() {
    // 1. Берем элемент из списка
    T value = source->get(current_index);
    current_index++;

    // 2. Прогоняем его через функцию
    return transform(value);
}

template<class T>
bool MapGenerator<T>::has_next() const {
    Cardinal length = source->get_length();

    // Бесконечный список при Map остается бесконечным
    if(length.is_infinite())
        return true;

    return current_index < length.get_offset();
}

template<class T>
T MapGenerator<T>::get(const Cardinal& index) {
    return transform(source->get(index));
}

#endif //LABA4_MAP_GENERATOR_H