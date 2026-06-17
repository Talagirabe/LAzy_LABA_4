#ifndef LABA4_GENERATOR_H
#define LABA4_GENERATOR_H

#include "../../LABA_2/sequence.h"

template<class T>
class Generator {
public:
    virtual ~Generator() = default;


    // Вычисляет и возвращает следующий элемент, сдвигая внутренний курсор.
    virtual T get_next() = 0;
    virtual T get(const Cardinal& index) = 0;

    //Проверяет, можно ли сгенерировать еще элементы.Для бесконечных последовательностей всегда возвращает true.
    virtual bool has_next() const = 0;
};

#endif //LABA4_GENERATOR_H