#include <gtest/gtest.h>
#include <stdexcept>
#include "../LABA_2/mutableArraySequence.h"
#include "../src/streams/sequence_streams.h"

TEST(StreamTest, ReadThrowsWhenClosed) {
    int data[] = {10, 20, 30, 40};
    Sequence<int>* seq = new MutableArraySequence<int>(data, 4);
    SequenceInputStream<int> stream(seq);

    EXPECT_THROW(stream.read(), std::logic_error);
    delete seq;
}

TEST(StreamTest, OpenAndReadPosition) {
    int data[] = {10, 20, 30, 40};
    Sequence<int>* seq = new MutableArraySequence<int>(data, 4);
    SequenceInputStream<int> stream(seq);

    stream.open();
    EXPECT_EQ(stream.get_position(), 0);
    EXPECT_EQ(stream.read(), 10);
    EXPECT_EQ(stream.read(), 20);
    EXPECT_EQ(stream.get_position(), 2);

    stream.seek(0);
    EXPECT_EQ(stream.read(), 10);

    stream.close();
    delete seq;
}