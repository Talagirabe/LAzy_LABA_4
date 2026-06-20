template <typename T>
class ConcatGenerator : public Generator<T> {
private:
    Sequence<T>* first_;
    Sequence<T>* second_;
    int index_;

public:
    ConcatGenerator(Sequence<T>* first, Sequence<T>* second)
        : first_(first), second_(second), index_(0) {}

    T get_next() override {
        Cardinal first_length = first_->get_length();

        // Если первая часть бесконечна, get_next всегда тянет из нее
        if (first_length.is_infinite()) {
            return first_->get(index_++);
        }

        // Если конечна — читаем ее до конца
        if (index_ < first_length.get_offset()) {
            return first_->get(index_++);
        }

        // Переключение на вторую часть со сдвигом индекса
        return second_->get(index_++ - first_length.get_offset());
    }

    bool has_next() const override {
        Cardinal first_length = first_->get_length();
        Cardinal second_length = second_->get_length();

        if (first_length.is_infinite() || second_length.is_infinite()) {
            return true;
        }

        return index_ < (first_length.get_offset() + second_length.get_offset());
    }

    T get(const Cardinal& index) override {
        Cardinal first_length = first_->get_length();

        if (index.get_omega_count() < first_length.get_omega_count()) {
            return first_->get(index);
        }

        return second_->get(Cardinal(
            index.get_omega_count() - first_length.get_omega_count(),
            index.get_offset()
        ));
    }
};