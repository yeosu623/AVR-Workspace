#ifndef TWI_H_
#define TWI_H_

void TWI_Init();
void TWI_Master_Write(unsigned char Data, unsigned char Addr);
unsigned char TWI_Master_Read(unsigned char Addr);
void TWI_Slave_Write(unsigned char Data, unsigned char Add);
unsigned char TWI_Slave_Read(unsigned char Add);

#endif /* TWI_H_ */