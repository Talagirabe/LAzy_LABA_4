#ifndef LABA4_CONCAT_GENERATOR_H
#define LABA4_CONCAT_GENERATOR_H

#include "generator.h"
#include "../../LABA_2/sequence.h"

template<class T>
class ConcatGenerator : public Generator<T> {
private:
    Sequence<T>* first;
    Sequence<T>* second;
    int current_index; // курсор для  обхода

public:
    ConcatGenerator(Sequence<T>* first, Sequence<T>* second);

    T get_next() override;
    T get(const Cardinal& index) override;
    bool has_next() const override;
};

template<class T>
ConcatGenerator<T>::ConcatGenerator(Sequence<T>* first, Sequence<T>* second)
    : first(first), second(second), current_index(0) {}

template<class T>
T ConcatGenerator<T>::get_next() {
    Cardinal first_length = first->get_length();

    // Если первая часть бесконечна, вторая для get_next() недостижима.
    if(first_length.is_infinite())
        return first->get(current_index++);

    // Пока находимся в границах первого списка
    if(current_index < first_length.get_offset())
        return first->get(current_index++);

    // Как только первый закончился, вычитаем его длину из курсора,
    // чтобы получить индекс (начиная с 0) для второго списка.
    return second->get(current_index++ - first_length.get_offset());
}

template<class T>
T ConcatGenerator<T>::get(const Cardinal& index) {
    Cardinal first_length = first->get_length();

    // Алгебра ординалов: если запрашиваемый порядок бесконечности меньше,
    // чем у первого списка (например, ищем конечный индекс в бесконечном first),
    // то элемент точно лежит в first.
    if(index.get_omega_count() < first_length.get_omega_count())
        return first->get(index);

    // Если порядок бесконечности равен или больше, элемент лежит во втором списке.
    // Сдвигаем порядок бесконечности вниз (вычитаем омеги первого списка).
    return second->get(Cardinal(
        index.get_omega_count() - first_length.get_omega_count(),
        index.get_offset()
    ));
}

template<class T>
bool ConcatGenerator<T>::has_next() const {
    Cardinal first_length = first->get_length();
    Cardinal second_length = second->get_length();

    // Если хотя бы один из списков бесконечен, то и их конкатенация бесконечна.
    if(first_length.is_infinite() || second_length.is_infinite())
        return true;

    // Иначе проверяем, не вышли ли за суммарную длину.
    return current_index < first_length.get_offset() + second_length.get_offset();
}

#endif //LABA4_CONCAT_GENERATOR_H