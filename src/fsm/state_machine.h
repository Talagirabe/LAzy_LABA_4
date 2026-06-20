#ifndef LABA4_STATE_MACHINE_H
#define LABA4_STATE_MACHINE_H

#include <string>
#include <stdexcept>
#include <memory>
#include <vector>
#include "../streams/sequence_streams.h"

namespace fsm {

// 1. СОСТОЯНИЕ АВТОМАТА (State)
struct State {
    std::string id; // Название состояния
    bool is_final;

    State(const std::string& state_id, bool final_state = false)
        : id(state_id), is_final(final_state) {}
};

// 2. ПЕРЕХОД
template <typename T>
class Transition {
public:
    // Конструктор: запоминаем откуда, куда и по какому правилу идем.
    Transition(const std::string& from, const std::string& to, bool (*condition)(const T&))
        : from_state(from), to_state(to), condition_(condition) {}

    // Проверка: можно ли прямо сейчас пройти
    bool can_transition(const std::string& current_state, const T& input) const {
        // Проходим, если мы находимся в правильном начале И символ подошел под правило
        return (current_state == from_state) && condition_(input);
    }

    // Возвращает имя состояния, в которое ведет эта стрелочка
    std::string get_next_state() const { return to_state; }

private:
    std::string from_state; // Из какого состояния выходим
    std::string to_state;   // В какое переходим

    // Прямой указатель на функцию-предикат
    bool (*condition_)(const T&);
};

// 3. ГЛАВНЫЙ КЛАСС
template <typename T>
class StateMachine {
public:
    StateMachine() : current_state_id_("") {}

    void add_state(const std::string& id, bool is_final = false) {
        if (find_state(id) != -1) {
            throw std::invalid_argument("StateMachine: State already exists");
        }
        states_.push_back(std::make_shared<State>(id, is_final));
    }

    // Установка стартового состояния (откуда начинаем)
    void set_initial_state(const std::string& id) {
        if (find_state(id) == -1) {
            throw std::invalid_argument("StateMachine: Initial state not found");
        }
        initial_state_id_ = id;
        current_state_id_ = id;
    }

    // Принимаем указатель на функцию-проверку.
    void add_transition(const std::string& from, const std::string& to, bool (*condition)(const T&)) {
        if (find_state(from) == -1 || find_state(to) == -1) {
            throw std::invalid_argument("StateMachine: Invalid from/to state in transition");
        }
        transitions_.emplace_back(from, to, condition);
    }

    //Запуск автомата на чтение из потока
    bool process(InputStream<T>* input_stream) {
        if (initial_state_id_.empty()) {
            throw std::logic_error("StateMachine: Initial state is not set");
        }

        reset(); // Сбрасываем автомат в начальное состояние

        // Читаем элементы по одному из потока, пока они не закончатся
        while (!input_stream->is_end_of_stream()) {
            T current_input = input_stream->read(); // Достали символ
            bool transitioned = false;

            for (const auto& transition : transitions_) {
                if (transition.can_transition(current_state_id_, current_input)) {
                    current_state_id_ = transition.get_next_state();
                    transitioned = true;
                    break; // Переход сделан
                }
            }

            if (!transitioned) {
                return false;
            }
        }

        return is_in_final_state();
    }

    // Возвращает автомат в стартовое положение
    void reset() { current_state_id_ = initial_state_id_; }

    std::string get_current_state() const { return current_state_id_; }

private:
    std::vector<std::shared_ptr<State>> states_; // Список всех состояний
    std::vector<Transition<T>> transitions_;     // Список всех переходов

    std::string initial_state_id_; // Имя стартового состояния
    std::string current_state_id_; // Имя текущего состояния

    int find_state(const std::string& id) const {
        for (size_t i = 0; i < states_.size(); ++i) {
            if (states_[i]->id == id) return static_cast<int>(i);
        }
        return -1;
    }

    bool is_in_final_state() const {
        int idx = find_state(current_state_id_);
        if (idx != -1) {
            return states_[idx]->is_final;
        }
        return false;
    }
};

} // namespace fsm

#endif // LABA4_STATE_MACHINE_H