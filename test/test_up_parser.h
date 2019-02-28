#ifndef TEST_UP_PARSER_H
#define TEST_UP_PARSER_H

#include <cppunit/TestCase.h>
#include <cppunit/TestFixture.h>

#include "microparcel.h"


class MicroParcelParserTest : public CppUnit::TestFixture { 
    CPPUNIT_TEST_SUITE(MicroParcelParserTest);
    CPPUNIT_TEST(testEncoding);
    CPPUNIT_TEST(testDecoding);
    CPPUNIT_TEST_SUITE_END();

    public:
        void setUp(){
        }

        void tearDown(){
        }

    protected:
        void testEncoding(){
            microparcel::Message<4> src_msg;
            src_msg.data[0] = 0;
            src_msg.data[1] = 1;
            src_msg.data[2] = 2;
            src_msg.data[3] = 3;

            microparcel::Frame<4> frame = microparcel::Parser<4>::encode(src_msg);

            CPPUNIT_ASSERT(frame.SOF == microparcel::Frame<4>::kSOF);
            CPPUNIT_ASSERT(frame.message.data[0] == 0);
            CPPUNIT_ASSERT(frame.message.data[1] == 1);
            CPPUNIT_ASSERT(frame.message.data[2] == 2);
            CPPUNIT_ASSERT(frame.message.data[3] == 3);
            CPPUNIT_ASSERT(frame.checksum == (microparcel::Frame<4>::kSOF + 0 + 1 + 2 + 3));
        }

        void testDecoding(){
            microparcel::Parser<4> parser;
            microparcel::Message<4> msg;

            // valid message
            CPPUNIT_ASSERT(parser.parse(0xAA, &msg) == microparcel::Parser<4>::notcomplete);
            CPPUNIT_ASSERT(parser.parse(0, &msg) == microparcel::Parser<4>::notcomplete);
            CPPUNIT_ASSERT(parser.parse(1, &msg) == microparcel::Parser<4>::notcomplete);
            CPPUNIT_ASSERT(parser.parse(2, &msg) == microparcel::Parser<4>::notcomplete);
            CPPUNIT_ASSERT(parser.parse(3, &msg) == microparcel::Parser<4>::notcomplete);
            CPPUNIT_ASSERT(parser.parse(0xFF & (0xAA+0+1+2+3), &msg) == microparcel::Parser<4>::complete);

            CPPUNIT_ASSERT(msg.data[0] == 0);
            CPPUNIT_ASSERT(msg.data[1] == 1);
            CPPUNIT_ASSERT(msg.data[2] == 2);
            CPPUNIT_ASSERT(msg.data[3] == 3);

            // invalid CS
            CPPUNIT_ASSERT(parser.parse(0xAA, &msg) == microparcel::Parser<4>::notcomplete);
            CPPUNIT_ASSERT(parser.parse(0, &msg) == microparcel::Parser<4>::notcomplete);
            CPPUNIT_ASSERT(parser.parse(1, &msg) == microparcel::Parser<4>::notcomplete);
            CPPUNIT_ASSERT(parser.parse(2, &msg) == microparcel::Parser<4>::notcomplete);
            CPPUNIT_ASSERT(parser.parse(255, &msg) == microparcel::Parser<4>::notcomplete);
            CPPUNIT_ASSERT(parser.parse(0xFF & (0xAA+0+1+2+3), &msg) == microparcel::Parser<4>::error);

            // Invalid SOF
            CPPUNIT_ASSERT(parser.parse(0xA8, &msg) == microparcel::Parser<4>::error);
            CPPUNIT_ASSERT(parser.parse(0xA9, &msg) == microparcel::Parser<4>::error);
            CPPUNIT_ASSERT(parser.parse(0xAA, &msg) == microparcel::Parser<4>::notcomplete);
            CPPUNIT_ASSERT(parser.parse(8, &msg) == microparcel::Parser<4>::notcomplete);
            CPPUNIT_ASSERT(parser.parse(9, &msg) == microparcel::Parser<4>::notcomplete);
            CPPUNIT_ASSERT(parser.parse(10, &msg) == microparcel::Parser<4>::notcomplete);
            CPPUNIT_ASSERT(parser.parse(11, &msg) == microparcel::Parser<4>::notcomplete);
            CPPUNIT_ASSERT(parser.parse(0xFF & (0xAA+8+9+10+11), &msg) == microparcel::Parser<4>::complete);
        }
};


#endif //TEST_UP_PARSER_H