#ifndef LABA4_CARDINAL_H
#define LABA4_CARDINAL_H

#include <stdexcept>

class Cardinal {

private:
    int omega_count; // кол-во бесконечностей
    int offset;      // Конечное число

public:

    Cardinal() : omega_count(0), offset(0) {}

    Cardinal(int offset) : omega_count(0), offset(offset) {}

    Cardinal(int omega_count, int offset): omega_count(omega_count), offset(offset) {}

    static Cardinal infinity() {
        return Cardinal(1, 0);
    }

    bool is_infinite() const {
        return omega_count > 0;
    }

    int get_omega_count() const {
        return omega_count;
    }

    int get_offset() const {
        return offset;
    }

    bool operator==(const Cardinal& other) const {
        return omega_count == other.omega_count && offset == other.offset;
    }

    bool operator!=(const Cardinal& other) const {
        return !(*this == other);
    }

    Cardinal operator+(const Cardinal& other) const {
        return Cardinal(omega_count + other.omega_count,offset + other.offset);
    }

    Cardinal operator-(const Cardinal& other) const {
        int new_omega = omega_count - other.omega_count;
        int new_offset = offset - other.offset;

        if(new_omega < 0)
            throw std::logic_error("Negative omega count");

        if(new_offset < 0)
            throw std::logic_error("Negative offset");

        return Cardinal(new_omega,new_offset);
    }
};




#endif //LABA4_CARDINAL_H
