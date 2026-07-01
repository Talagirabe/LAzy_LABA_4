#include <gtest/gtest.h>
#include <stdexcept>
#include "../LABA_2/mutableArraySequence.h"
#include "../src/streams/sequence_streams.h" 

// --- ТЕСТЫ ВВОДА (ЧТЕНИЯ) ---

TEST(InputStreamTest, ReadThrowsWhenClosed) {
    int data[] = {10, 20, 30};
    Sequence<int>* seq = new MutableArraySequence<int>(data, 3);
    SequenceInputStream<int> stream(seq);

    //чтение до open() должно кидать ошибку
    EXPECT_THROW(stream.read(), std::logic_error);
    delete seq;
}

TEST(InputStreamTest, OpenReadAndSeek) {
    int data[] = {10, 20, 30};
    Sequence<int>* seq = new MutableArraySequence<int>(data, 3);
    SequenceInputStream<int> stream(seq);

    stream.open();
    EXPECT_EQ(stream.get_position(), 0);

    EXPECT_EQ(stream.read(), 10);
    EXPECT_EQ(stream.read(), 20);
    EXPECT_EQ(stream.get_position(), 2) << "Курсор должен сдвинуться на 2 позиции";

    // Проверяем перемотку назад
    EXPECT_TRUE(stream.can_seek());
    stream.seek(0);
    EXPECT_EQ(stream.read(), 10) << "После seek(0) снова читаем первый элемент";

    stream.close();
    delete seq;
}

// --- ТЕСТЫ ВЫВОДА (ЗАПИСИ) ---

TEST(OutputStreamTest, WriteThrowsWhenClosed) {
    Sequence<int>* seq = new MutableArraySequence<int>();
    SequenceOutputStream<int> stream(seq);

    EXPECT_THROW(stream.write(42), std::logic_error);
    delete seq;
}

TEST(OutputStreamTest, WriteAppendsToSequence) {
    Sequence<int>* seq = new MutableArraySequence<int>();
    SequenceOutputStream<int> stream(seq);

    stream.open();
    stream.write(100);
    stream.write(200);

    EXPECT_EQ(stream.get_position(), 2);
    EXPECT_EQ(stream.get_target()->get(0), 100);
    EXPECT_EQ(stream.get_target()->get(1), 200);

    stream.close();
    delete seq;
}