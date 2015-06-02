/**
 * \file Vector.hpp
 * \brief template library for additional vector function
 *
 *  \date 11.10.2013
 *  \author: Mathias Gopp <gopp@rcs.ei.tum.de>
 */

#ifndef VECTOR_HPP
#define VECTOR_HPP

#include <vector>
#include <inttypes.h>

namespace linAlg{


template <class T>
class Vector : public std::vector<T>
{
public:

    ///constructor
    Vector();

    ///constructor
    Vector(int);

    ///destructor
    ~Vector();

    ///returns the value of an element(index)
    T operator()(uint32_t index) const;

    ///sets an element(index)
    T& operator()(uint32_t index);

};

//implementation follows here
#include <vector>
#include <inttypes.h>

template <class T>
Vector<T>::Vector()
{

}

template <class T>
Vector<T>::Vector(int init):vector<T>(init)
{

}

template <class T>
Vector<T>::~Vector()
{

}

template <class T>
T Vector<T>::operator()(uint32_t index) const
{
    return (*this)[index];
}

template <class T>
T& Vector<T>::operator()(uint32_t index)
{
    return this->operator[](index);
}

}
#endif // VECTOR_HPP
