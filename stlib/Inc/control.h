#pragma once
#include "error.h"

typedef struct {
    double accel_x;
    double accel_y;
    double accel_z;
    double temperature;
    double gyro_x;
    double gyro_y;
    double gyro_z;
} imu_data_t;

typedef struct {
    double ang_x;
    double ang_y;
    double ang_z;
    double vel_x;
    double vel_y;
    double vel_z;
    double pos_x;
    double pos_y;
    double pos_z;
} state_t;

error_t update_state(state_t* s, imu_data_t* imu_data, double delta_ticks);
