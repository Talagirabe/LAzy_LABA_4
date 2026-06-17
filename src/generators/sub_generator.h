
#ifndef LABA4_SUB_GENERATOR_H
#define LABA4_SUB_GENERATOR_H

template<class T>
class SubsequenceGenerator : public Generator<T> {
private:
    Sequence<T>* source;
    int start_index;
    int limit;
    int current_produced;

public:
    SubsequenceGenerator(Sequence<T>* source, int start_index, int limit)
        : source(source), start_index(start_index), limit(limit), current_produced(0) {}

    T get_next() override {
        if (current_produced >= limit) throw std::out_of_range("SubsequenceGenerator exhausted");
        return source->get(start_index + current_produced++);
    }

    bool has_next() const override {
        return current_produced < limit;
    }

    T get(const Cardinal& index) override {
        if (index.get_offset() >= limit) throw std::out_of_range("Index out of subsequence bounds");
        return source->get(start_index + index.get_offset());
    }
};

#endif //LABA4_SUB_GENERATOR_H
