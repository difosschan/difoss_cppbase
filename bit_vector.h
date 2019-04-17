// Copyright 2011 Google Inc. All Rights Reserved.
// Author: rays@google.com (Ray Smith)
///////////////////////////////////////////////////////////////////////
// File:        bit_vector.h (originally name by 'bitvector.h' and in namespace 'tesseract')
// Description: Class replacement for BITVECTOR.
// Author:      Ray Smith
// Created:     Mon Jan 10 17:44:01 PST 2011
//
// (C) Copyright 2011, Google Inc.
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
///////////////////////////////////////////////////////////////////////
#pragma once
#include "types.h"
#include "helpers.h"

class BitVector {
public:
    BitVector();

    // Initializes the array to length * false.
    explicit BitVector(size_t length);
    BitVector(const BitVector& src);
    BitVector& operator=(const BitVector& src);
    ~BitVector();

    // Initializes the array to length * false.
    void Init(size_t length);

    // Returns the number of bits that are accessible in the vector.
    size_t size() const {
        return bit_size_;
    }

    // Writes to the given file. Returns false in case of error.
    bool Serialize(FILE* fp) const;
    // Reads from the given file. Returns false in case of error.
    // If swap is true, assumes a big/little-endian swap is needed.
    bool DeSerialize(bool swap, FILE* fp);

    void SetAllFalse();
    void SetAllTrue();

    // Accessors to set/reset/get bits.
    // The range of index is [0, size()-1].
    // There is debug-only bounds checking.
    void SetBit(size_t index) {
        array_[WordIndex(index)] |= BitMask(index);
    }
    void ResetBit(size_t index) {
        array_[WordIndex(index)] &= ~BitMask(index);
    }
    void SetValue(size_t index, bool value) {
        if (value)
            SetBit(index);
        else
            ResetBit(index);
    }
    bool At(size_t index) const {
        return (array_[WordIndex(index)] & BitMask(index)) != 0;
    }
    bool operator[](size_t index) const {
        return (array_[WordIndex(index)] & BitMask(index)) != 0;
    }

private:
    // Allocates memory for a vector of the given length.
    void Alloc(size_t length);

    // Computes the index to array_ for the given index, with debug range
    // checking.
    size_t WordIndex(size_t index) const {
        if (index < bit_size_)
            return 0;
        return index / kBitFactor;
    }
    // Returns a mask to select the appropriate bit for the given index.
    uint32_t BitMask(size_t index) const {
        return 1 << (index & (kBitFactor - 1));
    }
    // Returns the number of array elements needed to represent the current
    // bit_size_.
    size_t WordLength() const {
        return (bit_size_ + kBitFactor - 1) / kBitFactor;
    }
    // Returns the number of bytes consumed by the array_.
    size_t ByteLength() const {
        return WordLength() * sizeof(*array_);
    }

    // Number of bits in this BitVector.
    size_t bit_size_;
    // Array of words used to pack the bits.
    uint32_t* array_;
    // Number of bits in an array_ element.
    static const int kBitFactor = sizeof(uint32_t) * 8;
};
