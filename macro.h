#pragma once

#include <iostream>
#include <memory.h> // memset()  -- not bzero() because it's not support in all platform.
#include "bit_util.h"
#include "str_util.h"
#include <sstream>
extern "C" {
#include <stddef.h>
};

// constructor of the C-style struct
#define C_STRUCT_CONSTRUCTOR(_class_name)  \
    _class_name(){memset(this, 0, sizeof(_class_name));}
//////////////////////////////////////////////////////////////////////////

// get the length of array which is in static memory.
// e.g.
//   Type var[N];
//   STATIC_ARRAY_LEN(var);  -- return N
#define STATIC_ARRAY_LEN(_arr) \
    sizeof(_arr)/sizeof(_arr[0])

#define STATIC_ARRAY_B_ZERO(_arr) \
    memset(_arr, 0, sizeof(_arr))

// avoid warning by use offsetof() in <stddef.h>
#define MY_OFFSETOF(s,m)   (((size_t)&(((s *)1)->m))-1)

#define ZERO_NUM_OF_4BYTE_ALIGN(_data_length)  (((_data_length)%4)  ? (4-(_data_length)%4): 0)
#define LENGTH_OF_4BYTE_ALIGN(_data_length)      (_data_length + ZERO_NUM_OF_4BYTE_ALIGN(_data_length))

//////////////////////////////////////////////////////////////////////////
// simple output to ostream
#define OS_OUT(_os_, _v) \
    (_os_) << #_v << " : " << (_v) << std::endl

#define OS_OUT_MEMBER_NAME_VALUE_FIRST(_os_, _member_name_, _value_)  \
    (_os_) << "/- "  #_member_name_ " : " << _value_ << std::endl
#define OS_OUT_MEMBER_FIRST(_os_, _member_name_) \
    OS_OUT_MEMBER_NAME_VALUE_FIRST(_os_, _member_name_, _member_name_)

#define OS_OUT_MEMBER_NAME_VALUE_LAST(_os_, _member_name_, _value_) \
    (_os_) << "\\- "  #_member_name_ " : " << _value_ << std::endl
#define OS_OUT_MEMBER_LAST(_os_, _member_name_) \
    OS_OUT_MEMBER_NAME_VALUE_LAST(_os_, _member_name_, _member_name_)

#define OS_OUT_MEMBER_NAME_VALUE(_os_, _member_name_, _value_)  \
    (_os_) << "|- "  #_member_name_ " : " << _value_ << std::endl
#define OS_OUT_MEMBER(_os_, _member_name_)  \
    OS_OUT_MEMBER_NAME_VALUE(_os_, _member_name_, _member_name_)
#define OS_OUT_MEMBER_NAME(_os_, _member_name_)  \
    (_os_) << "|- "  #_member_name_ " : " << std::endl

#define OS_OUT_MEMBER_DIFF(_os_, _member_name_, _other_obj_) \
    _os_ << "|- " #_member_name_ " : " << (_member_name_); \
    if (_other_obj_._member_name_ != _member_name_) { \
        os << " <=> " << _other_obj_._member_name_; \
    } \
    _os_ << std::endl

//////////////////////////////////////////////////////////////////////////
#define DECLARE_STD_OS_OPERATOR_LL(_class_name_) \
    std::ostream& operator<<(std::ostream& os, const _class_name_& const_ref)

#define DEFINE_STD_OS_OPERATOR_LL(_class_name_) \
    std::ostream& operator<<(std::ostream& os, const _class_name_& const_ref) { \
        const_ref.operator<<(os); \
        return os; \
    }

#define DECLARE_STD_OS_OPERATOR_LL_MEMBER() \
    std::ostream& operator<<(std::ostream& os) const

#define DEFINE_STD_OS_OPERATOR_LL_MEMBER(_class_name_) \
    std::ostream& _class_name_::operator<<(std::ostream& os) const

#define DECLARE_STD_OSTREAM_SUPPORT(_class_name_) \
    DECLARE_STD_OS_OPERATOR_LL_MEMBER()

#define DEFINE_STD_OSTREAM_SUPPORT(_class_name_) \
    DEFINE_STD_OS_OPERATOR_LL(_class_name_) \
    DEFINE_STD_OS_OPERATOR_LL_MEMBER(_class_name_)
//////////////////////////////////////////////////////////////////////////
#define FL()  "[" <<  __FILE__ << ":" << __PRETTY_FUNCTION__ << ":" << __LINE__ << "]"
#define FL_STR() std::string("[") + __FILE__ + ":" + StrUtil::IntToString(__LINE__)  + "]"

#define COUT(_v) \
    OS_OUT(std::cout, _v)

#define OUTPUT_TEXT(_text) \
	std::cout << FL() << (_text) << std::endl

#define OUTPUT(_v) \
	std::cout << FL() << #_v << " : " << (_v) << std::endl

#define QUICK_ERROR_OUTPUT() \
    std::cout << "E. " <<  FL() << std::endl

#define QUICK_DEBUG() \
    OUTPUT_TEXT("")

#define SEE(_var)   \
    COUT(_var); \
    BitUtil::HexDump((char*)&(_var), sizeof((_var)), cout)

#define SEE_ADDRESS_32_PTR(_obj_ptr) \
    printf("pointer " #_obj_ptr " : 0x%08X\n", (unsigned int)(void*)(_obj_ptr))

#define SEE_ADDRESS_32(_obj)    SEE_ADDRESS_32_PTR(&_obj)

#define SEE_ADDRESS_64_PTR(_obj_ptr) \
    std::cout << "pointer " #_obj_ptr " : 0x" << std::oct << (long long)(void*)(_obj_ptr) << std::dec << "\n"

#define SEE_ADDRESS_64(_obj)    SEE_ADDRESS_64_PTR(&_obj)

#define TRACE() std::cout << FL() << std::endl
	
#define COUT_STR_VALUE(_str_, _value_)  \
		std::cout << _str_ <<  " : " << _value_ << std::endl

#define COUT_MEMBER(_member_name_)  \
    OS_OUT_MEMBER(std::cout, _member_name_)
#define COUT_MEMBER_NAME_VALUE(_member_name_, _value_)  \
    OS_OUT_MEMBER_NAME_VALUE(std::cout, _member_name_, _value_)
#define COUT_MEMBER_NAME(_member_name_)  \
    OS_OUT_MEMBER_NAME(std::cout, _member_name_)

//////////////////////////////////////////////////////////////////////////
// check the return value of C++ statement calling.
#include "special_print.h"

#define CALLING_OK()        std::cout << "[ " << text_format_fg("OK", FG_GREEN) << " ]" << END_COLOR
#define CALLING_FAILED()    std::cout << "[ " << text_format_fg("FAILED", FG_RED) << " ]" << END_COLOR

#define CALLING_CHECK(_call_stm_, _suc_return_val_, _faild_retval_) \
    do { \
        std::cout << START_YELLOW << __FILE__ << ":" << __LINE__ << END_COLOR << " : " << #_call_stm_ << std::endl; \
        if ((_suc_return_val_) != (_call_stm_)) { \
            CALLING_FAILED() << std::endl; \
            return _faild_retval_; \
        } else { \
            CALLING_OK() << " return value: " << _suc_return_val_ << std::endl; \
        } \
    } while(0)

//////////////////////////////////////////////////////////////////////////
template<typename T>
std::string FormatString(T v)
{
    std::ostringstream tmp;
    tmp<<v;
    return tmp.str();
}

// return char* of objecet, which typed by std::string
#define GET_STL_STRING_TO_CHAR_PTR(_str_) (&*(_str).begin())
//////////////////////////////////////////////////////////////////////////
// use some boost character.
#if !defined (BOOST_PP_CAT)
// It works at centos 5.2.
#    define BOOST_PP_CAT(a, b) BOOST_PP_CAT_I(a, b)
#    define BOOST_PP_CAT_I(a, b) BOOST_PP_CAT_II(a ## b)
#    define BOOST_PP_CAT_II(res) res
#else
#   include <boost/preprocessor/cat.hpp>
#endif

//////////////////////////////////////////////////////////////////////////
#define SEC_PER_DAY 86400   // =60*60*24
#define USEC_PER_SEC 1000000// =1000*1000

//////////////////////////////////////////////////////////////////////////
#define UNUSED(x) do{ if ((x)==(x)) ; } while(false)
#define UNUSED_PARAM(p)    ((void)p)

//////////////////////////////////////////////////////////////////////////
#define FUNC_BEGIN() \
    CALLING_TRACE("In.")

#define CALLING_TRACE(_msg_) \
    std::cout << __FILE__ << ":" << __FUNCTION__ << "():" << __LINE__ << ":" << (_msg_) << std::endl

#define RETURN \
    CALLING_TRACE("Out."); \
    return

//////////////////////////////////////////////////////////////////////////
//
#ifndef ULL
#define ULL(A) A ## ULL
#endif

#ifndef LL
#define LL(A) A ## LL
#endif
