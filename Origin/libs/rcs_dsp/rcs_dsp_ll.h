/**
 * \file rcs_dsp_ll.h
 * \brief Double-Linked list
 *
 *  This file implements a simple, general-purpose double-linked list.
 *
 *  \date 06.07.2012
 *  \author: kindt
 */

#ifndef RCS_DSPLINKEDLIST_H_
#define RCS_DSP_LINKEDLIST_H_
#ifdef __cplusplus
extern "C" {
#endif

#include <inttypes.h>
#include "rcs_dsp_cfg.h"
#include "rcs_dsp_debug.h"

#ifndef NULL
#define NULL ((void*) 0)
#endif
/**
 * A function returning rcs_dsp_TRUE if both elements are equal and rcs_dsp_FALSE otherwise.
 */
typedef rcs_dsp_bool (*rcs_dsp_ll_comparator_fct)(void*, void*);


/**
 * \brief A node of the linked list.
 */
typedef struct _rcs_dsp_ll_node{

	///Pointer to the payload, that can be of any type.
	void* data;

	///The previous node or NULL for list-root.
	struct _rcs_dsp_ll_node* prev;

	///The next node or NULL for list-top.
	struct _rcs_dsp_ll_node* next;
} rcs_dsp_ll_node;

/**
 * \brief The structure representing a linked list.
 *
 */
typedef struct _rcs_dsp_ll{
	///pointer to the root node
	struct _rcs_dsp_ll_node* root;

	///pointer to the last element in list
	struct _rcs_dsp_ll_node* last;

	///The number of elements in list
	rcs_dsp_unsigned_integer nelements;
} rcs_dsp_ll;



/**
 * Initialize a linked list
 * \param ll pointer to the linked list. The pointer must allready point to an alloc'ed linked list.
 */
void rcs_dsp_ll_init(rcs_dsp_ll* ll);


/**
 * Find the last node of a linked list by iterating from the root to the list's end.
 * Normally, this function need not be called. use the last-pointer in rcs_dsp_ll.h first.
 * \param ll Pointer to the linked list.
 * \return Pointer to a rcs_dsp_node or NULL in case of failure
 */
rcs_dsp_ll_node* rcs_dsp_ll_find_last_iterating(rcs_dsp_ll* ll);


/** Check if linked list is empty.
 * \param ll Pointer to the linked list.
 * \return rcs_dsp_TRUE if emtpy, rcs_dsp_FALSE if not.
 */
rcs_dsp_bool rcs_dsp_ll_isempty(rcs_dsp_ll* ll);

/** Push a value on top of the linked list.
 * \param ll Pointer to the linked list.
 * \param data Pointer to data to push onto linked list
 * \return rcs_dsp_TRUE in case of success or rcs_dsp_FALSE in case of failure
 */
rcs_dsp_bool rcs_dsp_ll_push(rcs_dsp_ll* ll, void* data);

/** Push a value on top of the linked list and replace the old one, if the entry already exist.
 * \param ll Pointer to the linked list.
 * \param data Pointer to data to push onto linked list
 * \param isEqual Comparator function to check weather two elements are equal or not
 * \return rcs_dsp_TRUE in case of success or rcs_dsp_FALSE in case of failure
 */
rcs_dsp_bool rcs_dsp_ll_push_unique(rcs_dsp_ll* ll, void* data, rcs_dsp_ll_comparator_fct isEqual);

/**
 * Pop the element added most recently from the linked list
 * \param ll Pointer to the linked list.
 * \return NULL in case of failure, otherwise pointer to the payload of the element removed.
 */
void *rcs_dsp_ll_pop(rcs_dsp_ll *ll);

/**
 * Pop the element added most recently minus a given number of list elements from the linked list
 * \param ll Pointer to the linked list.
 * \param index Index of element, counted from the list's bottom, to return its payload and remove.
 * \return NULL in case of failure, otherwise pointer to the payload of the element removed.
 */
void *rcs_dsp_ll_pop_index_from_bottom(rcs_dsp_ll *ll,rcs_dsp_unsigned_integer index);

/** Returns a pointer to an element which es equal to another element according to a comparator function.
 *  Equality is defined by the comparator function. Thus, the elements can be scanned for a certain property.
 *  The element found will remain in list.
 * \param ll Pointer to the linked list.
 * \param data Pointer to data to compare the list-elements with
 * \param isEqual Comparator function to check weather two elements are equal or not
 * \return Pointer to a rcs_dsp_node or NULL in case of failure
 */
rcs_dsp_ll_node* rcs_dsp_ll_get_element(rcs_dsp_ll* ll, rcs_dsp_ll_comparator_fct isEqual, void* data);


/** Returns a pointer to an element which es equal to another element according to a comparator function.
 *  Equality is defined by the comparator function. Thus, the elements can be scanned for a certain property.
 *  The element found will be removed from the list.
 * \param ll Pointer to the linked list.
 * \param data Pointer to data to compare the list-elements with
 * \param isEqual Comparator function to check weather two elements are equal or not
 * \return Pointer to a rcs_dsp_node or NULL in case of failure
 */
void* rcs_dsp_ll_get_and_remove_element(rcs_dsp_ll* ll, rcs_dsp_ll_comparator_fct isEqual, void* data);


/** Remove all elements that are equal to a given element according to a comparator function.
 *  Equality is defined by the comparator function. Thus, all elements having a certain property can be removed.
 * \param ll Pointer to the linked list.
 * \param data Pointer to data to compare the list-elements with
 * \param isEqual Comparator function to check weather two elements are equal or not
 */
void rcs_dsp_ll_remove_all_equal_to(rcs_dsp_ll* ll, rcs_dsp_ll_comparator_fct isEqual, void* data);

/**
 * Free all nodes of a list.
 * \param ll Pointer to the linked list.
 * \param free_data if rcs_dsp_TRUE, the elements of the list are free'ed to. Otherwise, only the rcs_dsp_node-structures are freed.
 * All list pointers are adjusted accordingly.
 */
void rcs_dsp_ll_free_nodes(rcs_dsp_ll* ll,rcs_dsp_bool free_data);

/** Return the number of elements in a list.
 * \param ll Pointer to the linked list.
 */
rcs_dsp_unsigned_integer rcs_dsp_ll_get_nr_of_elements(rcs_dsp_ll* ll);

/**
 * Get next node in a linked list.
 * \param Node A node whose next node shall be determined
 * \return Pointer to the next node in list or NULL if no node could befound.
 */
rcs_dsp_ll_node* rcs_dsp_ll_get_next(rcs_dsp_ll_node* node);


/**
 * Comparator function for signed integers.
 * \param *a pointer to signed integer
 * \param *b pointer to signed integer
 * \return rcs_dsp_TRUE if (*a == *b), otherwise rcs_dsp_FALSE
 */
rcs_dsp_bool rcs_dsp_comparator_signed_integer(rcs_dsp_signed_integer *a, rcs_dsp_signed_integer *b);

/**
 * Comparator function for unsigned integers.
 * \param *a pointer to unsigned integer
 * \param *b pointer to unsigned integer
 * \return rcs_dsp_TRUE if (*a == *b), otherwise rcs_dsp_FALSE
 */
rcs_dsp_bool rcs_dsp_comparator_unsigned_integer(rcs_dsp_unsigned_integer *a, rcs_dsp_unsigned_integer *b);
#ifdef __cplusplus
}
#endif

#endif /* RCS_DSPLINKEDLIST_H_ */
