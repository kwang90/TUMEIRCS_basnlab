#include "sble_ll.h"
#include "sble_debug.h"

int main(){
	//create linked list
	sble_ll ll;
	sble_ll_init(&ll);

	int a,b,c,d;
	a = 1;
	b = 2;
	c = 3;
	d = 4;
	
	//fill list
	sble_ll_push(&ll,&a);
	sble_ll_push(&ll,&b);
	sble_ll_push(&ll,&c);
	sble_ll_push(&ll,&d);
	

	//... and pop values from list
	int* value;
	while(value = sble_ll_pop(&ll)){
		SBLE_DEBUG("Pop: %d",*value);	
	}
	return 0;
}
