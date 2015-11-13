#include <rip_main.h>
#include <rip_proto.h>


void *rip_up(void *ptr) 
{
    route_entry_t re;
    int i;
    
    while (TRUE) {
	pthread_mutex_lock (&lock);
	if (!adtable.ready) {
	    goto end_loop;
	}
	printf ("rip_up(): Update from %s\n",adtable.neighbor->name);
	for (i = 0; adtable.neightable[i]; i++) {
	    printf ("\tDestination = %s Cost = %d\n", 
		    adtable.neightable[i]->destination->name,
		    adtable.neightable[i]->cost);
	    rip_obj_destroy_route_entry (adtable.neightable[i]);
	}
	printf ("End of update\n");
	memset (&adtable, 0, advert_entry_t_len);
	adtable.ready = FALSE;
	
    end_loop:
	pthread_mutex_unlock (&lock);
    }
}
