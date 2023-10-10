// includes
#include <avr/io.h>
#include <stdio.h>
#include <math.h>
#include "TWI.h"
#include "MPU6050.h"

// registers
#define MPU6050_ADDRESS 0x68

#define ACCEL_XOUT_H 0x3B
#define ACCEL_XOUT_L 0x3C
#define ACCEL_YOUT_H 0x3D
#define ACCEL_YOUT_L 0x3E
#define ACCEL_ZOUT_H 0x3F
#define ACCEL_ZOUT_L 0x40
#define TEMP_OUT_H 0x41
#define TEMP_OUT_L 0x42
#define GYRO_XOUT_H 0x43
#define GYRO_XOUT_L 0x44
#define GYRO_YOUT_H 0x45
#define GYRO_YOUT_L 0x46
#define GYRO_ZOUT_H 0x47
#define GYRO_ZOUT_L 0x48

#define POWER_MANAGEMENT1 0x6B
#define CONFIG 0x1A
#define Gyroscope_Configuration 0x1B
#define Accelerometer_Configuration 0x1C

// variables
struct values {
	short xa_error;
	short ya_error;
	short za_error;
	short xg_error;
	short yg_error;
	short zg_error;
} values = {0, 0, 0, 0, 0, 0};
	
short temp1, temp2, temp3;
short gyro_x, gyro_y, gyro_z;
float gyro_x_velocity, gyro_y_velocity, gyro_z_velocity;
float gyro_x_angle = 0, gyro_y_angle = 0, gyro_z_angle = 0;
short accel_x, accel_y, accel_z;
float accel_x_angle = 0, accel_y_angle = 0, accel_z_angle = 0;
float complementary_x_angle, complementary_y_angle, complementary_z_angle;
float FS_SEL;
short AFS_SEL;
float RADIAN_TO_DEGREES = 180 / 3.141592;

// functions
void I2C_write(uint8_t register_address, uint8_t data)
{
	I2C_start();
	I2C_transmit_address_writeMode(MPU6050_ADDRESS << 1);
	I2C_transmit_data(register_address);
	I2C_transmit_data(data);
	I2C_stop();
}
uint8_t I2C_read(uint8_t register_address)
{
	uint8_t data;
	
	I2C_start();
	I2C_transmit_address_writeMode(MPU6050_ADDRESS << 1);
	I2C_transmit_data(register_address);
	I2C_repeated_start();
	I2C_transmit_address_readMode( (MPU6050_ADDRESS << 1) + 1);
	data =I2C_receive_NACK();
	I2C_stop();
	
	return data;
}

void MPU6050_init()
{
	I2C_write(POWER_MANAGEMENT1, 0x00); // Sleep Mode Off
	I2C_write(CONFIG, 0x05); // Digital Low Pass Filter = 10Hz
	
	printf("MPU-6050 initialization completed!\n\n");
}

void MPU6050_calibration()
{
	long xg_error_sum = 0;
	long yg_error_sum = 0;
	long zg_error_sum = 0;
	long xa_error_sum = 0;
	long ya_error_sum = 0;
	long za_error_sum = 0;
	
	// 평균값으로 초기값을 구한다.
	for(int i=0; i<10; i++)
	{
		MPU6050_get_gyro_error();
		xg_error_sum += temp1;
		yg_error_sum += temp2;
		zg_error_sum += temp3;
		MPU6050_get_accel_error();
		xa_error_sum += temp1;
		ya_error_sum += temp2;
		za_error_sum += temp3;
	}
	
	values.xg_error = xg_error_sum / 10;
	values.yg_error = yg_error_sum / 10;
	values.zg_error = zg_error_sum / 10;
	values.xa_error = xa_error_sum / 10;
	values.ya_error = ya_error_sum / 10;
	values.za_error = (za_error_sum / 10) - AFS_SEL;
	
	printf("MPU-6050 calibration completed!\n\n");
}

void MPU6050_config_gyro(uint8_t full_scale_range)
{
	/*
	parameters
	0x00 : Full Scale Range = +-250 degree/s
	0x08 : Full Scale Range = +-500 degree/s
	0x10 : Full Scale Range = +-1000 degree/s
	0x18 : Full Scale Range = +-2000 degree/s
	*/
	I2C_write(Gyroscope_Configuration, full_scale_range);
	
	switch(full_scale_range)
	{
		case 0x00:
			FS_SEL = 131;
			break;
		case 0x08:
			FS_SEL = 65.5;
			break;
		case 0x10:
			FS_SEL = 32.8;
			break;
		case 0x18:
			FS_SEL = 16.4;
			break;
		default:
			printf("something goes wrong... exit program.\n\n");
			while(1);
	}
	
	printf("MPU-6050 gyro full-scale-range set!\n\n");
}

void MPU6050_config_accel(uint8_t full_scale_range)
{
	/*
	parameters
	0x00 : Full Scale Range = +-2g (g = gravity accel.)
	0x08 : Full Scale Range = +-4g
	0x10 : Full Scale Range = +-8g
	0x18 : Full Scale Range = +-16g
	*/
	I2C_write(Accelerometer_Configuration, full_scale_range);
	
	switch(full_scale_range)
	{
		case 0x00:
			AFS_SEL = 16384;
			break;
		case 0x08:
			AFS_SEL = 8192;
			break;
		case 0x10:
			AFS_SEL = 4096;
			break;
		case 0x18:
			AFS_SEL = 2048;
			break;
		default:
			printf("something goes wrong... exit program.\n\n");
			while(1);
	}
	
	printf("MPU-6050 accel full-scale-range set!\n\n");
}

void MPU6050_get_gyro(float dt)
{
	gyro_x = (I2C_read(GYRO_XOUT_H) << 8) + I2C_read(GYRO_XOUT_L);
	gyro_y = (I2C_read(GYRO_YOUT_H) << 8) + I2C_read(GYRO_YOUT_L);
	gyro_z = (I2C_read(GYRO_ZOUT_H) << 8) + I2C_read(GYRO_ZOUT_L);
	
	// calculate error
	gyro_x = gyro_x - values.xg_error;
	gyro_y = gyro_y - values.yg_error;
	gyro_z = gyro_z - values.zg_error;
	
	// Convert gyro values to gyro velocity
	gyro_x_velocity = gyro_x / FS_SEL;
	gyro_y_velocity = gyro_y / FS_SEL;
	gyro_z_velocity = gyro_z / FS_SEL;
	
	// Convert gyro velocity to gyro angles
	gyro_x_angle += gyro_x_velocity * dt;
	gyro_y_angle += gyro_y_velocity * dt;
	gyro_z_angle += gyro_z_velocity * dt;
}

void MPU6050_get_gyro_error()
{
	temp1 = (I2C_read(GYRO_XOUT_H) << 8) + I2C_read(GYRO_XOUT_L);
	temp2 = (I2C_read(GYRO_YOUT_H) << 8) + I2C_read(GYRO_YOUT_L);
	temp3 = (I2C_read(GYRO_ZOUT_H) << 8) + I2C_read(GYRO_ZOUT_L);
}

void MPU6050_get_accel()
{
	accel_x = (I2C_read(ACCEL_XOUT_H) << 8) + I2C_read(ACCEL_XOUT_L);
	accel_y = (I2C_read(ACCEL_YOUT_H) << 8) + I2C_read(ACCEL_YOUT_L);
	accel_z = (I2C_read(ACCEL_ZOUT_H) << 8) + I2C_read(ACCEL_ZOUT_L);
	
	// calculate errors
	accel_x = accel_x - values.xa_error;
	accel_y = accel_y - values.ya_error;
	accel_z = accel_z - values.za_error;
	
	// calculate angle from accelerometer
	accel_y_angle = atan(-1 * accel_x / sqrt(pow(accel_y, 2) + pow(accel_z, 2))) * RADIAN_TO_DEGREES;
	accel_x_angle = atan(     accel_y / accel_z ) * RADIAN_TO_DEGREES;
	accel_z_angle = 0; // MPU-6050 can't calculate yaw angle.
}

void MPU6050_get_accel_error()
{
	temp1 = (I2C_read(ACCEL_XOUT_H) << 8) + I2C_read(ACCEL_XOUT_L);
	temp2 = (I2C_read(ACCEL_YOUT_H) << 8) + I2C_read(ACCEL_YOUT_L);
	temp3 = (I2C_read(ACCEL_ZOUT_H) << 8) + I2C_read(ACCEL_ZOUT_L);
}

void MPU6050_get_complementary_angle(float dt, float alpha)
{
	// 일반적으로 alpha는 0.96을 많이 사용한다.
	
	MPU6050_get_gyro(dt);
	MPU6050_get_accel();
	
	complementary_x_angle = alpha*gyro_x_angle + (1.0-alpha)*accel_x_angle;
	complementary_y_angle = alpha*gyro_y_angle + (1.0-alpha)*accel_y_angle;
	complementary_z_angle = gyro_z_angle; // MPU6050 doesn't give accel_z_angle.
}

void MPU6050_print_gyro_rawdata(float dt)
{
	MPU6050_get_gyro(dt);
	printf("%d\t", gyro_x);
	printf("%d\t", gyro_y);
	printf("%d\n", gyro_z);
}

void MPU6050_print_gyro_velocity(float dt)
{
	MPU6050_get_gyro(dt);
	printf("%f\t", gyro_x_velocity);
	printf("%f\t", gyro_y_velocity);
	printf("%f\n", gyro_z_velocity);
}

void MPU6050_print_gyro_angle(float dt)
{
	MPU6050_get_gyro(dt);
	printf("%f\t", gyro_x_angle);
	printf("%f\t", gyro_y_angle);
	printf("%f\n", gyro_z_angle);
}

void MPU6050_print_accel_rawdata()
{
	MPU6050_get_accel();
	printf("%d\t", accel_x);
	printf("%d\t", accel_y);
	printf("%d\n", accel_z);
}

void MPU6050_print_accel_angle()
{
	MPU6050_get_accel();
	printf("%f\t", accel_x_angle);
	printf("%f\t", accel_y_angle);
	printf("%f\n", accel_z_angle);
}

void MPU6050_print_complementary_angle(float dt, float alpha)
{
	MPU6050_get_complementary_angle(dt, alpha);
	printf("%f\t", complementary_x_angle);
	printf("%f\t", complementary_y_angle);
	printf("%f\n", complementary_z_angle);
}