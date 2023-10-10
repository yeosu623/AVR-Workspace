#ifndef GPIOTOPWM_H_
#define GPIOTOPWM_H_

void timer0_setting();
void GPIOtoPWMInit();
void GPIOtoPWM(uint8_t PORTx, uint8_t PINx, uint8_t duty, uint8_t inverse, uint16_t freq);
void SET_FREQ(uint8_t PORTx, uint8_t PINx, uint16_t freq);
void SET_DUTY(uint8_t PORTx, uint8_t PINx, uint8_t duty);

#endif /* GPIOTOPWM_H_ */