#ifndef LABA4_RULE_GENERATOR_H
#define LABA4_RULE_GENERATOR_H

#include "generator.h"
#include "../../LABA_2/sequence.h"
#include <stdexcept>
// ИСПРАВЛЕНО: Полностью удалили #include <functional>

template<class T>
class RuleGenerator : public Generator<T> {
public:
    // ИСПРАВЛЕНО: Конструктор теперь принимает классический указатель на функцию-правило
    RuleGenerator(T (*rule)(Sequence<T>*), Sequence<T>* materialized, bool infinite = true);

    T get_next() override;
    T get(const Cardinal& index) override;
    bool has_next() const override;

private:
    T (*rule)(Sequence<T>*);             // ИСПРАВЛЕНО: Математическое правило в виде указателя на функцию
    Sequence<T>* materialized;           // Кэш уже вычисленных элементов (принадлежит LazySequence)
    bool infinite;                       // Является ли последовательность бесконечной
};

// ИСПРАВЛЕНО: Сигнатура конструктора изменена, std::move удален за ненадобностью
template<class T>
RuleGenerator<T>::RuleGenerator(T (*rule)(Sequence<T>*), Sequence<T>* materialized, bool infinite)
    : rule(rule), materialized(materialized), infinite(infinite) {}

template<class T>
T RuleGenerator<T>::get_next() {
    // ИСПРАВЛЕНО: Переданная функция-правило сама решает, какие элементы из кэша ей нужны
    return rule(materialized);
}

template<class T>
bool RuleGenerator<T>::has_next() const {
    return infinite;
}

template<class T>
T RuleGenerator<T>::get(const Cardinal&) {
    // Эту логику (последовательное вычисление до индекса) возьмет на себя LazySequence.
    throw std::logic_error("RuleGenerator does not support direct indexed access. LazySequence should iterate to reach this index.");
}

#endif //LABA4_RULE_GENERATOR_H