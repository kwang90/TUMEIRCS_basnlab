/*
 * sble_llt.c
 *
 *  Created on: 06.07.2012
 *      Author: kindt
 */

#include "sble_ll.h"
#include "sble_scheduler.h"

void sble_ll_init(sble_ll* ll) {
	ll->root = NULL;
	ll->last = NULL;
	ll->nelements = 0;

}
;

sble_ll_node* sble_ll_find_last_iterating(sble_ll* ll) {
	sble_ll_node *node;
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

sble_bool sble_ll_isempty(sble_ll* ll) {

	sble_bool rv = (ll->root == NULL);

	return rv;
}

sble_bool sble_ll_push_unique(sble_ll* ll, void* data,sble_ll_comparator_fct isEqual) {

	sble_ll_remove_all_equal_to(ll, isEqual, data);
	sble_bool rv = sble_ll_push(ll, data);
}

sble_bool sble_ll_push(sble_ll* ll, void* data) {
	sble_ll_node *node;

	if (ll->nelements >= SBLE_BUF_MAXLEN)
	{
		SBLE_DEBUG("Not pushing on buf - buffer filled.");
		return 0;
	}

	node = malloc(sizeof(sble_ll_node));
	node->data = data;

	//SBLE_DEBUG("Node Created: %x, payload attached: %x", (uint32_t) node,(uint32_t) data);
	if (ll->nelements == 0)
	{
		ll->root = node;
		ll->last = node;
		node->next = NULL;
		node->prev = NULL;
	}
	else
	{
		node->next = NULL;
		node->prev = ll->last;
		ll->last->next = node;
		ll->last = node;
	}

	ll->nelements++;
	//	SBLE_DEBUG("Nelements: %d",ll->nelements);
	return 1;
}
void *sble_ll_pop_from_begin(sble_ll *ll) {
	void* rv;
	sble_ll_node *newroot;

	if (ll->nelements == 0) {
		SBLE_DEBUG("Buf is empty.");
		return NULL;
	} else if (ll->nelements == 1) {
		rv = ll->root->data;
		free(ll->root);
		ll->last = NULL;
		ll->root = NULL;
		//SBLE_DEBUG("LIST SIZE: 1\n");
	} else {
		rv = ll->root->data;
		if(ll->root->next == NULL){
			SBLE_ERROR("ll->root->next = NULL");
		}

		newroot = ll->root->next;

		newroot->prev = NULL;
		//	SBLE_DEBUG("Node to free: %x, payload attached:%x", (uint32_t) ll->last->next, (int32_t) rv);
		free(ll->root);
		ll->root = newroot;

	}

	ll->nelements--;
//	SBLE_DEBUG("Nelements: %d",ll->nelements);

	return rv;
}
void *sble_ll_pop(sble_ll *ll) {
	void* rv;
	sble_ll_node *newlast;

	if (ll->nelements == 0) {
		SBLE_DEBUG("Buf is empty.");
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
		//	SBLE_DEBUG("Node to free: %x, payload attached:%x", (uint32_t) ll->last->next, (int32_t) rv);
		free(ll->last);
		ll->last = newlast;

	}

	ll->nelements--;
	//	SBLE_DEBUG("Nelements: %d",ll->nelements);

	return rv;
}


void *sble_ll_pop_index_from_bottom(sble_ll *ll,sble_unsigned_integer index) {
	void* rv;
	uint32_t cnt;
	sble_ll_node* tmp;

	if (ll->nelements == 0) {
		SBLE_DEBUG("LL is empty.");
		return NULL;
	} else if (ll->nelements == 1) {
		if(index == 0){
			rv = ll->last->data;
			free(ll->last);
			ll->last = NULL;
			ll->root = NULL;
		}else{
			SBLE_DEBUG("Invalid Index");
			return NULL;
		}
	} else {
		tmp = ll->last;
		for(cnt = 0; cnt < index; cnt++){
			if(tmp->prev != NULL){
				tmp = tmp->prev;
			}else{
				SBLE_DEBUG("Invalid Index.");
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
	//	SBLE_DEBUG("Nelements: %d",ll->nelements);

	return rv;
}

sble_ll_node* sble_ll_get_next(sble_ll_node* node) {

	if (node == NULL) {
		return NULL;
	}

	sble_ll_node* n = node->next;
	return n;
}

sble_ll_node* sble_ll_get_element(sble_ll* ll, sble_ll_comparator_fct isEqual, void* data)
{
	sble_ll_node *node;

	if (ll->root == NULL)
	{
		return NULL;
	}

	node = ll->root;
	while (node != NULL)
	{
		if (isEqual(node->data, data))
		{
			return node;
		}
		node = node->next;
	}
	return NULL;
}
;

void* sble_ll_get_and_remove_element(sble_ll* ll,
	sble_ll_comparator_fct isEqual, void* data) {

	sble_ll_node* node = sble_ll_get_element(ll, isEqual, data);
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

void sble_ll_remove_all_equal_to(sble_ll* ll, sble_ll_comparator_fct isEqual,
		void* data) {

	sble_ll_node* node = sble_ll_get_element(ll, isEqual, data);
	while ((node = sble_ll_get_element(ll, isEqual, data)) != NULL) {
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

void sble_ll_free_nodes(sble_ll* ll, sble_bool free_data) {
	sble_ll_node *node;
	sble_ll_node *nodetmp;

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

void sble_ll_remove_all_elements(sble_ll* ll)
{
	sble_unsigned_integer numOfElements = sble_ll_get_nr_of_elements(ll);
	uint32_t i = 0;
	for(i = 0; i < numOfElements; i++)
	{
		sble_ll_pop_from_begin(ll);
	}
}

sble_unsigned_integer sble_ll_get_nr_of_elements(sble_ll* ll) {

	sble_unsigned_integer rv = ll->nelements;
	return rv;

}

sble_bool sble_comparator_signed_integer(sble_signed_integer *a, sble_signed_integer *b){
	if(*a == *b){
		return SBLE_TRUE;
	}else{
		return SBLE_FALSE;
	}
}

sble_bool sble_comparator_unsigned_integer(sble_unsigned_integer *a, sble_unsigned_integer *b){
	if(*a == *b){
		return SBLE_TRUE;
	}else{
		return SBLE_FALSE;
	}
}



