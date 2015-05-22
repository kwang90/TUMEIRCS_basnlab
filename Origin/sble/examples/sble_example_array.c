#include "sble_array.h"


int main(){
	sble_array* arr;
	sble_array_malloc_whole(&arr,5);			//allocate space for 5 bytes
	 arr->data[0] = 24;
	 arr->data[1] = 12;
	 arr->data[2] = 15;					//... some payload
	 sble_array_free_whole(&arr);

	 sble_array arr_allready_malloced;		//this structure is allready in memory => just allocate the payload

	 sble_array_malloc_data(&arr_allready_malloced,5);
	 //do something nifty with the data
	 sble_array_free_data(&arr_allready_malloced);

	 return 0;
}
