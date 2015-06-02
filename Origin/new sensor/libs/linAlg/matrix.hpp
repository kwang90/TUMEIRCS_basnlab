/**
 * \file matrix.hpp
 * \brief template library for basic matrix operations
 *
 *  \date 10.10.2013
 *  \author: Mathias Gopp <gopp@rcs.ei.tum.de>
 */

#ifndef MATRIX_HPP
#define MATRIX_HPP

#include <vector>
#include <inttypes.h>

namespace linAlg{
using namespace std;

template <class T>
class Matrix
{
public:

    ///constructor for creating a matrix(rows, columns)
    Matrix(uint32_t, uint32_t);

    ///destructor
    ~Matrix();

    ///sets an element in the matrix(element, row, column)
    void setElement(T, uint32_t, uint32_t);

    ///returns the value of an element in the matrix(row, column)
    T getElement(uint32_t, uint32_t);

    ///returns the value of an element in the matrix(row, column)
    T operator()(uint32_t row, uint32_t column) const;

    ///sets an element in the matrix(element, row, column)
    T& operator()(uint32_t row, uint32_t column);

private:

    //2 dimensional vector
    vector<vector<T> > matrix;
};

//implementation follows here

template <class T>
Matrix<T>::Matrix(uint32_t rows, uint32_t columns)
{
    //create the matrix with rows and columns
    matrix.resize(rows);

    for(uint32_t i=0;i<rows;i++)
    {
         matrix[i].resize(columns);
    }
}

template <class T>
Matrix<T>::~Matrix()
{
    //free matrix;
}

template <class T>
void Matrix<T>::setElement(T value, uint32_t row,uint32_t column)
{
    matrix[row][column]=value;
}

template <class T>
T Matrix<T>::getElement(uint32_t row,uint32_t column)
{
    T value;
    return value = matrix[row][column];
}

template <class T>
T Matrix<T>::operator()(uint32_t row, uint32_t column) const
{
    T value;
    return value = matrix[row][column];
}

template <class T>
T& Matrix<T>::operator()(uint32_t row, uint32_t column)
{
    return matrix[row][column];
}

}
#endif // MATRIX_HPP
