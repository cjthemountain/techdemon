/******************************************************************************
 *
 * Copyright (C) 2014 Microchip Technology Inc. and its
 *                    subsidiaries ("Microchip").
 *
 * All rights reserved.
 *
 * You are permitted to use the Aurea software, GestIC API, and other
 * accompanying software with Microchip products.  Refer to the license
 * agreement accompanying this software, if any, for additional info regarding
 * your rights and obligations.
 *
 * SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
 * MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR
 * PURPOSE. IN NO EVENT SHALL MICROCHIP, SMSC, OR ITS LICENSORS BE LIABLE OR
 * OBLIGATED UNDER CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH
 * OF WARRANTY, OR OTHER LEGAL EQUITABLE THEORY FOR ANY DIRECT OR INDIRECT
 * DAMAGES OR EXPENSES INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL,
 * INDIRECT OR CONSEQUENTIAL DAMAGES, OR OTHER SIMILAR COSTS.
 *
 ******************************************************************************/
#include <stdlib.h>
#include "console.h"

/* Human readable strings for the gestures */
static const char *gestures[] = {
    "-                       ",
    "Flick West > East       ",
    "Flick East > West       ",
    "Flick South > North     ",
    "Flick North > South     ",
    "Circle clockwise        ",
    "Circle counter-clockwise"
};

void init_data(data_t *data)
{
    /* Render menu on startup. */
    data->menu_current = -1;

    /* Automatic calibration enabled by default */
    data->auto_calib = 1;

    /* No gestures so far */
    data->last_gesture = 0;

    /* No gestic_t instance allocated yet */
    data->gestic = NULL;
}

void init_device(data_t *data)
{
    /* Bitmask later used for starting a stream with
     * SD-, position- and gesture-data
     */
    const int stream_flags = gestic_data_mask_dsp_status |
            gestic_data_mask_gesture | gestic_data_mask_touch |
            gestic_data_mask_airwheel | gestic_data_mask_position |
            gestic_data_mask_sd;

    /* Allocate a new gestic_t-instance */
    data->gestic = gestic_create();

    /* Initialize the gestic_t-instance */
    gestic_initialize(data->gestic);

    /* Open a connection to the device */
    if(gestic_open(data->gestic) < 0) {
        fprintf(stdout, "Could not open connection to device.\n");
        exit(-1);
    }

    /* Get pointers to the result-buffers */
    data->gestic_pos = gestic_get_position(data->gestic, 0);
    data->gestic_sd = gestic_get_sd(data->gestic, 0);
    data->gestic_calib = gestic_get_calibration(data->gestic, 0);
    data->gestic_gesture = gestic_get_gesture(data->gestic, 0);
    data->gestic_touch = gestic_get_touch(data->gestic, 0);
    data->gestic_air_wheel = gestic_get_air_wheel(data->gestic, 0);

    /* Reset the device to the default state:
     * - Automatic calibration enabled
     * - All frequencies allowed
     * - Approach detection enabled for power saving
     * - Enable all gestures ( bit 1 to 6 set to 1 = 126 )
     */
    if(gestic_set_auto_calibration(data->gestic, data->auto_calib, 100) < 0 ||
       gestic_select_frequencies(data->gestic, gestic_all_freq, 100) < 0 ||
       gestic_set_approach_detection(data->gestic, 1, 100) < 0 ||
       gestic_set_enabled_gestures(data->gestic, 126, 100) < 0)
    {
        fprintf(stdout, "Could not reset device to default state.\n");
        exit(-1);
    }

    /* Set output-mask to the bitmask defined above and output only changes */
    if(gestic_set_output_enable_mask(data->gestic, stream_flags, 0,
                                     gestic_data_mask_all, 100) < 0)
    {
        fprintf(stdout, "Could not set data-output of device.\n");
        exit(-1);
    }
}

void free_device(data_t *data)
{
    /* Close the connection to the device */
    gestic_close(data->gestic);

    /* Release resources that were associated with the gestic_t-instance */
    gestic_cleanup(data->gestic);

    /* Release the gestic_t-instance itself */
    gestic_free(data->gestic);
}

void update_device(data_t *data)
{
    int i;

    /* Abbreviations for the electrodes */
    const char source[] = { 'S', 'W', 'N', 'E', 'C' };

    while(gestic_data_stream_update(data->gestic, NULL) == 0)
    {
        /* Print update of the position */
        if(data->gestic_pos != NULL &&  data->gestic_pos->x !=0 && 
					data->gestic_pos->y !=0 && 
					data->gestic_pos->z !=0 ) {
/*&& ( data->gestic_touch->flags & (1<<4) )*/ 
            fprintf(stdout, 
		     "%5d\t%5d\t%5d\n",
		     data->gestic_pos->x, data->gestic_pos->y,
                     data->gestic_pos->z);
        }
    }
}

void switch_auto_calib(data_t *data)
{
    /* Try to swith autocalibration with a timeout of 100 ms */
    int new_mode = !data->auto_calib;
    if(gestic_set_auto_calibration(data->gestic, new_mode, 100) != 0)
        return;
    data->auto_calib = new_mode;
}

void calibrate_now(data_t *data)
{
    /* Try to force calibration with a timeout of 100 ms */
    gestic_force_calibration(data->gestic, 100);
}

void switch_touch_detect(data_t *data) {
    int new_mode = !data->touch_detect;
    if(gestic_set_touch_detection(data->gestic, new_mode, 100) != 0)
        return;
    data->touch_detect = new_mode;
}

void switch_air_wheel(data_t *data) {
    int new_mode = !data->air_wheel;
    if(gestic_set_air_wheel_enabled(data->gestic, new_mode, 100) != 0)
        return;
    data->air_wheel = new_mode;
}
