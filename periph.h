#ifndef PERIPH_H
#define PERIPH_H
#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "driverlib/ssi.h"


/* ================== Pins (TM4C123) ================== */
#define TFT_CS_PORT   GPIO_PORTA_BASE
#define TFT_CS_PIN    4   // PA4

#define TFT_DC_PORT   GPIO_PORTB_BASE
#define TFT_DC_PIN    0   // PB0

#define TFT_RST_PORT  GPIO_PORTB_BASE
#define TFT_RST_PIN   1   // PB1

#define TFT_BL_PORT   GPIO_PORTB_BASE
#define TFT_BL_PIN    2   // PB2

#define TFT_W 240
#define TFT_H 240

void pin_high_A(uint32_t n);
void pin_low_A (uint32_t n);
void pin_high_B(uint32_t n);
void pin_low_B (uint32_t n);

void CS_LOW(void);
void CS_HIGH(void);
void DC_LOW(void);
void DC_HIGH(void);
void RST_LOW(void);
void RST_HIGH(void);
void BL_ON(void);

/* ===== SysTick delay (1ms @ 16 MHz) ===== */
void SysTick_Handler(void);
void SysTick_Init(void);

void delay_ms(uint32_t ms);
uint32_t millis(void);

/* ===== SPI1 low-level ===== */
void ssi3_wait_tnf(void);
void ssi3_wait_not_busy(void);

void ssi3_write8(uint8_t b);

void ssi3_write_buf(const uint8_t* d, uint32_t n);

void ssi3_init_mode3_txonly_4mhz(void);
void gpio_init(void);
void clock_init_16mhz_piosc(void);

#endif
