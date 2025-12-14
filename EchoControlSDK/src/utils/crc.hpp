
#pragma once
#include "../global.h"

ECCS_BEGIN


//------------------------------------------------------
// T: uint8_t, uint16_t, uint32_t
//------------------------------------------------------
template<typename T, T Key>
class CRC
{
public:
    CRC(T init = 0, T xorOut = 0, bool refIn = false, bool refOut = false)
        : _key(Key), _register(init), _init(init), _xorOut(xorOut), _refIn(refIn), _refOut(refOut)
    {

    }

    // 方法一： 使用putByte/putBuf输入数据，使用done获取CRC校验结果
    T done()
    {
        T tmp = _refOut ? reflect(_register, sizeof(_register)*8) : _register;
        _register = _init;
        return tmp^_xorOut;
    }
    void putByte(uint8_t byte)
    {
        uint8_t refByte = _refIn ? reflect(byte, sizeof(uint8_t)*8) : byte;
        unsigned top = _register >> (sizeof(T) - sizeof(uint8_t))*8;
        top ^= refByte;
        _register = (_register << 8) ^ _table[top];
    }
    void putBuf(uint8_t* pBytes, size_t len)
    {
        for(size_t i = 0; i < len; i++){
            putByte(*(pBytes+i));
        }
    }

    // 方法二： 直接使用calc获取CRC校验结果
    T calc(uint8_t* pBytes, size_t len)
    {
        for(size_t i = 0; i < len; i++){
            putByte(*(pBytes+i));
        }
        return done();
    }

protected:
    static T reflect(T data, uint8_t nBits)
    {
        T  reflection = 0;
        uint8_t  bit;

        for (bit = 0; bit < nBits; ++bit)
        {
            if (data & 0x01){
                reflection |= (1 << ((nBits - 1) - bit));
            }
            data = (data >> 1);
        }

        return (reflection);
    }

    class Table
    {
    public:
        Table()
        {
            // for all possible byte values
            for (unsigned i = 0; i < 256; ++i)
            {
                T reg = i << (sizeof(T) - sizeof(uint8_t))*8;
                // for all bits in a byte
                for (int j = 0; j < 8; ++j)
                {
                    bool topBit = (reg & (1 << (sizeof(T)*8-1))) != 0;
                    reg <<= 1;
                    if (topBit){
                        reg ^= Key;
                    }
                }
                _table[i] = reg;
            }
        }
        T operator[](unsigned i) const
        {
            return _table[i];
        }

    private:
        T _table[256];
    };

private:
    const static Table _table;

    T _key;
    T _register;

    T _init;
    T _xorOut;

    bool _refIn;
    bool _refOut;
};

template<typename T, T Key>
const typename CRC<T, Key>::Table CRC<T, Key>::_table;


ECCS_END
