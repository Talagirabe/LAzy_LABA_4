#ifndef LABA4_TAKE_GENERATOR_H
#define LABA4_TAKE_GENERATOR_H

#include "generator.h"
#include "../../LABA_2/sequence.h"
#include <stdexcept>

// Ограничивает выдачу элементов заданным лимитом
template<class T>
class TakeGenerator : public Generator<T> {
private:
    Sequence<T>* source;
    int limit;
    int produced; // Локальный счетчик

public:
    TakeGenerator(Sequence<T>* source, int limit)
        : source(source), limit(limit), produced(0) {}

    T get_next() override {
        if (produced >= limit)
            throw std::out_of_range("TakeGenerator: limit reached");
        return source->get(produced++);
    }

    bool has_next() const override {
        // Мы отдаем элементы, пока не упремся в лимит или пока не иссякнет источник
        Cardinal len = source->get_length();
        bool source_has_more = len.is_infinite() || produced < len.get_offset();
        return produced < limit && source_has_more;
    }

    T get(const Cardinal& index) override {
        if (index.get_omega_count() != 0)
            throw std::logic_error("TakeGenerator does not support omega indices");

        if (index.get_offset() >= limit)
            throw std::out_of_range("TakeGenerator: index out of range");

        return source->get(index.get_offset());
    }
};

#endif //LABA4_TAKE_GENERATOR_H