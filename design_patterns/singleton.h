#ifndef SINGLETON_H
#define SINGLETON_H

/////////////////////////////
// Meyers Singleton Pattern
#define SINGLETON_DECLARE( _class_name_ ) \
public: \
    static _class_name_& instance() \
    { \
        static _class_name_ _theone; \
        return _theone; \
    } \
public: \
    virtual ~_class_name_(); \
private: \
    _class_name_(); \
    _class_name_(const _class_name_&){} \
    _class_name_& operator = (const _class_name_&){return *this;} \

// 构造器缺省实现
#define SINGLETON_IMPLEMENT_CONSTRUCTOR( _class_name_ ) \
    _class_name_::_class_name_(){}

// 析构器缺省实现
#define SINGLETON_IMPLEMENT_DESTRUCTOR( _class_name_ ) \
    _class_name_::~_class_name_(){}

#endif // SINGLETON_H
