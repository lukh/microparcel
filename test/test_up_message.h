#include <cppunit/TestCase.h>
#include <cppunit/TestFixture.h>


class MicroParcelMessageTest : public CppUnit::TestFixture { 
    CPPUNIT_TEST_SUITE(MicroParcelMessageTest);
    CPPUNIT_TEST(testToto);
    CPPUNIT_TEST_SUITE_END();

    public:
        void setUp(){}
        void tearDown(){}
    protected: 
        void testToto() {
            CPPUNIT_ASSERT( false );
        }
};