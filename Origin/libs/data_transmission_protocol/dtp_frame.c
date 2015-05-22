/*
 * dtp_frame.c - Toplevel Representation of a single DTP transmission. Contains multiple
 * chuncs.
 *
 *  Created on: 12.03.2013
 *      Author: Philipp Kindt <kindt@rcs.ei.tum.de>
 */

#include "dtp_cfg.h"
#include "dtp_frame.h"
#include "dtp_chunc.h"
#include "dtp_debug.h"
#include <inttypes.h>
#include <stdio.h>
#include <malloc.h>

#ifdef DTP_CFG_PLATTFORM_POSIX
#include <unistd.h>
#include <fcntl.h>
#endif

void dtp_frame_init(dtp_frame* frame){
	if(frame == NULL){
		return;
	}
#if(DTP_CFG_REALLOC_PRESENT == 1)
	frame->chuncs = NULL;
#else

	frame->chuncs = (dtp_chunc**) malloc(DTP_CFG_STATIC_ALLOC_MAXCHUNCS * sizeof(dtp_chunc**));
#endif
	frame->nChuncs = 0;
	frame->size = 0;
}
dtp_chunc* dtp_frame_get_chunc_by_id(dtp_frame* frame, dtp_cfg_id_type type_id){
	if(frame == NULL){
		return NULL;
	}
	if(frame->nChuncs == 0){
		return NULL;
	}
	dtp_cfg_size_type chuncCnt = 0;
	dtp_chunc* rv;
	for(chuncCnt = 0; chuncCnt < frame->nChuncs; chuncCnt++){
		rv = (frame->chuncs[chuncCnt]);
		if(rv->type_id == type_id){
			return rv;
		}
	}
	return NULL;
}
void dtp_frame_add_chunc(dtp_frame* frame, dtp_chunc *chunc){
	if((frame == NULL)||(chunc == NULL)){
		return;
	}
	frame->nChuncs++;
#if(DTP_CFG_REALLOC_PRESENT==1)
		frame->chuncs = realloc(frame->chuncs, frame->nChuncs * sizeof(dtp_chunc*));
#endif
	frame->chuncs[frame->nChuncs - 1] = chunc;
	frame->size += sizeof(struct _dtp_chunc) + chunc->size;
}

void dtp_frame_destroy(dtp_frame* frame, uint8_t destroy_chuncs, uint8_t destroy_chunc_data){
	if(frame == NULL){
		return;
	}
	dtp_cfg_size_type chuncCnt;
	for(chuncCnt = 0; chuncCnt < frame->nChuncs; chuncCnt++){
		if(destroy_chunc_data){
			free(frame->chuncs[chuncCnt]->data);
		}
		if(destroy_chuncs){
			free(frame->chuncs[chuncCnt]);
		}
	}
	frame->nChuncs = 0;
}

void dtp_frame_print(dtp_frame* frame){
	if(frame == NULL){
		return;
	}
	dtp_cfg_size_type cnt2;
	dtp_cfg_size_type chuncCnt;
	for(chuncCnt = 0; chuncCnt < frame->nChuncs; chuncCnt++){
		printf(" --------------[Chunc %d, %d bytes]--------------\n",frame->chuncs[chuncCnt]->type_id, frame->chuncs[chuncCnt]->size);
		printf("[");
			if(frame->chuncs[chuncCnt]->size == 0){
				printf(" - empty - \n");
			}else{
				for(cnt2 = frame->chuncs[chuncCnt]->size; cnt2 > 0; cnt2--){
					printf("%x",frame->chuncs[chuncCnt]->data[cnt2 - 1]);
					if(((cnt2) % 1 == 0)||(cnt2 != 0)){
						printf(" ");
					}
				}
			}
			printf("]\n");

	}
}
dtp_frame* dtp_frame_generate(uint8_t *bitstream){
	if(bitstream == NULL){
		return NULL;
	}
	dtp_cfg_size_type nChuncs = *((dtp_cfg_size_type*) bitstream);
	DTP_DEBUG("Decoding Frame with %d chuncs.",nChuncs);
	dtp_cfg_size_type cnt;
	dtp_chunc* tmp;
	uint8_t* ptr = bitstream + sizeof(nChuncs);
	dtp_frame* f = (dtp_frame*) malloc(sizeof(dtp_frame));
	f->size = 0;
	f->nChuncs = nChuncs;
	f->chuncs = (dtp_chunc**) malloc(nChuncs * sizeof(dtp_chunc**));


	for(cnt = 0; cnt < nChuncs; cnt++){
		tmp = (dtp_chunc*) ptr;
		f->chuncs[cnt] = (dtp_chunc*) malloc(sizeof(dtp_chunc));
		DTP_DEBUG(" Chunc %d: type_id= %d",cnt,tmp->type_id);
		DTP_DEBUG(" Chunc %d: size= %d",cnt,tmp->size);

		f->chuncs[cnt]->size = tmp->size;
		f->chuncs[cnt]->type_id = tmp->type_id;
		f->chuncs[cnt]->data = (uint8_t*) (ptr + sizeof(tmp->size) + sizeof(tmp->type_id));
		ptr += sizeof(dtp_cfg_size_type) + sizeof(dtp_cfg_id_type) + tmp->size;
		f->size +=  sizeof(dtp_cfg_size_type) + sizeof(dtp_cfg_id_type) + tmp->size;


	}
	return f;
}
#if DTP_CFG_PLATTFORM==DTP_CFG_PLATTFORM_POSIX
dtp_frame* dtp_frame_generate_from_filedescriptor(int fd){
	uint8_t buf2[256];
	if(fd < 0){
		DTP_DEBUG("Invalid Fildescriptor!");
		return NULL;
	}

	//read frame header
	if(read(fd,buf2,sizeof(dtp_cfg_size_type)) != sizeof(dtp_cfg_size_type)){
		DTP_DEBUG("Could not get dtp_frame header");
		return NULL;
	} ;
	dtp_cfg_size_type nChuncs = *((dtp_cfg_size_type*) buf2);
	DTP_DEBUG("Decoding Frame with %d chuncs.",nChuncs);
	dtp_cfg_size_type cnt;
	dtp_chunc* tmp;
	dtp_frame* f = (dtp_frame*) malloc(sizeof(dtp_frame));
	f->size = 0;
	f->nChuncs = nChuncs;
	f->chuncs = (dtp_chunc**) malloc(nChuncs * sizeof(dtp_chunc**));


	for(cnt = 0; cnt < nChuncs; cnt++){
		if(read(fd,buf2,sizeof(dtp_cfg_id_type) + sizeof(dtp_cfg_size_type)) != sizeof(dtp_cfg_id_type) + sizeof(dtp_cfg_size_type)){
			return NULL;
		} ;

		tmp = (dtp_chunc*) buf2;
		f->chuncs[cnt] = (dtp_chunc*) malloc(sizeof(dtp_chunc));
		DTP_DEBUG(" Chunc %d: type_id= %d",cnt,tmp->type_id);
		DTP_DEBUG(" Chunc %d: size= %d",cnt,tmp->size);

		f->chuncs[cnt]->size = tmp->size;
		f->chuncs[cnt]->type_id = tmp->type_id;
		f->chuncs[cnt]->data = (uint8_t*) malloc(tmp->size);
		DTP_DEBUG("reading payload of %d bytes...",tmp->size);
		if(read(fd,f->chuncs[cnt]->data,tmp->size) != tmp->size){
			printf("Read failed.");
			return NULL;

		} ;
		uint32_t cnt2;
		f->size +=  sizeof(dtp_cfg_size_type) + sizeof(dtp_cfg_id_type) + tmp->size;

	}
	DTP_DEBUG("Frame complete, size is: %d bytes without header",f->size);
	return f;
}

#endif
dtp_cfg_size_type dtp_frame_get_size(dtp_frame* frame, uint8_t wholeFrame){
	if(frame == NULL){
		return 0;
	}
	if(!wholeFrame){
		return frame->size;
	}else{
		return frame->size + sizeof(dtp_frame) - sizeof(dtp_chunc**);
	}

}
