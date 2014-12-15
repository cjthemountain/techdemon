#include "../captouch/mgc3130/gestic.c"
#include "../bridge/sub20/lib/libsub.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>



//Global Variables
sub_handle *fd = NULL;


int updatesensors(data_t *data, int *skipped){
	//update temperature sensors

	//update capacitive touch sensors
	int errorcode;

	errorcode = gestic_data_stream_update(data->gestic, skipped);
	if (!skipped) {
		fprintf(stdout, "skipped is NULL inside sensor update\n");
		return 1;
	} else if (errorcode != 0) {
		fprintf(stdout, "gestic_data_stream_update returned ERROR: %d\n", errorcode);
		return 1;
	}
	
	return 0;
}

int actuate(){
	//control hardware output (fans, TECs, etc)

	return 0;
}


int updatetimer(struct timespec *time){
	time[0] = time[1];
	return clock_gettime(CLOCK_REALTIME,&time[1]);
}

int failsafe(){ 	
	fprintf(stdout, "beginning emergency exit procedures...\n");
	//set TEC controllers output to 0
	
	//set fan controllers output to max

	fprintf(stdout, "finished\nexiting");
	exit(-1);
	return 0;
}

int show(struct timespec *times, data_t *data, int *skipped){
	//print loop data
	fprintf(stdout,"X:\t\t%5d\nY:\t\t%5d\nZ:\t\t%5d\n",data->gestic_pos->x, data->gestic_pos->y,
		data->gestic_pos->z);
	fprintf(stdout, "loop time:\t%llu nanoseconds\n",1000000000*(times[1].tv_sec - times[0].tv_sec) + 
								(times[1].tv_nsec - times[0].tv_nsec));	
	if (!skipped) {
		fprintf(stdout, "skipped is NULL inside show\n");
		return 1;
	} else {
		fprintf(stdout, "skipped:\t%d messages from mgc3130\n", *skipped); 
	}
	return 0;
}

int main(){
	//generic declarations
	int i;

	//mgc declarations
	data_t mgc;
	int skipped=0;
	struct timespec times[2];

	//sub20 declarations
	sub_int32_t tmp1, tmp2;
	struct usb_device *dev;
	int rc;

	//init mgc3130
	init_data(&mgc); 
	init_device(&mgc);
	
	//init sub20
	dev = NULL;
        char sn_buf[20];
        int found = 0 ;
        while( ( dev =  sub_find_devices( dev ) ) ){
		fd = sub_open( dev );
		if(!fd){
                	printf("ERROR: Cannot open handle to device\n" );
                        return -1;
		}
            	rc = sub_get_serial_number( fd, sn_buf, sizeof(sn_buf) );
                sub_close( fd );
                if(rc<0){
                	printf("ERROR: Cannot obtain seral number: %d\n", rc );
                        return -1;
		}
                if( config.serno == strtoul(sn_buf, NULL, 16 ) ){
                	/* device found */
                    	found = 1;
			break;
                }
	}

	if( !found ){
        	printf("ERROR: Device with SN=0x%04x not found\n", config.serno );
                return -1;
	}

	//init timer
	clock_gettime(CLOCK_REALTIME,&times[1]);

	//main loop
	fprintf(stdout, "entering main loop\n");
	while (1) { 
		if (updatesensors(&mgc, &skipped)<0){
			failsafe();
		}
		if (actuate()!=0) {
			failsafe();
		}
		if (updatetimer(times)!=0) {
			fprintf(stdout, "timer update failed, which is really wierd\n");
			failsafe();
		}
		if (show(times, &mgc, &skipped)<0) {
			failsafe();
		}	
	
		fprintf(stdout, "\n");
	}
}

