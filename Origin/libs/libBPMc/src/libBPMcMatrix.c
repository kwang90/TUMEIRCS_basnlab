#include "libBPMcMatrix.h"
#include <malloc.h>
#include <inttypes.h>
#include <string.h>

double _libBPMcMatrix_factorial(uint32_t n){
	uint32_t i;
	double result = (double)n;
	if(n == 0){
		return 1;
	}
	if(n == 1){
		return 1;
	}

	for(i = n - 1;  i > 1 ; i--){
		result *= i;
	}
	return result;
}

void libBPMcMatrix_init(libBPMcMatrix *m, int32_t y, uint32_t x){
	m->x = x;
	m->y = y;
	m->data = malloc(y * sizeof(double*));
	if(m->data == NULL){
#if LIBBPMC_PLATFORM_ARCHITECTURE == LIBBPMC_PLATFORM_ARCHITECTURE_POSIX
		perror("ERROR: malloc() failed: ");
#else
		printf("ERROR: malloc() failed.");
#endif
		return;
	}

	uint32_t cnt;
	for(cnt = 0; cnt < y; cnt++){
		m->data[cnt] = malloc(x*sizeof(double));
		if(m->data[cnt] == NULL){
	#if LIBBPMC_PLATFORM_ARCHITECTURE == LIBBPMC_PLATFORM_ARCHITECTURE_POSIX
			perror("ERROR: malloc() failed in alloc loop: ");
	#else
			printf("ERROR: malloc() failed in alloc loop.");
	#endif
			return;
		}

	}
}
void libBPMcMatrix_setToZero(libBPMcMatrix* m){
	uint32_t row,col;
	for(row = 0; row < m->y; row++){
		for(col = 0; col < m->x; col++){
			m->data[row][col] = 0;
		}
	}
}

void libBPMcMatrix_setToI(libBPMcMatrix* m){
	if(m->x != m->y){
		printf("error: dimensions must be equal\n");
		return;
	}
	uint32_t d = 0;
	libBPMcMatrix_setToZero(m);
	for(d = 0; d < m->x; d++){
		m->data[d][d] = 1.0;
	}
}

void libBPMcMatrix_destroy(libBPMcMatrix* m){
	uint32_t cnt = 0;
//	printf("Destroy: %x\n",m);
	for(cnt = 0; cnt < m->y; cnt++){
		free(m->data[cnt]);
	}
	free(m->data);

}

void libBPMcMatrix_matMult(libBPMcMatrix* op1, libBPMcMatrix* op2, libBPMcMatrix* dest, uint32_t init){
	if(op1->x != op2->y){
		printf("MatMult(): error: op1->x != op2->y as it ought to be.");
		return;
	}
	if(init){
		libBPMcMatrix_init(dest,op1->y,op2->x);
	}
	uint32_t x;
	uint32_t y;
	uint32_t i;
	double partSum;
	for(y = 0; y < op1->y; y++){
		for(x = 0; x < op2->x; x++){
			partSum = 0;
			for(i = 0; i < op1->x; i++){
				partSum += op1->data[y][i] * op2->data[i][x];
			}
			dest->data[y][x] = partSum;
		}
	}
}
void libBPMcMatrix_sum(libBPMcMatrix* s1, libBPMcMatrix* s2, libBPMcMatrix* result, uint32_t init){
	uint32_t x,y;

	if((s1->x != s2->x)||(s1->y != s2->y)){
		printf("dimension missmatch\n");
	}

	if(init){
		libBPMcMatrix_init(result,s1->x,s1->y);
	}
	for(y = 0; y < s1->y; y++){
		for(x = 0; x < s1->x; x++){
			result->data[y][x] = s1->data[y][x] + s2->data[y][x];
		}
	}
}

void libBPMcMatrix_copy(libBPMcMatrix* src, libBPMcMatrix* dest, uint32_t init){
	uint32_t x,y;
	if(init){
		libBPMcMatrix_init(dest,src->x,src->y);
	}
	for(y = 0; y < src->y; y++){
		for(x = 0; x < src->x; x++){
			dest->data[y][x] = src->data[y][x];
		}
	}
}

void libBPMcMatrix_matPow(libBPMcMatrix* m, libBPMcMatrix* result, uint32_t exponent, uint32_t init){
	uint32_t cnt = 0;
	uint8_t first = 1;
	libBPMcMatrix tmp;
	if(exponent == 0){
		if(init){
			libBPMcMatrix_init(result,m->y,m->x);
		}
		libBPMcMatrix_setToI(result);
		return;
	}
	libBPMcMatrix_copy(m,result,init);
	//printf("result for exp %d: \n",exponent);
	//libBPMcMatrix_print(m);
	for(cnt = 0; cnt < exponent-1; cnt++){

		libBPMcMatrix_matMult(m,result,&tmp,1);
		libBPMcMatrix_copy(&tmp,result,0);
		libBPMcMatrix_destroy(&tmp);

	}
	//libBPMcMatrix_print(result);

}

void libBPMcMatrix_multByScalar(libBPMcMatrix* m, libBPMcMatrix* result, double multiplicator, uint32_t init){
	uint32_t x,y;
	if(init){
		libBPMcMatrix_init(result,m->y,m->x);
	}

	for(y = 0; y < m->y; y++){
		for(x = 0; x < m->x; x++){
			result->data[y][x] = m->data[y][x] * multiplicator;
		}
	}

}
void libBPMcMatrix_print(libBPMcMatrix* m){
	uint32_t x;
	uint32_t y;
	printf("\t\t");
	for(x= 0; x < m->x; x++){
		printf("[%d]\t\t",x);
	}
	printf("\n");

	for(y = 0; y < m->y; y++){			//all dest
		for(x = 0; x < m->x; x++){		//all src classes
			if(x== 0){
				printf("[%d]\t\t",y);
			};
			printf("% -7.5g\t\t",(double) m->data[y][x]);
		}
		printf("\n");
	}

}

void libBPMcMatrix_matExpTaylor(libBPMcMatrix *m, libBPMcMatrix* result, uint32_t k, uint32_t init){
	libBPMcMatrix tmp1;
	libBPMcMatrix tmp2;
	uint32_t i;
	double tmp = 0;
	libBPMcMatrix_init(&tmp1,m->x,m->y);
	libBPMcMatrix_init(&tmp2,m->x,m->y);
	if(init){
		libBPMcMatrix_init(result,m->x,m->y);
	}
	//printf("taylor initial: size (%d x %d)\n",m->x,m->y);
	//libBPMcMatrix_print(m);
	libBPMcMatrix_setToZero(result);
	for(i = 0; i <= k; i++){
		libBPMcMatrix_matPow(m,&tmp1,i,0);
//		printf("i=%d\n",i);
//		libBPMcMatrix_print(&tmp1);


		libBPMcMatrix_multByScalar(&tmp1,&tmp2,1.0/(double) _libBPMcMatrix_factorial(i),0);

		libBPMcMatrix_sum(&tmp2,result,result,0);
	//	printf("//////////////////////////////////////////////////////////////////////////////////////////////////\n");
	}
	libBPMcMatrix_destroy(&tmp1);
	libBPMcMatrix_destroy(&tmp2);


}


double libBPMcMatrix_getSumOfElements(libBPMcMatrix* m){
	uint32_t x;
	uint32_t y;
	double sum = 0;
	for(y = 0; y < m->y; y++){			//all dest
		for(x = 0; x < m->x; x++){		//all src classes
			sum += m->data[y][x];
		}
	}
	return sum;
}



