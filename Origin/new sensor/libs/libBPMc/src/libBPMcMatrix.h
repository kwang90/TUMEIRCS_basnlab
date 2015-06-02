/*
 * libBPMcMatrix.h
 *
 *  Created on: 29.10.2013
 *      Author: kindt
 */

#ifndef LIBBPMCMATRIX_H_
#define LIBBPMCMATRIX_H_

#include <inttypes.h>

typedef struct _libBPMcMatrix{
	uint32_t x;					///< Number of columns
	uint32_t y;					///< Number of rows
	double** data;				///< Data format is data[y][x] = data[row][column]
} libBPMcMatrix;

void libBPMcMatrix_init(libBPMcMatrix *m, int32_t y, uint32_t x);

void libBPMcMatrix_setToZero(libBPMcMatrix* m);

void libBPMcMatrix_destroy(libBPMcMatrix* m);

void libBPMcMatrix_copy(libBPMcMatrix* src, libBPMcMatrix* dest, uint32_t init);

void libBPMcMatrix_sum(libBPMcMatrix* s1, libBPMcMatrix* s2, libBPMcMatrix* result, uint32_t init);

void libBPMcMatrix_multByScalar(libBPMcMatrix* m, libBPMcMatrix* result, double multiplicator, uint32_t init);

void libBPMcMatrix_matMult(libBPMcMatrix* op1, libBPMcMatrix* op2, libBPMcMatrix* dest, uint32_t init);

void libBPMcMatrix_matPow(libBPMcMatrix* m, libBPMcMatrix* result, uint32_t exponent, uint32_t init);

void libBPMcMatrix_matExpTaylor(libBPMcMatrix *m, libBPMcMatrix* result, uint32_t k, uint32_t init);

double libBPMcMatrix_getSumOfElements(libBPMcMatrix* m);

void libBPMcMatrix_print(libBPMcMatrix* m);
#endif /* LIBBPMCMATRIX_H_ */
