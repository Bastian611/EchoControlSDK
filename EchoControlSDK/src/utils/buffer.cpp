
#include "buffer.h"
#include <assert.h>
#include <string.h>
#include "../debug/exceptions.h"
#include "../debug/Logger.h"

ECCS_BEGIN


Buffer::Buffer()
{
    _capacity = 16;
    _buf = new char[_capacity];
    _pos = 0;
    _count = 0;
    //printf("Buffer()\n");
}
Buffer::Buffer(size_t s)
{
    _capacity = (s + 4 - s % 4);
    assert(_capacity > 1);
    _buf = new char[_capacity];
    _pos = 0;
    _count = 0;
}
Buffer::Buffer(const char* dat, size_t len)
{
    _capacity = (len + 4 - len % 4);
    assert(_capacity > 1);
    _buf = new char[_capacity];
    memcpy(_buf, dat, len);
    _pos = 0;
    _count = len;
}
Buffer::Buffer(const Buffer& other)
{
    _capacity = other._capacity;
    _pos = other._pos;
    _count = other._count;
    assert(_capacity > 1);
    _buf = new char[_capacity];
    memcpy(_buf, other._buf, _count);
}
Buffer& Buffer::operator=(const Buffer& other)
{
    if(this != &other){
        _capacity = other._capacity;
        _pos = other._pos;
        _count = other._count;
        delete[] _buf;
        assert(_capacity > 1);
        _buf = new char[_capacity];
        memcpy(_buf, other._buf, _count);
    }
    return (*this);
}
Buffer::~Buffer()
{
    _count = 0;
    delete[] _buf;
    _buf = NULL;
    //printf("~Buffer()\n");
}

void Buffer::clear()
{
    _count = 0;
    _pos = 0;
}
void Buffer::recapacity(size_t s)
{
    if (s <= 1) {
        return;
    }

    size_t cap = (s + 4 - s % 4);
    char* newBuf = new char[cap];

    size_t cnt = s > _count ? _count : s;
    memcpy((void*)newBuf, (void*)(&_buf[_pos]), cnt);
    delete[] _buf;

    _buf = newBuf;
    _pos = 0;
    _count = cnt;
    _capacity = cap;
}
void Buffer::reserve(size_t cnt)
{
    size_t front = _pos;
    size_t behind = _capacity - _pos - _count;
    if (behind < cnt)
    {
        if (front + behind >= cnt)
        {
            /* Now front > 0 */
            if (_count > 0)
            {
                if (front >= _count) {
                    memcpy(_buf, _buf+_pos, _count);
                }
                else {
                    recapacity(cnt + _count*2);
                }
            }
            _pos = 0;
        }
        else {
            recapacity(cnt + _count*2);
        }
    }
}
void Buffer::resize(size_t s)
{
    if (_count < s) {
        size_t cnt = s - _count;
        reserve(cnt);
    }
    _count = s;
}
void Buffer::swap(Buffer& other)
{
    if(this != &other){
        auto tmpCapacity = _capacity;
        auto tmpPos = _pos;
        auto tmpCount = _count;
        auto tmpBuf = _buf;

        _capacity = other._capacity;
        _pos = other._pos;
        _count = other._count;
        _buf = other._buf;

        other._capacity = tmpCapacity;
        other._pos = tmpPos;
        other._count = tmpCount;
        other._buf = tmpBuf;
    }
}

size_t Buffer::capacity() const
{
    return _capacity;
}
size_t Buffer::size() const
{
    return _count;
}
char& Buffer::front()
{
    if (_count == 0) {
        throw EOutOfBoundary("Buffer::front()");
    }
    return _buf[_pos];
}
char& Buffer::back()
{
    if (_count == 0) {
        throw EOutOfBoundary("Buffer::back()");
    }
    return _buf[_pos + _count - 1];
}
const char& Buffer::front() const
{
    return ((Buffer*)this)->front();
}
const char& Buffer::back() const
{
    return ((Buffer*)this)->back();
}
char& Buffer::operator[](size_t idx)
{
    if (_count>0 && idx<_count){
        return (*(_buf + _pos + idx));
    }
    throw EOutOfBoundary("Buffer::operator[]()", idx, _count);
}
const char& Buffer::operator[](size_t idx) const
{
    if (_count>0 && idx<_count){
        return (*(_buf + _pos + idx));
    }
    throw EOutOfBoundary("Buffer::operator[]()", idx, _count);
}
char* Buffer::buf()
{
    return _count != 0 ? &(_buf[_pos]) : NULL;
}
char* Buffer::buf() const
{
    return _count != 0 ? &(_buf[_pos]) : NULL;
}

char Buffer::pop_front()
{
    char b = 0;

    if (_count > 1) {
        b = _buf[_pos];
        ++_pos;
        --_count;
    }
    else if (_count == 1) {
        b = _buf[_pos];
        clear();
    }
    else {
        throw EOutOfBoundary("Buffer::pop_front()");
    }

    return b;
}
char Buffer::pop_back()
{
    char b = 0;

    if (_count > 1) {
        b = _buf[_pos + _count - 1];
        --_count;
    }
    else if (_count == 1) {
        b = _buf[_pos];
        clear();
    }
    else{
        throw EOutOfBoundary("Buffer::pop_back()");
    }

    return b;
}
void Buffer::pop_front(size_t n)
{
    if (_count > n){
        _pos += n;
        _count -= n;
    }
    else if (_count == n) {
        clear();
    }
    else{
        throw EOutOfBoundary("Buffer::pop_front()");
    }
}
void Buffer::pop_back(size_t n)
{
    if (_count >= n){
        _count -= n;
    }
    else if (_count == n) {
        clear();
    }
    else{
        throw EOutOfBoundary("Buffer::pop_back()");
    }
}
void Buffer::push_back(const char b)
{
    reserve(1);
    _buf[_pos + _count] = b;
    _count += 1;
}
void Buffer::push_back(const char* bs, size_t count)
{
    reserve(count);
    memcpy((void*)(&_buf[_pos + _count]), (void*)bs, count);
    _count += count;
}


ECCS_END
