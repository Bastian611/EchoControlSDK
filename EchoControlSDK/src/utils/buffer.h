
#pragma once
#include <stddef.h>
#include "../global.h"

ECCS_BEGIN


class Buffer
{
public:
    Buffer();
    Buffer(size_t s);
    Buffer(const char* dat, size_t len);
    Buffer(const Buffer& other);
    Buffer& operator=(const Buffer& other);
    virtual ~Buffer();

    void clear();
    void recapacity(size_t s);
    void reserve(size_t cnt);
    void resize(size_t s);
    void swap(Buffer& other);

    size_t capacity() const;
    size_t size() const;
    char& front();// 返回队首数据
    char& back();// 返回队尾数据
    const char& front() const;
    const char& back() const;
    char& operator[](size_t idx);
    const char& operator[](size_t idx) const;
    char* buf();
    char* buf() const;

    char pop_front();// 弹出队首数据并删除
    char pop_back();// 弹出队尾数据并删除
    void pop_front(size_t n);
    void pop_back(size_t n);

    void push_back(const char b);// 数据放入队尾
    void push_back(const char* bs, size_t count);

private:
    char*   _buf;
    size_t  _capacity;
    size_t  _pos;
    size_t  _count;
};


ECCS_END
