#ifndef TWI_H_
#define TWI_H_

void I2C_init();
void I2C_start();
void I2C_repeated_start();
void I2C_transmit_address_writeMode(uint8_t address);
void I2C_transmit_address_readMode(uint8_t address);
void I2C_transmit_data(uint8_t data);
uint8_t I2C_receive_ACK();
uint8_t I2C_receive_NACK();
void I2C_stop();

#endif /* TWI_H_ */
