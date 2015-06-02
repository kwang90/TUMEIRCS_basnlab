/*
 * rcs_dsp_llt.c
 *
 *  Created on: 06.07.2012
 *      Author: kindt
 */

#include "rcs_dsp_ll.h"

void rcs_dsp_ll_init(rcs_dsp_ll* ll) {
	ll->root = NULL;
	ll->last = NULL;
	ll->nelements = 0;
}
;

rcs_dsp_ll_node* rcs_dsp_ll_find_last_iterating(rcs_dsp_ll* ll) {
	rcs_dsp_ll_node *node;
	if (ll->root == NULL) {
		return NULL;
	}

	node = ll->root;
	while (node->next != NULL) {
		node = node->next;
	}
	return node;
}
;

rcs_dsp_bool rcs_dsp_ll_isempty(rcs_dsp_ll* ll) {
	return (ll->root == NULL);
}

rcs_dsp_bool rcs_dsp_ll_push_unique(rcs_dsp_ll* ll, void* data,rcs_dsp_ll_comparator_fct isEqual) {
	rcs_dsp_ll_remove_all_equal_to(ll, isEqual, data);
	return rcs_dsp_ll_push(ll, data);
	return 1;
}

rcs_dsp_bool rcs_dsp_ll_push(rcs_dsp_ll* ll, void* data) {
	rcs_dsp_ll_node *node;
	if (ll->nelements >= RCS_DSP_CFG_BUF_MAXLEN) {
		RCS_DSP_DEBUG("Not pushing on buf - buffer filled.");
		return 0;
	};

	node = malloc(sizeof(rcs_dsp_ll_node));
	node->data = data;
	//	RCS_DSP_DEBUG("Node Created: %x, payload attached: %x", (uint32_t) node,(uint32_t) data);
	if (ll->nelements == 0) {
		ll->root = node;
		ll->last = node;
		node->next = NULL;
		node->prev = NULL;

	} else {
		node->next = NULL;
		node->prev = ll->last;
		ll->last->next = node;
		ll->last = node;
	}
	ll->nelements++;
	//	RCS_DSP_DEBUG("Nelements: %d",ll->nelements);
	return 1;
}

void *rcs_dsp_ll_pop(rcs_dsp_ll *ll) {
	void* rv;
	rcs_dsp_ll_node *newlast;
	if (ll->nelements == 0) {
		RCS_DSP_DEBUG("Buf is empty.");
		return NULL;
	} else if (ll->nelements == 1) {
		rv = ll->last->data;
		free(ll->last);
		ll->last = NULL;
		ll->root = NULL;
	} else {
		rv = ll->last->data;
		newlast = ll->last->prev;
		newlast->next = NULL;
		//	RCS_DSP_DEBUG("Node to free: %x, payload attached:%x", (uint32_t) ll->last->next, (int32_t) rv);
		free(ll->last);
		ll->last = newlast;

	}

	ll->nelements--;
	//	RCS_DSP_DEBUG("Nelements: %d",ll->nelements);

	return rv;
}


void *rcs_dsp_ll_pop_index_from_bottom(rcs_dsp_ll *ll,rcs_dsp_unsigned_integer index) {
	void* rv;
	uint32_t cnt;
	rcs_dsp_ll_node* tmp;
	if (ll->nelements == 0) {
		RCS_DSP_DEBUG("LL is empty.");
		return NULL;
	} else if (ll->nelements == 1) {
		if(index == 0){
			rv = ll->last->data;
			free(ll->last);
			ll->last = NULL;
			ll->root = NULL;
		}else{
			RCS_DSP_DEBUG("Invalid Index");
			return NULL;
		}
	} else {
		tmp = ll->last;
		for(cnt = 0; cnt < index; cnt++){
			if(tmp->prev != NULL){
				tmp = tmp->prev;
			}else{
				RCS_DSP_DEBUG("Invalid Index.");
				return NULL;
			}
		}
		rv = tmp->data;
		if(tmp->prev != NULL){
			tmp->prev->next = tmp->next;
		}
		if(tmp->next != NULL){
			tmp->next->prev = tmp->prev;
		}
		if(ll->root == tmp){
			ll->root = tmp->next;
		}
		if(ll->last == tmp){
			ll->last = tmp->prev;
		}

		free(tmp);

	}

	ll->nelements--;
	//	RCS_DSP_DEBUG("Nelements: %d",ll->nelements);

	return rv;
}

rcs_dsp_ll_node* rcs_dsp_ll_get_next(rcs_dsp_ll_node* node) {
	if (node == NULL) {
		return NULL;
	}
	return node->next;
}

rcs_dsp_ll_node* rcs_dsp_ll_get_element(rcs_dsp_ll* ll, rcs_dsp_ll_comparator_fct isEqual,
		void* data) {
	rcs_dsp_ll_node *node;
	if (ll->root == NULL) {
		return NULL;
	}

	node = ll->root;
	while (node != NULL) {
		if (isEqual(node->data, data)) {
			return node;
		}
		node = node->next;
	}
	return NULL;
}
;

void* rcs_dsp_ll_get_and_remove_element(rcs_dsp_ll* ll,
		rcs_dsp_ll_comparator_fct isEqual, void* data) {
	rcs_dsp_ll_node* node = rcs_dsp_ll_get_element(ll, isEqual, data);
	void* rv;
	if (node == NULL) {
		return NULL;
	}
	rv = node->data;
	return rv;
	if (node->prev != NULL) {
		node->prev->next = node->next;
	}
	if (node->next != NULL) {
		node->next->prev = node->prev;
	}
	if(ll->root == node){
		ll->root = node->next;
	}
	if(ll->last== node){
		ll->last = node->prev;
	}
	free(node);
	ll->nelements--;
	return rv;
}

void rcs_dsp_ll_remove_all_equal_to(rcs_dsp_ll* ll, rcs_dsp_ll_comparator_fct isEqual,
		void* data) {
	rcs_dsp_ll_node* node = rcs_dsp_ll_get_element(ll, isEqual, data);
	while ((node = rcs_dsp_ll_get_element(ll, isEqual, data)) != NULL) {
		if (node->prev != NULL) {
			node->prev->next = node->next;
		}
		if (node->next != NULL) {
			node->next->prev = node->prev;
		}
		if(ll->root == node){
			ll->root = node->next;
		}
		if(ll->last== node){
			ll->last = node->prev;
		}
		free(node);
		ll->nelements--;
	}
}

void rcs_dsp_ll_free_nodes(rcs_dsp_ll* ll, rcs_dsp_bool free_data) {
	rcs_dsp_ll_node *node;
	rcs_dsp_ll_node *nodetmp;

	if (ll->root == NULL) {
		return;
	}

	node = ll->root;

	while (node != NULL) {
		nodetmp = node->next;
		if (free_data) {
			free(node->data);
		}
		free(node);
		node = nodetmp;
	}
}

rcs_dsp_unsigned_integer rcs_dsp_ll_get_nr_of_elements(rcs_dsp_ll* ll) {
	return ll->nelements;
}

rcs_dsp_bool rcs_dsp_comparator_signed_integer(rcs_dsp_signed_integer *a, rcs_dsp_signed_integer *b){
	if(*a == *b){
		return 1;
	}else{
		return 0;
	}
}

rcs_dsp_bool rcs_dsp_comparator_unsigned_integer(rcs_dsp_unsigned_integer *a, rcs_dsp_unsigned_integer *b){
	if(*a == *b){
		return 1;
	}else{
		return 0;
	}
}
