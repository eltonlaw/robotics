#include <stdint.h>
#include "error.h"
#include "i2c.h"
#include "mpu6050.h"
#include "log.h"

avcerr_t mpu6050_init(mpu6050_dev_t* mdev, mpu6050_gyro_range_t mgr,
        mpu6050_accel_range_t mar) {
    i2c_start();

    /* Disable sleep mode, returns 0's in sensor registers otherwise */
    i2c_write_sla_w(MPU6050_I2C_SLA_W);
    i2c_write_data(MPU6050_REG_PWR_MGMT);
    i2c_write_data(MPU6050_PWR_MGMT_OFF);

    /* Read in gyro config and accel config */
    i2c_start();
    i2c_write_sla_w(MPU6050_I2C_SLA_W);
    i2c_write_data(MPU6050_REG_GYRO_CONFIG);

    i2c_start();
    i2c_write_sla_r(MPU6050_I2C_SLA_R);
    // FIXME: On init these seem to be 0
    uint8_t gyro_config = i2c_read_ack();
    log_info("Read gyro config: %x", gyro_config);
    uint8_t accel_config = i2c_read_nack();
    log_info("Read accel config: %x", accel_config);
    /* set range on specific bits */
    gyro_config &= (((1 << 2) - 1) & mgr) << 3;
    accel_config &= (((1 << 2) - 1) & mar) << 3;

    /* write new gyro config */
    i2c_start();
    i2c_write_sla_w(MPU6050_I2C_SLA_W);
    i2c_write_data(MPU6050_REG_GYRO_CONFIG);
    i2c_write_data(gyro_config);
    switch(mgr) {
        case MPU6050_GYRO_RANGE_250:
            mdev->gyro_scaler = 131;
            break;
        case MPU6050_GYRO_RANGE_500:
            mdev->gyro_scaler = 65.5;
            break;
        case MPU6050_GYRO_RANGE_1000:
            mdev->gyro_scaler = 32.8;
            break;
        case MPU6050_GYRO_RANGE_2000:
            mdev->gyro_scaler = 16.4;
            break;
    }

    /* write new accel config */
    i2c_start();
    i2c_write_sla_w(MPU6050_I2C_SLA_W);
    i2c_write_data(MPU6050_REG_ACCEL_CONFIG);
    i2c_write_data(accel_config);
    switch(mar) {
        // Convert raw data from accelerometer into units of g. Raw data is
        // a 16 bit signed integer [-32768, 32767]
        case MPU6050_ACCEL_RANGE_2:
            // [-2g, +2g] 32768 / 2 = 16384
            mdev->accel_scaler = 16384;
            break;
        case MPU6050_ACCEL_RANGE_4:
            // [-4g, +4g] 32768 / 4 = 8192
            mdev->accel_scaler = 8192;
            break;
        case MPU6050_ACCEL_RANGE_8:
            // [-8g, +8g] 32768 / 8 = 4096
            mdev->accel_scaler = 4096;
            break;
        case MPU6050_ACCEL_RANGE_16:
            // [-16g, +16g] 32768 / 16 = 2048
            mdev->accel_scaler = 2048;
            break;
    }

    i2c_stop();
    return E_OK;
}

/* Read sensor data from mpu6050 */
avcerr_t mpu6050_read(mpu6050_dev_t* mdev) {
    i2c_start();
    // send 7 bit i2c address of mpu6050 + write bit on bus
    // wait for ack from mpu6050
    i2c_write_sla_w(MPU6050_I2C_SLA_W);
    // put register address on the bus
    // wait for ack from mpu6050
    i2c_write_data(MPU6050_REG_ACCEL_XOUT_H);
    // send start condition
    i2c_start();
    // send 7 bit i2c address of mpu6050 + read bit on bus
    i2c_write_sla_r(MPU6050_I2C_SLA_R);

    // burst read, keep sending acks, MPU6050 will
    // auto-inc address being read.
    mdev->reg->accel_x = (i2c_read_ack() << 8) | i2c_read_ack();
    mdev->reg->accel_y = (i2c_read_ack() << 8) | i2c_read_ack();
    mdev->reg->accel_z = (i2c_read_ack() << 8) | i2c_read_ack();
    mdev->reg->temp = (i2c_read_ack() << 8) | i2c_read_ack();
    mdev->reg->gyro_x = (i2c_read_ack() << 8) | i2c_read_ack();
    mdev->reg->gyro_y = (i2c_read_ack() << 8) | i2c_read_ack();
    mdev->reg->gyro_z = (i2c_read_ack() << 8) | i2c_read_nack();
    // Send stop condition
    i2c_stop();
    return E_OK;
}
