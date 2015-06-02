/**
 * \file sble_ll.h
 * \brief Double-Linked list
 *
 *  sble_ll.h implements a simple, general-purpose double-linked list. It is used for the reception queues.
 *
 *  \date 06.07.2012
 *  \author: kindt
 *  \example sble_example_ll.c
 */

#ifndef SBLE_LINKEDLIST_H_
#define SBLE_LINKEDLIST_H_
#ifdef __cplusplus
extern "C" {
#endif

#include <inttypes.h>
#include "sble_config.h"
#include "sble_debug.h"



/**
 * A function returning SBLE_TRUE if both elements are equal and SBLE_FALSE otherwise.
 */
typedef sble_bool (*sble_ll_comparator_fct)(void*, void*);


/**
 * \brief A node of the linked list.
 */
typedef struct _sble_ll_node{

	///Pointer to the payload, that can be of any type.
	void* data;

	///The previous node or NULL for list-root.
	struct _sble_ll_node* prev;

	///The next node or NULL for list-top.
	struct _sble_ll_node* next;
} sble_ll_node;

/**
 * \brief The structure representing a linked list.
 *
 */
typedef struct _sble_ll{
	///pointer to the root node
	struct _sble_ll_node* root;

	///pointer to the last element in list
	struct _sble_ll_node* last;

	///The number of elements in list
	sble_unsigned_integer nelements;
} sble_ll;



/**
 * Initialize a linked list
 * \param ll pointer to the linked list. The pointer must allready point to an alloc'ed linked list.
 */
void sble_ll_init(sble_ll* ll);


/**
 * Find the last node of a linked list by iterating from the root to the list's end.
 * Normally, this function need not be called. use the last-pointer in sble_ll.h first.
 * \param ll Pointer to the linked list.
 * \return Pointer to a sble_node or NULL in case of failure
 */
sble_ll_node* sble_ll_find_last_iterating(sble_ll* ll);


/** Check if linked list is empty.
 * \param ll Pointer to the linked list.
 * \return SBLE_TRUE if emtpy, SBLE_FALSE if not.
 */
sble_bool sble_ll_isempty(sble_ll* ll);

/** Push a value on top of the linked list.
 * \param ll Pointer to the linked list.
 * \param data Pointer to data to push onto linked list
 * \return SBLE_TRUE in case of success or SBLE_FALSE in case of failure
 */
sble_bool sble_ll_push(sble_ll* ll, void* data);

/** Push a value on top of the linked list and replace the old one, if the entry already exist.
 * \param ll Pointer to the linked list.
 * \param data Pointer to data to push onto linked list
 * \param isEqual Comparator function to check weather two elements are equal or not
 * \return SBLE_TRUE in case of success or SBLE_FALSE in case of failure
 */
sble_bool sble_ll_push_unique(sble_ll* ll, void* data, sble_ll_comparator_fct isEqual);

/**
 * Pop the element added most recently from the linked list
 * \param ll Pointer to the linked list.
 * \return NULL in case of failure, otherwise pointer to the payload of the element removed.
 */


/**
 * Pop the element added least recently
 * \param ll Pointer to the linked list.
 * \param index Index of element, counted from the list's bottom, to return its payload and remove.
 * \return NULL in case of failure, otherwise pointer to the payload of the element removed.
 */
void *sble_ll_pop_from_begin(sble_ll *ll);

/**
 * Pop the element added most recently
 * \param ll Pointer to the linked list.
 * \param index Index of element, counted from the list's bottom, to return its payload and remove.
 * \return NULL in case of failure, otherwise pointer to the payload of the element removed.
 */

void *sble_ll_pop(sble_ll *ll);
/**
 * Pop the element added most recently minus a given index
 * \param ll Pointer to the linked list.
 * \param index Index of element, counted from the list's bottom, to return its payload and remove.
 * \return NULL in case of failure, otherwise pointer to the payload of the element removed.
 */
void *sble_ll_pop_index_from_bottom(sble_ll *ll,sble_unsigned_integer index);

/** Returns a pointer to an element which es equal to another element according to a comparator function.
 *  Equality is defined by the comparator function. Thus, the elements can be scanned for a certain property.
 *  The element found will remain in list.
 * \param ll Pointer to the linked list.
 * \param data Pointer to data to compare the list-elements with
 * \param isEqual Comparator function to check weather two elements are equal or not
 * \return Pointer to a sble_node or NULL in case of failure
 */
sble_ll_node* sble_ll_get_element(sble_ll* ll, sble_ll_comparator_fct isEqual, void* data);


/** Returns a pointer to an element which es equal to another element according to a comparator function.
 *  Equality is defined by the comparator function. Thus, the elements can be scanned for a certain property.
 *  The element found will be removed from the list.
 * \param ll Pointer to the linked list.
 * \param data Pointer to data to compare the list-elements with
 * \param isEqual Comparator function to check weather two elements are equal or not
 * \return Pointer to a sble_node or NULL in case of failure
 */
void* sble_ll_get_and_remove_element(sble_ll* ll, sble_ll_comparator_fct isEqual, void* data);


/** Remove all elements that are equal to a given element according to a comparator function.
 *  Equality is defined by the comparator function. Thus, all elements having a certain property can be removed.
 * \param ll Pointer to the linked list.
 * \param data Pointer to data to compare the list-elements with
 * \param isEqual Comparator function to check weather two elements are equal or not
 */
void sble_ll_remove_all_equal_to(sble_ll* ll, sble_ll_comparator_fct isEqual, void* data);

/**
 * Free all nodes of a list.
 * \param ll Pointer to the linked list.
 * \param free_data if SBLE_TRUE, the elements of the list are free'ed to. Otherwise, only the sble_node-structures are freed.
 * All list pointers are adjusted accordingly.
 */
void sble_ll_free_nodes(sble_ll* ll,sble_bool free_data);

/** Return the number of elements in a list.
 * \param ll Pointer to the linked list.
 */
sble_unsigned_integer sble_ll_get_nr_of_elements(sble_ll* ll);

/**
 * Get next node in a linked list.
 * \param Node A node whose next node shall be determined
 * \return Pointer to the next node in list or NULL if no node could befound.
 */
sble_ll_node* sble_ll_get_next(sble_ll_node* node);

/**
 * Removes all elements from the linked list
 * \param ll Pointer to the linked list.
 */
void sble_ll_remove_all_elements(sble_ll* ll);

/**
 * Comparator function for signed integers.
 * \param *a pointer to signed integer
 * \param *b pointer to signed integer
 * \return SBLE_TRUE if (*a == *b), otherwise SBLE_FALSE
 */
sble_bool sble_comparator_signed_integer(sble_signed_integer *a, sble_signed_integer *b);

/**
 * Comparator function for unsigned integers.
 * \param *a pointer to unsigned integer
 * \param *b pointer to unsigned integer
 * \return SBLE_TRUE if (*a == *b), otherwise SBLE_FALSE
 */
sble_bool sble_comparator_unsigned_integer(sble_unsigned_integer *a, sble_unsigned_integer *b);




#ifdef __cplusplus
}
#endif

#endif /* SBLE_LINKEDLIST_H_ */
