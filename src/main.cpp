#include "generators/generator.h"
#include <iostream>
#include <string>
#include <limits>
#include "../LABA_2/mutableArraySequence.h"
#include "lazy/lazy_sequence.h"
#include "fsm/state_machine.h"

int read_int(int min, int max) {
    int value;
    std::cin >> value;

    while (std::cin.fail() || value < min || value > max) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Ошибка! Введите число от " << min << " до " << max << ": ";
        std::cin >> value;
    }
    return value;
}

// Предикаты для датчиков Умного Дома
bool is_motion(const std::string& sensor_data) { return sensor_data == "ДВИЖЕНИЕ"; }
bool is_silence(const std::string& sensor_data) { return sensor_data == "ТИШИНА"; }
bool is_night(const std::string& sensor_data) { return sensor_data == "НОЧЬ"; }
bool is_morning(const std::string& sensor_data) { return sensor_data == "УТРО"; }

void setup_smart_home_fsm(fsm::StateMachine<std::string>& machine) {
    // 1. Регистрируем состояния комнатного контроллера
    machine.add_state("EMPTY_ROOM", false);   // Комната пуста (всё выключено, энергосбережение)
    machine.add_state("LIGHTS_ON", false);    // В комнате человек (включен свет и климат-контроль)
    machine.add_state("NIGHT_MODE", true);    // Человек лег спать (Финальное/Успешное состояние комфорта)

    // Указываем начальное состояние
    machine.set_initial_state("EMPTY_ROOM");

    // 2. Настраиваем логику переходов (Граф автомата)

    // Если в пустой комнате сработало ДВИЖЕНИЕ — включаем свет и переходим в LIGHTS_ON
    machine.add_transition("EMPTY_ROOM", "LIGHTS_ON", is_motion);

    // Если человек находится в комнате и наступила ТИШИНА (он ушел) — возвращаемся в EMPTY_ROOM
    machine.add_transition("LIGHTS_ON", "EMPTY_ROOM", is_silence);

    // Если человек в комнате и наступила НОЧЬ — переводим систему в уютный NIGHT_MODE (приглушаем свет)
    machine.add_transition("LIGHTS_ON", "NIGHT_MODE", is_night);

    // Если в ночном режиме наступило УТРО — возвращаем обычный свет (LIGHTS_ON)
    machine.add_transition("NIGHT_MODE", "LIGHTS_ON", is_morning);
}

int main() {
// Инициализируем автомат Умного Дома
    fsm::StateMachine<std::string> smart_home;
    setup_smart_home_fsm(smart_home);

    int choice = 0;
    while (choice != 2) {
        std::cout << "\n==== Контроллер Умного Дома (Smart Home FSM) ====\n";
        std::cout << "  1. Симулировать сигналы с датчиков\n";
        std::cout << "  2. Выход\n";
        std::cout << "Выберите действие: ";

        choice = read_int(1, 2);

        if (choice == 1) {
            std::cout << "\nВыберите жизненный сценарий:\n";
            std::cout << "  1. Идеальный вечер: зашел и лег спать (ДВИЖЕНИЕ -> НОЧЬ)\n";
            std::cout << "  2. Короткий визит: зашел, побыл и ушел (ДВИЖЕНИЕ -> ТИШИНА)\n";
            std::cout << "  3. Полный цикл: вечер, сон и утро (ДВИЖЕНИЕ -> НОЧЬ -> УТРО)\n";
            std::cout << "  4. Сложный день: зашел, ушел, снова зашел и уснул (ДВИЖЕНИЕ -> ТИШИНА -> ДВИЖЕНИЕ -> НОЧЬ)\n";
            std::cout << "  5. Сбой датчика: сигнал из пустой комнаты (НОЧЬ)\n";
            std::cout << "Ваш выбор: ";

            int scenario = read_int(1, 5);

            // Создаем последовательность для сигналов датчиков из Лабы 2
            Sequence<std::string>* sensor_sequence = new MutableArraySequence<std::string>();

            if (scenario == 1) {
                // Успешный финал в NIGHT_MODE (true)
                sensor_sequence = sensor_sequence->append("ДВИЖЕНИЕ")->append("НОЧЬ");
            }
            else if (scenario == 2) {
                // Возврат в EMPTY_ROOM (false, так как не финал)
                sensor_sequence = sensor_sequence->append("ДВИЖЕНИЕ")->append("ТИШИНА");
            }
            else if (scenario == 3) {
                // Переход EMPTY_ROOM -> LIGHTS_ON -> NIGHT_MODE -> LIGHTS_ON (false, утро — не финал)
                sensor_sequence = sensor_sequence->append("ДВИЖЕНИЕ")->append("НОЧЬ")->append("УТРО");
            }
            else if (scenario == 4) {
                // Длинная цепочка переходов, финал в NIGHT_MODE (true)
                sensor_sequence = sensor_sequence->append("ДВИЖЕНИЕ")->append("ТИШИНА")->append("ДВИЖЕНИЕ")->append("НОЧЬ");
            }
            else if (scenario == 5) {
                // Автомат должен заклинить на первом же шаге и вернуть false.
                sensor_sequence = sensor_sequence->append("НОЧЬ");
            }

            // Упаковываем ленивые сигналы в поток ввода
            SequenceInputStream<std::string> sensor_stream(sensor_sequence);
            sensor_stream.open();

            std::cout << "-----------------------------------------------------------\n";
            std::cout << "Потоковый анализ сигналов датчиков Умного Дома...\n";

            // Передаем поток контроллеру (автомату)
            bool is_comfortable_night = smart_home.process(&sensor_stream);

            sensor_stream.close();

            std::cout << "\n[КОНЕЧНЫЙ РЕЖИМ СИСТЕМЫ]: " << smart_home.get_current_state() << "\n";

            if (is_comfortable_night) {
                std::cout << "РЕЗУЛЬТАТ: Поток успешно обработан. Система зафиксирована в комфортном ночном режиме (NIGHT_MODE).\n";
            } else {
                if (smart_home.get_current_state() == "EMPTY_ROOM" && scenario == 5) {
                    std::cout << "РЕЗУЛЬТАТ: Обнаружена аномалия! Автомат отверг некорректный поток сигналов (Защита сработала).\n";
                } else if (smart_home.get_current_state() == "LIGHTS_ON") {
                    std::cout << "РЕЗУЛЬТАТ: Поток успешно обработан. В комнате горит стандартный свет, работает климат-контроль.\n";
                } else {
                    std::cout << "РЕЗУЛЬТАТ: Поток обработан. Система вернулась в энергосберегающий режим ожидания.\n";
                }
            }
            std::cout << "-----------------------------------------------------------\n";

            delete sensor_sequence;
        }
    }
    std::cout << "Система отключена. До свидания!\n";
    return 0;
}