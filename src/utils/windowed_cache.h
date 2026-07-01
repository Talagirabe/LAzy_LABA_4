#ifndef LABA4_WINDOWED_CACHE_H
#define LABA4_WINDOWED_CACHE_H

#include "../../LABA_2/dynamicArray.h"
#include <stdexcept>

template<class T>
class WindowedCache {
private:
    DynamicArray<T> storage;

    int max_size;            // максимальный размер окна
    int current_elements;    // сколько сейчас элементов внутри

    int physical_head_pos;   // где в памяти начало номер ячейки
    int logical_start_idx;   // какой индекс у первого элемента
    int logical_end_idx;     // какой индекс у последнего

public:
    explicit WindowedCache(int limit = 100);

    bool is_empty() const;
    int get_size() const; // уже есть
    int get_limit() const; // максимум

    int get_start_bound() const;
    int get_end_bound() const;

    //проверяет, попадает ли запрошенный индекс в рамки нашего окна
    bool has_index(int target_logical_index) const;

    // Получить элемент по логическому индексу
    const T& retrieve(int target_logical_index) const;

    //добавление нового элемента и сдвиг окна
    void add(const T& new_item, int logical_index);

    void reset();
};

template<class T>
WindowedCache<T>::WindowedCache(int limit)
    : storage(limit), max_size(limit), current_elements(0),
      physical_head_pos(0), logical_start_idx(0), logical_end_idx(0) {
    if(limit < 1) {
        throw std::invalid_argument("WindowedCache: limit must be at least 1");
    }
}

template<class T>
bool WindowedCache<T>::is_empty() const {
    return current_elements == 0;
}

template<class T>
int WindowedCache<T>::get_size() const {
    return current_elements;
}

template<class T>
int WindowedCache<T>::get_limit() const {
    return max_size;
}

template<class T>
int WindowedCache<T>::get_start_bound() const {
    if(current_elements == 0) throw std::logic_error("Cache is empty, no bounds available");
    return logical_start_idx;
}

template<class T>
int WindowedCache<T>::get_end_bound() const {
    if(current_elements == 0) throw std::logic_error("Cache is empty, no bounds available");
    return logical_end_idx;
}

template<class T>
bool WindowedCache<T>::has_index(int target_logical_index) const {
    if(current_elements == 0) return false;

    // Проверяем, находится ли индекс между началом и концом скользящего окна
    return (target_logical_index >= logical_start_idx) && (target_logical_index <= logical_end_idx);
}

template<class T>
const T& WindowedCache<T>::retrieve(int target_logical_index) const {
    if(!has_index(target_logical_index)) {
        throw std::out_of_range("Requested index has fallen out of the sliding window");
    }

    // Вычисляем смещение от начала окна
    int offset = target_logical_index - logical_start_idx;

    // переводим логическое смещение в физический индекс массива
    int real_memory_index = (physical_head_pos + offset) % max_size;

    return storage.get(real_memory_index);
}

template<class T>
void WindowedCache<T>::add(const T& new_item, int logical_index) {
    if (current_elements == 0) {
        storage.set(new_item, 0);

        physical_head_pos = 0;
        logical_start_idx = logical_index;
        logical_end_idx = logical_index;
        current_elements = 1;
        return;
    }

    if (current_elements < max_size) {
        int next_free_slot = (physical_head_pos + current_elements) % max_size;
        storage.set(new_item, next_free_slot);

        logical_end_idx = logical_index;
        current_elements++;
        return;
    }

    storage.set(new_item, physical_head_pos);

    // Сдвигаем физическую на 1 шаг
    physical_head_pos = (physical_head_pos + 1) % max_size;

    // Обновляем логические рамки окна
    logical_start_idx = logical_index - max_size + 1;
    logical_end_idx = logical_index;
}

template<class T>
void WindowedCache<T>::reset() {
    physical_head_pos = 0;
    logical_start_idx = 0;
    logical_end_idx = 0;
    current_elements = 0;
}

#endif //LABA4_WINDOWED_CACHE_H