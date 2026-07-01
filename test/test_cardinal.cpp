#include <gtest/gtest.h>
#include "../src/utils/cardinal.h" // Укажи свой путь к cardinal.h

TEST(CardinalTest, ConstructorAndInfinity) {
    Cardinal finite(10);
    EXPECT_EQ(finite.get_omega_count(), 0);
    EXPECT_EQ(finite.get_offset(), 10);
    EXPECT_FALSE(finite.is_infinite());

    Cardinal inf = Cardinal::infinity();
    EXPECT_EQ(inf.get_omega_count(), 1);
    EXPECT_EQ(inf.get_offset(), 0);
    EXPECT_TRUE(inf.is_infinite());
}

TEST(CardinalTest, ArithmeticOperations) {
    Cardinal a(1, 5); // w + 5
    Cardinal b(2, 3); // 2w + 3

    Cardinal res = a + b;
    // Ожидаем 3w + 8
    EXPECT_EQ(res.get_omega_count(), 3);
    EXPECT_EQ(res.get_offset(), 8);
}