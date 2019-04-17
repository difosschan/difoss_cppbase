//////////////////////////////////////////////////////////////////////////
// @File Name: fixed_length_class.h
// @author: ChenRuirong(difoss@163.com)
// @copyright: ChenRuirong personal
// @license: Apache Licence 2.0
// @Abbreviations:
//   1. PLF: Position and Length of Field
//
// @histroy:
//  Version   Date         Author           Modification
// ---------------------------------------------------------------------
// 1.01.001 | 2015-08-15 | difoss@163.com | Create.
//////////////////////////////////////////////////////////////////////////
#pragma once

#include <boost/preprocessor/seq/transform.hpp>
#include <boost/preprocessor/seq/enum.hpp>
#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/seq/for_each_i.hpp>
#include "enum_util.h"
#include "str_util.h"
#include "types.h"
#include <memory.h>
#include <stdio.h>

//////////////////////////////////////////////////////////////////////////
// expand SEQUENCE<TUPLE(3)>
#define OP_EXTRACT_NAME(d, data, _3_elem_tuple_)  BOOST_PP_TUPLE_ELEM(3, 0, _3_elem_tuple_)
#define OP_EXTRACT_POS( d, data, _3_elem_tuple_)  BOOST_PP_TUPLE_ELEM(3, 1, _3_elem_tuple_)
#define OP_EXTRACT_LEN( d, data, _3_elem_tuple_)  BOOST_PP_TUPLE_ELEM(3, 2, _3_elem_tuple_)

#define EXTRACT_NAME(_elem_)    OP_EXTRACT_NAME(,,_elem_)
#define EXTRACT_POS( _elem_)    OP_EXTRACT_POS(,,_elem_)
#define EXTRACT_LEN( _elem_)    OP_EXTRACT_LEN(,,_elem_)

// PLF_ARRAY: Array contains Position and Length of Field.
#define PLF_ARRAY_NAME PosLenOfField

#define PLF_ARR_IDX_T PLF_Index_t

#define EXTRACT_ENUM_SEQ(_seq_inc_3_tuple_) \
    BOOST_PP_SEQ_TRANSFORM( OP_EXTRACT_NAME, d, _seq_inc_3_tuple_)
    
// declare an anonymous enum type.
#define DECLARE_ENUM(_prefix_, _seq_) \
    public: \
        DEFINE_ENUM(PLF_ARR_IDX_T, _prefix_, EXTRACT_ENUM_SEQ(_seq_))

#define OP_DECLARE_PLF_ARRAY(r, _data_, i, _elem_) \
    { EXTRACT_POS(_elem_), EXTRACT_LEN(_elem_) },

#define INIT_PLF_ARRAY(_seq_) \
    { BOOST_PP_SEQ_FOR_EACH_I( OP_DECLARE_PLF_ARRAY, , _seq_) }

// 1. declare PRIVATE member:
//   (1) PLF_ARRAY: which named by 'PLF_ARRAY_NAME';
// 2. declare & define PUBLIC functions associated of PLF_ARRAY:
//   (1) static size_t PositionOf(int)
//   (2) static size_t LengthOf(int)
//   (3) static size_t FixedLength();
#define DECLARE_PLF_ARRAY_AND_ACCESS_FUNC(_prefix_) \
    public: \
        static size_t PositionOf(int enum_val) { \
            if (enum_val >= 0 && (size_t)enum_val < BOOST_PP_CAT(PLF_ARR_IDX_T,_get_count)()) \
                return PLF_ARRAY_NAME[enum_val][0] - 1; \
            return 0; \
        } \
        static size_t LengthOf(int enum_val) { \
            if (enum_val >= 0 && (size_t)enum_val < BOOST_PP_CAT(PLF_ARR_IDX_T,_get_count)()) \
                return PLF_ARRAY_NAME[enum_val][1]; \
            return 0; \
        } \
        static size_t FixedLength() { \
            size_t fixlen = PLF_ARRAY_NAME[BOOST_PP_CAT(PLF_ARR_IDX_T,_last_enum)()][0] \
                                 + LengthOf(BOOST_PP_CAT(PLF_ARR_IDX_T,_last_enum)()) \
                                 - PLF_ARRAY_NAME[0][0]; \
            return fixlen; \
        } \
    private: \
        static int PLF_ARRAY_NAME[ENUM_END_NAME(_prefix_, PLF_ARR_IDX_T)][2];

// 1. declare PRIVATE member:
//   (1) const char* _decode_buf;
//   (2) char* _encode_buf;
// 2. declare & define PUBLIC functions:
//   (1) void mark_decode_buffer(const char*);
//   (2) void mark_encode_buffer(char*);
#define DECLARE_CODEC_BUFFER() \
    public: \
        void mark_decode_buffer(const char* decodeBuffer) { _decode_buf = decodeBuffer; } \
        void mark_encode_buffer(char* encodeBuffer) { _encode_buf = encodeBuffer; } \
    private: \
        const char* _decode_buf; \
        char*       _encode_buf

// 1. declare & define PUBLIC functions:
//  (1) std::string get_field(int) const
//  (2) void set_field(int, const char*)
//  (3) uint64_t get_field_as_u64(int) const
//  (4) void set_field(int, uint64_t, char = '0')
//  (5) void set_field(int, const std::string&, char = ' ')
//  (6) void print(std::ostream&)
#define DECLARE_CODEC_FUNC() \
    public: \
        std::string get_field(int enum_val) const { \
            std::string r; \
            if (_decode_buf && enum_val >= 0 && enum_val < BOOST_PP_CAT(PLF_ARR_IDX_T,_last_enum)()) { \
                size_t len = LengthOf(enum_val); \
                r.assign(_decode_buf + PositionOf(enum_val), len); \
            } \
            return r; \
        } \
        void set_field(int enum_val, const char* field_buf) { \
            if (_encode_buf && enum_val >= 0 && enum_val < BOOST_PP_CAT(PLF_ARR_IDX_T,_last_enum)()) { \
                memcpy(_encode_buf, field_buf, LengthOf(enum_val)); \
            } \
        } \
        uint64_t get_field_as_u64(int enum_val) const { \
            std::string r = get_field(enum_val); \
            StrUtil::TrimSpace(r); \
            return StrUtil::ToUlonglong(r); \
        } \
        void set_field(int enum_val, uint64_t field_val, char fill = '0') { \
            if (_encode_buf && enum_val >= 0 && enum_val < BOOST_PP_CAT(PLF_ARR_IDX_T,_last_enum)()) { \
                std::string strField = StrUtil::ToString(field_val); \
                size_t fieldValueLen = strField.size(); \
                size_t fieldMaxLen = LengthOf(enum_val); \
                if (fieldValueLen < fieldMaxLen) { \
                    memset(_encode_buf + PositionOf(enum_val), fill, (fieldMaxLen - fieldValueLen)); \
                } \
                memcpy(_encode_buf + PositionOf(enum_val) + fieldMaxLen - fieldValueLen, strField.c_str(), fieldValueLen); \
            } \
        } \
        void set_field(int enum_val, const std::string& field_val, char fill = ' ') { \
            size_t fieldValueLen = field_val.size(); \
            size_t fieldMaxLen = LengthOf(enum_val); \
            if (fieldValueLen < fieldMaxLen) { \
                memset(_encode_buf + PositionOf(enum_val) + fieldValueLen, fill, (fieldMaxLen - fieldValueLen)); \
            } \
            memcpy(_encode_buf + PositionOf(enum_val), field_val.c_str(), std::min(fieldValueLen, fieldMaxLen)); \
        } \
        void print(std::ostream& os) { \
            for (int i = 0; i < BOOST_PP_CAT(PLF_ARR_IDX_T,_last_enum)(); ++i) { \
                os << (std::string)(BOOST_PP_CAT(PLF_ARR_IDX_T,_get_name)(i)) << ": <" << get_field(i) << ">" << std::endl; \
            } \
        }

#define INIT_CODEC_BUFFER() \
    _decode_buf = NULL; \
    _encode_buf = NULL

#define DECLARE_MEMBERS(_prefix_, _seq_) \
    DECLARE_ENUM(_prefix_, _seq_) \
    DECLARE_PLF_ARRAY_AND_ACCESS_FUNC(_prefix_) \
    DECLARE_CODEC_FUNC() \
    DECLARE_CODEC_BUFFER()

//////////////////////////////////////////////////////////////////////////
// declare a class, which used 'Fixed-Length Structure'.
// usage: put it in 'h' file.
// parameter:
// 1. _class_name_: assignate the Class Name use wanna declare;
// 2. _seq_: BOOST_PP_SEQ < BOOST_PP_TUPLE >
//   'tuple' as: (field_name, field_begin_pos, field_len),
//   'field_begin_pos' begin with 1.
//      e.g. #define EXAMPLE_SEQ
//              (( field_1, 1,  10 ))
//              (( field_2, 11, 2  ))
#define DECLARE_FIXED_LENGTH_CLASS(_class_name_, _seq_) \
    class _class_name_ { \
        DECLARE_MEMBERS(,_seq_); \
    public: \
        _class_name_() { \
            INIT_CODEC_BUFFER(); \
        } \
    }

// usage: put it in 'cpp' file.
#define DEFINE_PLF_ARRAY(_class_name_, _seq_) \
    int _class_name_::PLF_ARRAY_NAME[][2] = INIT_PLF_ARRAY(_seq_)
