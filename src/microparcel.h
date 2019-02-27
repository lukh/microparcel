#ifndef MICROPARCEL_H
#define MICROPARCEL_H

#include <cstdint>


namespace microparcel{
    template <uint8_t Size>
    class Message{
        public:
            template <typename T, uint8_t Offset, uint8_t Bitsize>
            T get(){

            };

            template <typename T, uint8_t Offset, uint8_t Bitsize>
            void set(T field){

            };

        protected:
            uint8_t data[Size];
    };

    template <uint8_t MsgSize>
    class Frame{
        public:
            static const uint8_t kSOF = 0xAA;
            static const uint8_t FrameSize = MsgSize + 2;

        protected:
            uint8_t SOF;
            Message<MsgSize> message;
            uint8_t checksum;

    };


    template <uint8_t MsgSize>
    class Parser{
        public:
            using Message_T = Message<MsgSize>;
            using Frame_T = Frame<MsgSize>;
            
            enum Status{
                complete = 0,
                notcomplete,
                error
            };

            Status parse(uint8_t in_byte, Message_T *out_msg);
            static Frame_T encode(const Message_T &in_msg);

        private:
            enum State{
                idle = 0,
                busy
            };

            State state;
            Status status;

            uint8_t buffer[Frame_T::FrameSize];
            uint8_t buff_ptr;
    };
};

#endif //MICROPARCEL_H
