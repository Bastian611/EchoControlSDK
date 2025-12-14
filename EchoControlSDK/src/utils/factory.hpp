
#pragma once
#include <map>
#include "../global.h"

#ifndef SUPPORT_C11
#include "create_map.hpp"
#else
#include <unordered_map>
#endif

ECCS_BEGIN


//--------------------------------------------------
// 以下宏用于动态创建对象，不需要在运行时注册，使用方法如下：
//
// 1. 注册类型(使用全局静态 std::map 存储KEY、类型名称和创建方法)
//    FACTORY_BEGIN(int, Parent)
//    FACTORY_APPEND(1, Child1, Parent)
//    FACTORY_APPEND(2, Child2, Parent)
//    FACTORY_END(int, Parent)
//    * FACTORY_BEGIN, FACTORY_APPEND, FACTORY_END应该放置在 .cpp 文件中
//    * FACTORY_DECL 是工厂函数申明，应该放置在 .h 文件中
//
// 2. 类型定义有两种方法，建议使用方法二
//    方法一：
//      class Parent { public: FACTORY_BASE() };
//      class Child1 : public Parent { public: FACTORY_CHILD(Parent, Child1) };
//      class Child2 : public Parent { public: FACTORY_CHILD(Parent, Child2) };
//      * 可以使用 FACTORY_CHILD_WITH_SPEC_NAME() 指定类型名称
//      * 此方法在也没有指定类型ID以及类型ID的类型，不能通过对象获取类型ID
//      * 此方法对于 通过模板定义子类 的情形，不能方便的指定类型名称
//    方法二：
//      class Parent { public: FACTORY_ID_BASE(int) };
//      class Child1 : public Parent { public: FACTORY_ID_CHILD(int, 1, Parent, Child1) };
//      class Child2 : public Parent { public: FACTORY_ID_CHILD(int, 2, Parent, Child2) };
//      * 可以使用 FACTORY_ID_CHILD_WITH_SPEC_NAME() 指定类型名称
//      * 此方法可以直接在定义子类时就指定了ID，通过对象直接获取类型ID（静态成员和虚函数）
//        因此可以使用 FACTORY_ID_APPEND(TChild, Parent) 方便的添加类型
//      * 对于 通过模板定义子类 的情形，可以使用 FACTORY_ID_CHILD_WITH_MAP_NAME()，
//        此时需要先声明 FACTORY_DECL_EXTERN()
//
// 3. 使用工厂创建对象
//    auto* c1 = FACTORY_CREATE(1, Parent);
//    auto* c2 = FACTORY_CREATE(2, Parent);
//
//--------------------------------------------------

#define FACTORY_DECL(TKey, TBase) \
    TBase* _##TBase##_Create_(TKey); \
    const char* _##TBase##_TypeName_(TKey key);

#define FACTORY_DECL_EXTERN(TKey, TBase) \
    extern TBase* _##TBase##_Create_(TKey); \
    extern const char* _##TBase##_TypeName_(TKey key);

#define FACTORY_CREATE(key, TBase) \
    _##TBase##_Create_(key)

#define FACTORY_TYPENAME(key, TBase) \
    _##TBase##_TypeName_(key)

#define _FACTORY_IMPL(TKey, TBase) \
    TBase* _##TBase##_Create_(TKey key) \
    { \
        auto it = _##TBase##_MAP_.find(key); \
        if (it != _##TBase##_MAP_.end()) { \
            return it->second.Creator(); \
        }\
        return NULL; \
    } \
    const char* _##TBase##_TypeName_(TKey key) \
    { \
        auto it = _##TBase##_MAP_.find(key); \
        if (it != _##TBase##_MAP_.end()) { \
            return it->second.TypeName; \
        } \
        return NULL; \
    }

#ifdef SUPPORT_C11

#define FACTORY_BEGIN(TKey, TBase) \
    namespace { \
    typedef TBase* (*_##TBase##_CreateInstance_)();\
    struct _##TBase##_CreatorInfo_\
    {\
        const char* TypeName; \
        _##TBase##_CreateInstance_ Creator;\
    };\
    static std::unordered_map<TKey, _##TBase##_CreatorInfo_> _##TBase##_MAP_ = {

#define FACTORY_APPEND(key, TChild, TBase) \
    { key,  {#TChild, &(TChild::createInstance)} },

#define FACTORY_END(TKey, TBase) \
    };} \
    _FACTORY_IMPL(TKey, TBase)

#else

#define FACTORY_BEGIN(TKey, TBase) \
    namespace { \
    typedef TBase* (*_##TBase##_CreateInstance_)();\
    struct _##TBase##_CreatorInfo_\
    {\
        const char* TypeName; \
        _##TBase##_CreateInstance_ Creator;\
        \
        _##TBase##_CreatorInfo_() : TypeName(NULL), Creator(NULL) {} \
        _##TBase##_CreatorInfo_(const char* typeName, _##TBase##_CreateInstance_ creator) \
            : TypeName(typeName), Creator(creator) {} \
    };\
    static std::map<TKey, _##TBase##_CreatorInfo_> _##TBase##_MAP_ = CreateMap<TKey, _##TBase##_CreatorInfo_>

#define FACTORY_APPEND(key, TChild, TBase) \
    ( key,  _##TBase##_CreatorInfo_(#TChild, &(TChild::createInstance)) )

#define FACTORY_END(TKey, TBase) \
    ;} \
    _FACTORY_IMPL(TKey, TBase)

#endif


#define FACTORY_BASE() \
    virtual const char* typeName() const = 0;

#define FACTORY_CHILD_WITH_SPEC_NAME(TBase, TChild, TypeName) \
    static TBase* createInstance() { return new TChild(); } \
    virtual const char* typeName() const { return TypeName; }

#define FACTORY_CHILD(TBase, TChild) \
    FACTORY_CHILD_WITH_SPEC_NAME(TBase, TChild, #TChild)


#define FACTORY_ID_BASE(TKey) \
    virtual TKey typeId() const = 0; \
    FACTORY_BASE()

#define FACTORY_ID_CHILD_WITH_SPEC_NAME(TKey, key, TBase, TChild, TypeName) \
    const static TKey _FACTORY_ID_ = key; \
    virtual TKey typeId() const { return key; }; \
    FACTORY_CHILD_WITH_SPEC_NAME(TBase, TChild, TypeName)

#define FACTORY_ID_CHILD_WITH_MAP_NAME(TKey, key, TBase, TChild) \
    FACTORY_ID_CHILD_WITH_SPEC_NAME(TKey, key, TBase, TChild, FACTORY_TYPENAME(key, TBase))

#define FACTORY_ID_CHILD(TKey, key, TBase, TChild) \
    FACTORY_CHILD_WITH_SPEC_NAME(TBase, TChild, #TChild)

#define FACTORY_ID_APPEND(TChild, TBase) \
    FACTORY_APPEND(TChild::_FACTORY_ID_, TChild, TBase)


ECCS_END
