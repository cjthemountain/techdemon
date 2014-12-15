#pragma once

#include "../../api/include/gestic_api.h"
#include "../../api/include/gestic_dynamic.h"
#include <stdio.h>
#include <curses.h>

#define FLICK_TIMEOUT 200

typedef struct 
{
    int running;
    int menu_current;
    int auto_calib;
    int last_gesture;
    int touch_detect;
    int air_wheel;

    gestic_t *gestic;
    gestic_position_t *gestic_pos;
    gestic_gesture_t *gestic_gesture;
    gestic_signal_t *gestic_sd;
    gestic_calib_t *gestic_calib;
    gestic_touch_t *gestic_touch;
    gestic_air_wheel_t *gestic_air_wheel;
} data_t;

void gotoxy(int x, int y);
void init_data(data_t *data);
void update_menu(data_t *data);

void init_device(data_t *data);
void free_device(data_t *data);
void update_device(data_t *data);

void switch_auto_calib(data_t *data);
void calibrate_now(data_t *data);

void switch_touch_detect(data_t *data);
void switch_air_wheel(data_t *data);
