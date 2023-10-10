#ifndef MPU6050_H_
#define MPU6050_H_

// setup functions
void MPU6050_init();
void MPU6050_calibration();
void MPU6050_config_gyro(uint8_t full_scale_range);
void MPU6050_config_accel(uint8_t full_scale_range);
void MPU6050_get_gyro(float dt);
void MPU6050_get_accel();
void MPU6050_get_complementary_angle(float dt, float alpha);

// print functions
void MPU6050_print_gyro_rawdata(float dt);
void MPU6050_print_gyro_velocity(float dt);
void MPU6050_print_gyro_angle(float dt);
void MPU6050_print_accel_rawdata();
void MPU6050_print_accel_angle();
void MPU6050_print_complementary_angle(float dt, float alpha);

#endif /* MPU6050_H_ */