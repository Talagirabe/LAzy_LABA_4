#include <gtest/gtest.h>
#include <stdexcept>
#include "../src/utils/windowed_cache.h"
// 1. Инициализация и защита от некорректных размеров
TEST(WindowedCacheTest, InitializationAndLimits) {
    WindowedCache<int> cache(3);

    EXPECT_TRUE(cache.is_empty());
    EXPECT_EQ(cache.get_limit(), 3);
    EXPECT_EQ(cache.get_size(), 0);

    // Попытка получить границы пустой коллекции должна бросить ошибку
    EXPECT_THROW(cache.get_start_bound(), std::logic_error);

    // Нельзя создать кэш нулевого размера
    EXPECT_THROW(WindowedCache<int> invalid(0), std::invalid_argument);
}

// 2. Добавление элементов без вытеснения (когда место еще есть)
TEST(WindowedCacheTest, AddWithoutSliding) {
    WindowedCache<int> cache(5);
    cache.add(10, 0); // значение 10 на индекс 0
    cache.add(20, 1); // значение 20 на индекс 1

    EXPECT_FALSE(cache.is_empty());
    EXPECT_EQ(cache.get_size(), 2);

    EXPECT_TRUE(cache.has_index(0));
    EXPECT_TRUE(cache.has_index(1));

    EXPECT_EQ(cache.get_start_bound(), 0);
    EXPECT_EQ(cache.get_end_bound(), 1);

    EXPECT_EQ(cache.retrieve(0), 10);
    EXPECT_EQ(cache.retrieve(1), 20);
}

// 3. ГЛАВНЫЙ ТЕСТ: Вытеснение старых элементов (сдвиг окна)
TEST(WindowedCacheTest, SlidingBehavior) {
    WindowedCache<int> cache(3); // Размер всего 3!

    cache.add(10, 0);
    cache.add(20, 1);
    cache.add(30, 2);
    // Кэш заполнен: [10, 20, 30]. Логические индексы: 0..2

    cache.add(40, 3); // Вытесняет 10. Кэш: [20, 30, 40]. Индексы: 1..3
    cache.add(50, 4); // Вытесняет 20. Кэш: [30, 40, 50]. Индексы: 2..4

    EXPECT_EQ(cache.get_size(), 3); // Размер не должен превысить лимит
    EXPECT_EQ(cache.get_start_bound(), 2); // Самый старый индекс теперь 2
    EXPECT_EQ(cache.get_end_bound(), 4);   // Самый новый индекс теперь 4

    EXPECT_FALSE(cache.has_index(0));
    EXPECT_FALSE(cache.has_index(1));
    EXPECT_TRUE(cache.has_index(2));
    EXPECT_TRUE(cache.has_index(4));

    EXPECT_EQ(cache.retrieve(2), 30);
    EXPECT_EQ(cache.retrieve(4), 50);

    // Попытка достать вытесненный элемент должна выбросить out_of_range
    EXPECT_THROW(cache.retrieve(1), std::out_of_range);
}

// 4. Очистка кэша
TEST(WindowedCacheTest, ResetCache) {
    WindowedCache<int> cache(3);
    cache.add(10, 0);
    cache.reset();

    EXPECT_TRUE(cache.is_empty());
    EXPECT_EQ(cache.get_size(), 0);
}