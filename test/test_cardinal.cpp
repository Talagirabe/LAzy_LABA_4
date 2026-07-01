#include <gtest/gtest.h>
#include "../src/utils/cardinal.h"

// 1. Проверка базовых конструкторов и фабрики бесконечности
TEST(CardinalTest, ConstructorsAndInfinity) {
    Cardinal finite(10);
    EXPECT_EQ(finite.get_omega_count(), 0) << "У конечного числа нет бесконечностей";
    EXPECT_EQ(finite.get_offset(), 10);
    EXPECT_FALSE(finite.is_infinite());

    Cardinal inf = Cardinal::infinity();
    EXPECT_TRUE(inf.is_infinite());
    EXPECT_EQ(inf.get_omega_count(), 1) << "Омега содержит ровно 1 бесконечность";
    EXPECT_EQ(inf.get_offset(), 0);
}

// 2. Проверка операторов сравнения (== и !=)
TEST(CardinalTest, ComparisonOperators) {
    Cardinal a(1, 5); // 1w + 5
    Cardinal b(1, 5); // 1w + 5
    Cardinal c(1, 6); // 1w + 6

    EXPECT_TRUE(a == b);
    EXPECT_FALSE(a != b);
    EXPECT_TRUE(a != c);
}

// 3. Проверка трансфинитного сложения
TEST(CardinalTest, Addition) {
    Cardinal a(1, 5); // 1w + 5
    Cardinal b(2, 3); // 2w + 3

    Cardinal res = a + b;
    // Ожидаем 3w + 8
    EXPECT_EQ(res.get_omega_count(), 3);
    EXPECT_EQ(res.get_offset(), 8);
}

// 4. Проверка трансфинитного вычитания
TEST(CardinalTest, Subtraction) {
    Cardinal a(3, 10); // 3w + 10
    Cardinal b(1, 3);  // 1w + 3

    Cardinal res = a - b;
    // Ожидаем 2w + 7
    EXPECT_EQ(res.get_omega_count(), 2);
    EXPECT_EQ(res.get_offset(), 7);
}