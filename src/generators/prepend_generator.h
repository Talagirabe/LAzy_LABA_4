#ifndef LABA4_PREPEND_GENERATOR_H
#define LABA4_PREPEND_GENERATOR_H

#include "generator.h"
#include <stdexcept>

template<class T>
class PrependGenerator : public Generator<T> {
private:
    Sequence<T>* source;
    T prepended_item; // Элемент, который теперь станет головой
    int current_index;

public:
    PrependGenerator(Sequence<T>* source, const T& item);

    T get_next() override;
    T get(const Cardinal& index) override;
    bool has_next() const override;
};

template<class T>
PrependGenerator<T>::PrependGenerator(Sequence<T>* source, const T& item)
    : source(source), prepended_item(item), current_index(0) {}

template<class T>
T PrependGenerator<T>::get_next() {
    // Самый первый вызов всегда возвращает наш новый элемент
    if(current_index == 0) {
        current_index++;
        return prepended_item;
    }

    // Все последующие вызовы сдвигаются на -1
    return source->get(current_index++ - 1);
}

template<class T>
bool PrependGenerator<T>::has_next() const {
    Cardinal length = source->get_length();

    if(length.is_infinite())
        return true;

    // Длина логически увеличилась на 1
    return current_index <= length.get_offset();
}

template<class T>
T PrependGenerator<T>::get(const Cardinal& index) {
    if(index.get_omega_count() != 0)
        throw std::logic_error("PrependGenerator does not support omega indices");

    int pos = index.get_offset();

    // Если просят первый элемент, отдаем наш
    if(pos == 0)
        return prepended_item;

    return source->get(pos - 1);
}

#endif //LABA4_PREPEND_GENERATOR_H