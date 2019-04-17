#pragma once
#include <debug/debug.h>

namespace MemUtil {

    // similar with std::auto_ptr
    template <class T_>
      class PtrHolder {
      public:
        PtrHolder(T_* ptr) throw() : _ptr(ptr) {}
        ~PtrHolder() throw() {
            delete _ptr;
        }
        void cancel() throw() {
            _ptr = NULL;
        }
        T_* release() throw() {
            T_* tmp = _ptr;
            _ptr = 0;
            return tmp;
        }
        const T_* get() const throw() { return _ptr; }
        T_* get() throw() { return _ptr; }
        T_& operator*() const throw() {
            _GLIBCXX_DEBUG_ASSERT(_ptr != 0);
            return *_ptr;
        }
        T_* operator->() const throw() {
            _GLIBCXX_DEBUG_ASSERT(_ptr != 0);
            return _ptr;
        }

      private:
        T_* _ptr;
      };

    template <class T_>
      class ArrayPtrHolder {
      public:
        ArrayPtrHolder(T_* ptr) : _arr_ptr(ptr) {}
        ~ArrayPtrHolder() {
            delete [] _arr_ptr;
        }
        void cancel() {
            _arr_ptr = NULL;
        }
        const T_* get() const throw() { return _arr_ptr; }
        T_* get() throw() { return _arr_ptr; }

      private:
        T_* _arr_ptr;
      };
      
    // c language poionter holder which call 'free' install of 'delete' when destruct.
    template <class T_>
    class ClangPtrHolder {
      public:
        ClangPtrHolder(T_* ptr) throw() : _ptr(ptr) {}
        ~ClangPtrHolder() throw() {
            free(_ptr);
        }
        void cancel() throw() {
            _ptr = NULL;
        }
        T_* release() throw() {
            T_* tmp = _ptr;
            _ptr = 0;
            return tmp;
        }
        const T_* get() const throw() { return _ptr; }
        T_* get() throw() { return _ptr; }
        T_& operator*() const throw() {
            _GLIBCXX_DEBUG_ASSERT(_ptr != 0);
            return *_ptr;
        }
        T_* operator->() const throw() {
            _GLIBCXX_DEBUG_ASSERT(_ptr != 0);
            return _ptr;
        }

      private:
        T_* _ptr;
    };
}
