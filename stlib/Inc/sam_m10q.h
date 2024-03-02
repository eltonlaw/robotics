#pragma once

#include "stm32g0xx.h"
#include "error.h"

#define SAM_M10Q_ADDR (0x42 << 1)

typedef struct {
    I2C_HandleTypeDef* i2cx;
} sam_m10q_cfg_t;

typedef struct {
    uint8_t status;
} sam_m10q_data_t;

error_t sam_m10q_init(sam_m10q_cfg_t* cfg, I2C_HandleTypeDef* i2cx);
error_t sam_m10q_read(sam_m10q_cfg_t* cfg, sam_m10q_data_t *data);
