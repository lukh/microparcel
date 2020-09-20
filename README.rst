===========
microparcel
===========

.. image:: https://travis-ci.org/lukh/microparcel.svg?branch=master
    :target: https://travis-ci.org/lukh/microparcel

Serialize and deserialize structured data.

Designed for use on bare-metal embedded systems (no dynamic allocation, template based for optimisation),
And provides implementation in Python. (see [microparcel-python](https://github.com/lukh/microparcel-python))


* Free software: MIT license
* Documentation: https://microparcel.readthedocs.io.


Provide three differents entities.

Message
-------

The Message is the Payload, transmitted via a serial line (UART, I2C), byte by byte.

The Message object holds a Data buffer (an array of uint8_t), the size is defined by a class template parameter.
And provides methods to access a specific part of the message (a specific bitfield in the message):
It uses offset, in bits, and a bitmask

.. code-block:: cpp

    #include <microparcel/microparcel.h>

    int main(){
        // creates a message with a 8 bytes payload
        microparcel::Message<8> msg = microparcel::Message<8>;

        // set the 5th, 6th, 7th bits of the payload (5,6,7 of the first byte) at the value "2"
        msg.set<uint8_t, 5, 3>(2);

        // set the 13th, 14th, 15th bits of the payload (5,6,7 of the second byte) at the value "3"
        msg.set<uint8_t, 13, 3>(3);

        // set the 6th, 7th, 8th, 9th bits of the payload; eg:
        // bits 6 and 7 of the first byte, bits 0 and 1 of the second
        // at the value "1"
        msg.set<uint8_t, 6, 4>(1);


        // for bisize higher that 8 (one byte), the offset must be aligned on a byte
        // bitsize is limited to 16; and the rettype should be change to uint16_t
        msg.set<uint16_t, 24, 16>(0xFFAF);


        // getter works in the same way:
        msg.get<uint8_t, 5, 3>();
        msg.get<uint16_t, 24, 16>();

    }

Frame
-----

A Frame encapsulate the Message between a StartOfFrame (SOF) and a CheckSum.

The SOF is an arbitrary value (in our case, 0xAA),
and the CheckSum is the sum of all bytes, including the SOF, truncated to 8bits.

It allows a lighweight and fast data integrity validation.

Parser
------

The Parser takes bytes, and builds up a Message from the data stream.

.. code-block:: cpp

    #include <microparcel/microparcel.h>

    // a way to get data from a Serial Line (UART ?)
    uint8_t getByteFromDataLine();
    bool isDataLineEmpty();

    int main(){
        // a Parser for Message with a Payload of 6.
        using TParser = microparcel::Parser<6>;

        TParser parser;
        TParser::Message_T msg;
        TParser::Status status;

        // main loop of embedded application
        while(true){
            // continue till the fifo is empty
            while(!isDataLineEmpty()){
                uint8_t byte = getByteFromDataLine();
                status = parser.parse(byte, &msg);
                switch(status){
                    // not complete and error could be treated differently...
                    // error means mainly that the checksum is not valid; transmission failed.
                    case TParser::eNotComplete:
                    case TParser::eError:
                        break;

                    case TParser::eComplete:
                        // msg is complete, handle it
                        // HANDLE_MSG(msg);
                        break;
                }
            }

        }
    }


The Parser also encodes Message into Frames for sending data

.. code-block:: cpp

    #include <microparcel/microparcel.h>

    // prototype to send data
    void send(uint8_t *data, uint8_t datasize);

    int main(){
        // a Parser for Message with a Payload of 6.
        using TParser = microparcel::Parser<6>;

        TParser::Message_T msg;

        // fill the message
        msg.set<uint8_t, 4, 8>(60);
        msg.set<uint8_t, 0, 4>(0xC);
        //...

        // builds the frame, with SOF and checksum
        TParser::Frame_T frame = TParser.encode(msg);

        // send over physical layer of choice
        send((uint8_t*)&inFrame, TFrame::FrameSize);

    }





