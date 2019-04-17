#pragma once

#include <boost/preprocessor/seq/transform.hpp>
#include <boost/preprocessor/seq/enum.hpp>
#include <boost/preprocessor/stringize.hpp>
#include <boost/preprocessor/cat.hpp>

//////////////////////////////////////////////////////////////////////////
#define EU_OP_COMBINE_REAL_ENUM(r, _enum_prefix_, val)  BOOST_PP_CAT(_enum_prefix_, val)
#define EU_OP_ENUM_STRINGIZE(r, _enum_prefix_, val)     BOOST_PP_STRINGIZE(val)

// get end of enum values
#define ENUM_END_NAME(_enum_prefix_, _enum_type_name_) BOOST_PP_CAT(_enum_prefix_, BOOST_PP_CAT(_enum_type_name_, _ENUM_END))

// define an enum, without declaring 2 functions.
#define DEFINE_ENUM_NO_FUNC(_enum_type_name_, _enum_prefix_, _val_seq_) \
    typedef enum { \
        BOOST_PP_SEQ_ENUM(BOOST_PP_SEQ_TRANSFORM(EU_OP_COMBINE_REAL_ENUM, _enum_prefix_, _val_seq_)), \
        ENUM_END_NAME(_enum_prefix_, _enum_type_name_) \
    } _enum_type_name_;

// define an enum, declare 3 functions: XXX_get_name, XXX_get_count, XXX_last_enum
#define DEFINE_ENUM(_enum_type_name_, _enum_prefix_, _val_seq_) \
    DEFINE_ENUM_NO_FUNC(_enum_type_name_, _enum_prefix_, _val_seq_) \
    static const char* BOOST_PP_CAT(_enum_type_name_, _get_name)(int enum_val) { \
        static const char* const str[] = { BOOST_PP_SEQ_ENUM(BOOST_PP_SEQ_TRANSFORM(EU_OP_ENUM_STRINGIZE, , _val_seq_)) }; \
        return (enum_val) < ENUM_END_NAME(_enum_prefix_, _enum_type_name_) ? str[enum_val] : "(unknown)"; \
    } \
    static size_t BOOST_PP_CAT(_enum_type_name_, _get_count)() { return ENUM_END_NAME(_enum_prefix_, _enum_type_name_); } \
    static int BOOST_PP_CAT(_enum_type_name_, _last_enum)() { return ENUM_END_NAME(_enum_prefix_, _enum_type_name_) - 1; }

//////////////////////////////////////////////////////////////////////////
#define EU_SELECT_1ST(first, second)     first
#define EU_SELECT_2ND(first, second)     second
#define EU_EXTRACT_1ST(elem)  EU_SELECT_1ST elem
#define EU_EXTRACT_2ND(elem)  EU_SELECT_2ND elem

#define EU_OP_MAKE_ENUM(_unused_, _enum_prefix_, _elem_) \
    BOOST_PP_CAT(_enum_prefix_, EU_EXTRACT_1ST(_elem_))
    
#define EU_OP_MAKE_DATA(_unused_, _unused_2_, _elem_) \
    EU_EXTRACT_2ND(_elem_)

#define DEFINE_ARRAY_BY_ENUM(_array_name_, _enum_type_name_, _enum_prefix_, _enum_and_data_seq_) \
    typedef enum { \
        BOOST_PP_SEQ_ENUM( BOOST_PP_SEQ_TRANSFORM( EU_OP_MAKE_ENUM, _enum_prefix_, _enum_and_data_seq_)), \
        BOOST_PP_CAT(_enum_prefix_, BOOST_PP_CAT( _enum_type_name_, _ENUM_END) ) \
    } _enum_type_name_; \
    _array_name_ = { \
        BOOST_PP_SEQ_ENUM( BOOST_PP_SEQ_TRANSFORM( EU_OP_MAKE_DATA, , _enum_and_data_seq_)) \
    }
