#include <cppunit/TestCase.h>
#include <cppunit/TestFixture.h>

#include "microparcel.h"

template<uint8_t Size>
class tMessage : public microparcel::Message<Size>{
    public:
        uint8_t getByte(uint8_t b) {
            return this->data[b];
        }

        void setByte(uint8_t b, uint8_t data){
            this->data[b] = data;
        }

};

class MicroParcelMessageTest : public CppUnit::TestFixture { 
    CPPUNIT_TEST_SUITE(MicroParcelMessageTest);
    CPPUNIT_TEST(testGet8bits);
    CPPUNIT_TEST(testSet8bits);
    CPPUNIT_TEST(testGet16bits);
    CPPUNIT_TEST(testSet16bits);
    CPPUNIT_TEST_SUITE_END();

    public:
        void setUp(){
            msg = new tMessage<8>();
        }

        void tearDown(){
            delete msg;
        }

    protected: 
        void testGet8bits() {
            uint8_t b;

            // full nibble, off 0
            msg->setByte(0, 0xF);
            b = msg->get<uint8_t, 0, 4>();
            CPPUNIT_ASSERT(b == 0xF);
            b = msg->get<uint8_t, 4, 4>();
            CPPUNIT_ASSERT(b == 0x0);
            msg->setByte(0, 0x0);


            // full nibble, off 8
            msg->setByte(1, 0xF);
            b = msg->get<uint8_t, 8, 4>();
            CPPUNIT_ASSERT(b == 0xF);
            b = msg->get<uint8_t, 12, 4>();
            CPPUNIT_ASSERT(b == 0x0);
            msg->setByte(1, 0x0);

            // full nibble, off 6
            msg->setByte(0, 0b11000000);
            msg->setByte(1, 0b00000011);
            b = msg->get<uint8_t, 0, 6>();
            CPPUNIT_ASSERT(b == 0x0);
            b = msg->get<uint8_t, 10, 6>();
            CPPUNIT_ASSERT(b == 0x0);
            b = msg->get<uint8_t, 6, 4>();
            CPPUNIT_ASSERT(b == 0xF);
            msg->setByte(1, 0x0);
            msg->setByte(0, 0x0);

            // full byte, off 0
            msg->setByte(0, 0xFF);
            b = msg->get<uint8_t, 0, 8>();
            CPPUNIT_ASSERT(b == 0xFF);
            b = msg->get<uint8_t, 8, 8>();
            CPPUNIT_ASSERT(b == 0x0);
            msg->setByte(0, 0x0);


            // full byte, off 4
            msg->setByte(0, 0b11110000);
            msg->setByte(1, 0b00001111);
            b = msg->get<uint8_t, 0, 4>();
            CPPUNIT_ASSERT(b == 0x0);
            b = msg->get<uint8_t, 12, 4>();
            CPPUNIT_ASSERT(b == 0x0);
            b = msg->get<uint8_t, 4, 8>();
            CPPUNIT_ASSERT(b == 0xFF);
            msg->setByte(1, 0x0);
            msg->setByte(0, 0x0);


            // 6bits, off 6
            msg->setByte(0, 0b11000000);
            msg->setByte(1, 0b00001001);
            b = msg->get<uint8_t, 0, 6>();
            CPPUNIT_ASSERT(b == 0x0);
            b = msg->get<uint8_t, 12, 4>();
            CPPUNIT_ASSERT(b == 0x0);
            b = msg->get<uint8_t, 6, 6>();
            CPPUNIT_ASSERT(b == 0b100111);
            msg->setByte(1, 0x0);
            msg->setByte(0, 0x0);
        }

        void testSet8bits() {
            msg->set<uint8_t, 0, 4>(0xF);
            CPPUNIT_ASSERT(msg->getByte(0) == 0xF); 
            msg->setByte(0, 0x0);

            msg->set<uint8_t, 4, 4>(0xF);
            CPPUNIT_ASSERT(msg->getByte(0) == (0xF<<4)); 
            msg->setByte(0, 0x0);

            msg->set<uint8_t, 2, 4>(0xF);
            CPPUNIT_ASSERT(msg->getByte(0) == (0xF<<2)); 
            msg->setByte(0, 0x0);

            msg->set<uint8_t, 6, 4>(0xF);
            CPPUNIT_ASSERT(msg->getByte(0) == (0x3<<6)); 
            CPPUNIT_ASSERT(msg->getByte(1) == (0x3)); 
            msg->setByte(0, 0x0);
        }

        void testGet16bits() {
            uint8_t b = msg->get<uint16_t, 0, 4>();


            CPPUNIT_ASSERT( true );
        }

        void testSet16bits() {
            msg->set<uint16_t, 0, 4>(1024);


            CPPUNIT_ASSERT( true );
        }

    private:
        tMessage<8> *msg;
};