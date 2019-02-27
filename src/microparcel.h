#ifndef MICROPARCEL_H
#define MICROPARCEL_H

#include <cstdint>
#include <cstring>
#include <limits>
#include <numeric>

namespace microparcel{
    /**
     * \brief a Message class that provides API to access specific fields of a data payload
     * \tparam Size: the Byte Size of the Message
     */
    template <uint8_t Size>
    class Message{
        public:
            /**
             * \brief returns bitfields of Bitsize located at Offset in a uint8_t data chunk
             * Can return field from 1 to 16 bits
             * \tparam T the return type
             * \tparam Offset the offset, in bits (from 0 to 8*Size)
             * \tparam Bitsize the bitsize of the returned field (defines the mask)
             * */
            template <typename T, uint8_t Offset, uint8_t Bitsize>
            T get(){
                //check consistency
                static_assert(std::numeric_limits<T>::digits <= 16, "Can't get data larger than uint16_t");
                static_assert(Bitsize <= 16, "Bit size is bigger than 16");
                static_assert(Bitsize > 0, "Bit size can't be zero");
                
                static_assert(std::numeric_limits<T>::digits >= Bitsize, "the return type can't handle Bitsize");
                static_assert((Offset + Bitsize) <= 8 * Size, "Bitsize+Offset is out of range");

                static_assert((Bitsize <= 8) or ((Bitsize > 8) and (Offset & 0x3) == 0), "Offset must be a multiple of 8 for >8 bit bitfield");

                // for <= 8bits:
                if(Bitsize <= 8){
                    // on one byte
                    if((Offset & 0x7) + Bitsize <= 8){
                        uint8_t mask = (1<<Bitsize) - 1;
                        uint8_t byte_idx = Offset >> 3;
                        uint8_t byte_shift = Offset & 0x7;
                        return (data[byte_idx] >> byte_shift) & mask;
                    }

                    else{
                        uint8_t mask = (1<<Bitsize) - 1;
                        uint8_t byte_idx = Offset >> 3;
                        uint8_t byte_shift = Offset & 0x7;
                        uint8_t mask_lsb = mask & ( (1 << (8 - byte_shift)) - 1);
                        uint8_t mask_msb = mask >> (8 - byte_shift);

                        uint8_t lsb_part = (data[byte_idx] >> byte_shift) & mask_lsb;
                        uint8_t msb_part = data[byte_idx+1] & mask_msb;

                        return lsb_part | (msb_part << (8 - byte_shift));
                    }
                }

                // for >8bits
                else{
                    uint16_t mask = (1<<Bitsize) - 1;
                    uint16_t byte_idx = Offset >> 3;

                    return (data[byte_idx] & (uint8_t)(mask&0xFF)) | ((data[byte_idx+1] & (uint8_t)(mask>>8)) << 8);
                }
            };

            template <typename T, uint8_t Offset, uint8_t Bitsize>
            void set(T field){
                //check consistency
                static_assert(std::numeric_limits<T>::digits <= 16, "Can't get data larger than uint16_t");
                static_assert(Bitsize <= 16, "Bit size is bigger than 16");
                static_assert(Bitsize > 0, "Bit size can't be zero");
                
                static_assert(std::numeric_limits<T>::digits >= Bitsize, "the return type can't handle Bitsize");
                static_assert((Offset + Bitsize) <= 8 * Size, "Bitsize+Offset is out of range");

                static_assert((Bitsize <= 8) or ((Bitsize > 8) and (Offset & 0x3) == 0), "Offset must be a multiple of 8 for >8 bit bitfield");

                // for <= 8bits:
                if(Bitsize <= 8){
                    // on one byte
                    if((Offset & 0x7) + Bitsize <= 8){
                        uint8_t mask = (1<<Bitsize) - 1;
                        uint8_t byte_idx = Offset >> 3;
                        uint8_t byte_shift = Offset & 0x7;
                        data[byte_idx] &= ~(mask << byte_shift);
                        data[byte_idx] |= (field & mask) << byte_shift;
                        return;
                    }

                    else{
                        uint8_t mask = (1<<Bitsize) - 1;
                        uint8_t byte_idx = Offset >> 3;
                        uint8_t lsb_byte_shift = Offset & 0x7;
                        uint8_t msb_byte_shift = 8 - lsb_byte_shift;
                        uint8_t mask_lsb = mask & ( (1 << msb_byte_shift) - 1);
                        uint8_t mask_msb = mask >> msb_byte_shift;

                        // lsb
                        data[byte_idx] &= ~( mask_lsb << lsb_byte_shift );
                        data[byte_idx] |= (field & mask_lsb) << lsb_byte_shift;
                        // msb
                        data[byte_idx+1] &= ~mask_msb;
                        data[byte_idx+1] |= (field >> msb_byte_shift) & mask_msb;
                        return; // lsb_part | (msb_part << (8 - byte_shift));
                    }
                }

                // for >8bits
                else{
                    uint16_t mask = (1<<Bitsize) - 1;
                    uint8_t byte_idx = Offset >> 3;

                    data[byte_idx] &= ~(mask & 0xFF);
                    data[byte_idx] |= (field & 0xFF);

                    data[byte_idx+1] &= ~(mask >> 8);
                    data[byte_idx+1] |= (field >> 8);
                }
            };

            uint8_t data[Size];
    };

    template <uint8_t MsgSize>
    class Frame{
        public:
            static const uint8_t kSOF = 0xAA;
            static const uint8_t FrameSize = MsgSize + 2;

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

            Parser(): state(idle), status(notcomplete){}


            Status parse(uint8_t in_byte, Message_T *out_msg){
                switch(state){
                    case idle:
                        // reset the state machine
                        status = notcomplete;
                        buff_ptr = 0;

                        if(in_byte == Frame_T::kSOF){
                            // first byte is valid;
                            state = busy;
                            buffer[buff_ptr++] = in_byte;
                        }

                        break;

                    case busy:
                        buffer[buff_ptr++] = in_byte;

                        // handle the last data byte
                        if(buff_ptr == Frame_T::FrameSize){
                            if(isCheckSumValid()){
                                status = complete;
                                std::memcpy(out_msg->data, buffer+1, MsgSize);
                            }

                            else{
                                status = error;
                            }

                            state = idle; //ready to retrieve new messages
                        }
                        /*else{
                            status = notcomplete;
                        }*/
                        break;
                }

                return status;
            }


            static Frame_T encode(const Message_T &in_msg){
                Frame_T frame;
                frame.SOF = Frame_T::kSOF;
                frame.message = in_msg;
                frame.checksum = std::accumulate(std::begin(in_msg.data), std::end(in_msg.data), Frame_T::kSOF);

                return frame;
            }

        protected:
            bool isCheckSumValid(){
                uint8_t cs = std::accumulate(std::begin(buffer), std::end(buffer)-1, 0);
                return cs == buffer[Frame_T::FrameSize-1];
            }

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
