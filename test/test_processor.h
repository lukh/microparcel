#ifndef TEST_PROCESSOR_H
#define TEST_PROCESSOR_H

#include <cppunit/TestCase.h>
#include <cppunit/TestFixture.h>

#include "microparcel.h"


template<uint8_t Size>
class tMessage2 : public microparcel::Message<Size>{
    public:
        uint8_t getByte(uint8_t b) {
            return this->data[b];
        }

        void setByte(uint8_t b, uint8_t data){
            this->data[b] = data;
        }

};




template <typename MsgType>
class DummyRouter{
    public:
        void process(MsgType &msg){
            CPPUNIT_ASSERT(msg.getByte(0) == 0x01);
            CPPUNIT_ASSERT(msg.getByte(1) == 0x02);
            CPPUNIT_ASSERT(msg.getByte(2) == 0x03);
        }
};

template <typename MsgType>
class DummyProcessor: public microparcel::MsgProcessor<DummyProcessor<MsgType>, DummyRouter<MsgType>, MsgType >{
    public:
        virtual void sendFrame(uint8_t *buffer, uint8_t buffer_size){
            CPPUNIT_ASSERT(buffer_size == 5);

            CPPUNIT_ASSERT(buffer[0] == 0xAA);
            CPPUNIT_ASSERT(buffer[1] == 0x81);
            CPPUNIT_ASSERT(buffer[2] == 0x82);
            CPPUNIT_ASSERT(buffer[3] == 0x83);
            CPPUNIT_ASSERT(buffer[4] == ((0xAA + 0x81 + 0x82 + 0x83) & 0xFF));
        }
};

class MicroParcelProcessorTest : public CppUnit::TestFixture { 
    CPPUNIT_TEST_SUITE(MicroParcelProcessorTest);
    CPPUNIT_TEST(testParse);
    CPPUNIT_TEST(testSend);
    CPPUNIT_TEST_SUITE_END();

    public:
        using tDummyProcessor = DummyProcessor<tMessage2<3>>;

        void setUp(){
        }

        void tearDown(){
        }

    protected: 
        void testParse(){
            processor.parse(0xAA);
            processor.parse(0x01);
            processor.parse(0x02);
            processor.parse(0x03);
            processor.parse(0xAA + 0x01 + 0x02 + 0x03);
        }
      

        void testSend(){
            tMessage2<3> msg;
            msg.setByte(0, 0x81);
            msg.setByte(1, 0x82);
            msg.setByte(2, 0x83);
            
            processor.send(msg);
        }
    private:
        tDummyProcessor processor;
};

#endif //TEST_PROCESSOR_H