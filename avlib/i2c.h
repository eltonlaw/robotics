#pragma once
#include <stdbool.h>
#include <avr/io.h>
#include <util/twi.h>

#define I2C_WRITE 0x00
#define I2C_READ 0x01

void i2c_init(unsigned long bus_speed);
void i2c_wait_for_complete(void);
bool i2c_start(void);
void i2c_stop(void);
uint8_t i2c_read_ack(void);
uint8_t i2c_read_nack(void);
void i2c_write(uint8_t data);
uint8_t i2c_get_status(void);
