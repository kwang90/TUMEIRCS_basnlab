/*
 * dtp_bitstream.c
 *
 *  Created on: 12.03.2013
 *      Author: kindt
 */

#include "dtp_bitstream.h"
#include "dtp_cfg.h"
#include "dtp_frame.h"
#include "dtp_chunc.h"
#include <stdio.h>
#include <string.h>
#include <malloc.h>
uint8_t* dtp_bitstream_generate(dtp_frame* f, uint8_t *bitstream, dtp_cfg_size_type *size){
	dtp_cfg_size_type size_tmp = 0;

	if(f == NULL){
		return NULL;
	}
	if(bitstream == NULL){
		bitstream = (uint8_t*) malloc(f->size);		//f->size is pessimistic, but allways enough
	}else{
#if(DTP_CFG_REALLOC_PRESENT==1)
	bitstream = realloc(bitstream, f->size);	//f->size is pessimistic, but allways enough
#else
	free(bitstream);
	bitstream = (uint8_t*) malloc(f->size);		//f->size is pessimistic, but allways enough
#endif
	}
	uint8_t* ptr = bitstream;
	dtp_cfg_size_type sizeT;
	sizeT = f->nChuncs;
	memcpy(ptr,&sizeT,sizeof(sizeT)); ptr += sizeof(sizeT);
	size_tmp += sizeof(sizeT);
	dtp_cfg_id_type chuncCnt;
	dtp_chunc* tmp;
	for(chuncCnt = 0; chuncCnt < f->nChuncs; chuncCnt++){
		tmp = f->chuncs[chuncCnt];
		memcpy(ptr,&(tmp->type_id),sizeof(tmp->type_id)); ptr += sizeof(tmp->type_id);
		memcpy(ptr,&(tmp->size),sizeof(tmp->size)); ptr += sizeof(tmp->size);
		memcpy(ptr,(tmp->data),tmp->size); ptr += tmp->size;
		size_tmp += sizeof(tmp->size) + sizeof(tmp->type_id) + tmp->size;

	}
	if(size != NULL){
		*size = size_tmp;
	}
	return bitstream;
}

void dtp_bitstream_print(uint8_t* bitstream){
	dtp_cfg_size_type nChuncs = *((dtp_cfg_size_type*) bitstream);
	dtp_cfg_size_type cnt,cnt2;
	dtp_chunc* tmp;
	uint8_t* ptr = bitstream + sizeof(nChuncs);
	for(cnt = 0; cnt < nChuncs; cnt++){
		tmp = (dtp_chunc*) ptr;
		ptr += sizeof(dtp_cfg_size_type) + sizeof(dtp_cfg_id_type);

		printf("----[ Chunc ID %d, %d bytes] ----\n",tmp->type_id, tmp->size);
		printf("[");
		if(tmp->size == NULL){
			printf(" - empty - \n");
		}else{
			for(cnt2 = tmp->size; cnt2 > 0; cnt2--){
				printf("%x",ptr[cnt2 - 1]);
				if(((cnt2) % 1 == 0)||(cnt2 != 0)){
					printf(" ");
				}
			}
		}
		ptr = ptr + tmp->size;

		printf("]\n");

	}
}

