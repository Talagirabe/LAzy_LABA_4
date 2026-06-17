#ifndef LABA4_INSERT_GENERATOR_H
#define LABA4_INSERT_GENERATOR_H

#include "generator.h"
#include <stdexcept>

template<class T>
class InsertAtGenerator : public Generator<T> {
private:
    Sequence<T>* source;
    T inserted_item;     //  элемент, который мы вставили
    int insert_index;    // Позиция вставки
    int current_index;

public:
    InsertAtGenerator(Sequence<T>* source, const T& item, int insert_index);

    T get_next() override;
    T get(const Cardinal& index) override;
    bool has_next() const override;
};

template<class T>
InsertAtGenerator<T>::InsertAtGenerator(Sequence<T>* source, const T& item, int insert_index)
    : source(source), inserted_item(item), insert_index(insert_index), current_index(0) {}

template<class T>
T InsertAtGenerator<T>::get_next() {
    // Если курсор дошел, выдаем наш новый элемент
    if(current_index == insert_index) {
        current_index++;
        return inserted_item;
    }

    // Рассчитываем, откуда брать элемент
    int source_index;
    if(current_index < insert_index) {
        source_index = current_index; // До вставки индексы совпадают
    } else {
        source_index = current_index - 1; // После вставки берем предыдущий элемент
    }

    current_index++;
    return source->get(source_index);
}

template<class T>
bool InsertAtGenerator<T>::has_next() const {
    Cardinal length = source->get_length();

    if(length.is_infinite())
        return true;

    // Поскольку мы логически добавили 1 элемент, +1
    return current_index <= length.get_offset();
}

template<class T>
T InsertAtGenerator<T>::get(const Cardinal& index) {
    // Чтобы не ломать логику, мы запрещаем вставку/получение по трансфинитным индексам для этой операции.
    if(index.get_omega_count() != 0)
        throw std::logic_error("InsertAtGenerator does not support omega indices");

    int pos = index.get_offset();

    if(pos == insert_index)
        return inserted_item;

    if(pos < insert_index)
        return source->get(pos);

    // сдвиг для прямого доступа
    return source->get(pos - 1);
}

#endif //LABA4_INSERT_GENERATOR_H