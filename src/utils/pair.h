#ifndef LABA4_PAIR_H
#define LABA4_PAIR_H

template<class T1, class T2>
struct Pair { // data holder
    T1 first;
    T2 second;

    Pair()
        : first(), second() {}

    Pair(const T1& first,const T2& second)
        : first(first), second(second) {}
};

#endif //LABA4_PAIR_H