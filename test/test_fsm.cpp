#include <gtest/gtest.h>
#include "../src/fsm/state_machine.h"
#include "../LABA_2/mutableArraySequence.h"

bool is_binary_char(const char& c) {
    return c == '0' || c == '1'; 
}

class StateMachineTest : public ::testing::Test {
protected:
    fsm::StateMachine<char> machine;

    void SetUp() override {
        machine.add_state("START", false);
        machine.add_state("BIN_READING", true);
        machine.set_initial_state("START");

        machine.add_transition("START", "BIN_READING", is_binary_char);
        machine.add_transition("BIN_READING", "BIN_READING", is_binary_char);
    }
};

TEST_F(StateMachineTest, ValidStream) {
    char data[] = {'1', '0', '1'};
    Sequence<char>* seq = new MutableArraySequence<char>(data, 3);
    SequenceInputStream<char> stream(seq);
    
    stream.open();
    EXPECT_TRUE(machine.process(&stream));
    EXPECT_EQ(machine.get_current_state(), "BIN_READING");
    
    stream.close();
    delete seq;
}

TEST_F(StateMachineTest, InvalidStreamRejected) {
    char data[] = {'1', '0', '2'}; // '2' сломает логику
    Sequence<char>* seq = new MutableArraySequence<char>(data, 3);
    SequenceInputStream<char> stream(seq);
    
    stream.open();
    EXPECT_FALSE(machine.process(&stream));
    
    stream.close();
    delete seq;
}