#include "periph.h"

void pin_high_A(uint32_t n){ GPIO_PORTA_DATA_R |=  (1u<<n); }
void pin_low_A (uint32_t n){ GPIO_PORTA_DATA_R &= ~(1u<<n); }
void pin_high_B(uint32_t n){ GPIO_PORTB_DATA_R |=  (1u<<n); }
void pin_low_B (uint32_t n){ GPIO_PORTB_DATA_R &= ~(1u<<n); }

void CS_LOW(void)   { pin_low_A (TFT_CS_PIN); }
void CS_HIGH(void)  { pin_high_A(TFT_CS_PIN); }
void DC_LOW(void)   { pin_low_B (TFT_DC_PIN); }
void DC_HIGH(void)  { pin_high_B(TFT_DC_PIN); }
void RST_LOW(void)  { pin_low_B (TFT_RST_PIN); }
void RST_HIGH(void) { pin_high_B(TFT_RST_PIN); }
void BL_ON(void)    { pin_high_B(TFT_BL_PIN); }


/* ===== SysTick delay (1ms @ 16 MHz) ===== */
static volatile uint32_t g_ms = 0;
void SysTick_Handler(void){ g_ms++; }
void SysTick_Init(void){
    NVIC_ST_RELOAD_R = 16000 - 1;
    NVIC_ST_CURRENT_R = 0;
    NVIC_ST_CTRL_R = (1u<<2) | (1u<<1) | (1u<<0); // CLKSRC=1, INTEN=1, ENABLE=1
}

void delay_ms(uint32_t ms){
    uint32_t t = g_ms;
    while((g_ms - t) < ms) { __asm(" NOP"); }
}

uint32_t millis(void) {
  return g_ms;
}

/* ===== SPI3 low-level ===== */
void ssi3_wait_tnf(void){ while((SSI3_SR_R & (1u<<1)) == 0){} } // TNF=bit1
void ssi3_wait_not_busy(void){ while(SSI3_SR_R & (1u<<4)){} }  // BSY=bit4

void ssi3_write8(uint8_t b){
    ssi3_wait_tnf();
    SSI3_DR_R = b;
    ssi3_wait_not_busy();
}

void ssi3_write_buf(const uint8_t* d, uint32_t n){
    while(n--){
      ssi3_wait_tnf();
      SSI3_DR_R = *d++;
    }
    ssi3_wait_not_busy();
}


/* ===== Clock/GPIO/SPI init ===== */
void clock_init_16mhz_piosc(void){
  // �� ��������� ����� reset ������ PIOSC=16MHz, �� ��������� ����:
  SYSCTL_RCC_R &= ~SYSCTL_RCC_USESYSDIV;
  // �� ������� PLL � ��������� 16 MHz.
}

void gpio_init(void){
  // Clocks: GPIOA, GPIOB, GPIOD, SSI3
  SYSCTL_RCGCGPIO_R |= (1u<<0) | (1u<<1) | (1u<<3); // A,B,D
  SYSCTL_RCGCSSI_R  |= (1u<<3);                     // SSI3
  (void)SYSCTL_RCGCGPIO_R; // dummy read for delay
  (void)SYSCTL_RCGCSSI_R;

  /* ---- Port D: PD0=SSI3CLK, PD3=SSI3TX ---- */
  // digital enable
  GPIO_PORTD_DEN_R |= (1u<<0) | (1u<<3);
  // AFSEL
  GPIO_PORTD_AFSEL_R |= (1u<<0) | (1u<<3);
  // PCTL: PD0 SSI3CLK (0x1), PD3 SSI3TX (0x1)
  GPIO_PORTD_PCTL_R &= ~((0xFu<<(0*4)) | (0xFu<<(3*4)));
  GPIO_PORTD_PCTL_R |=  ((0x1u<<(0*4)) | (0x1u<<(3*4)));
  // DIR: PD0/PD3 outputs (for SSI alt func it is ok)
  GPIO_PORTD_DIR_R |= (1u<<0) | (1u<<3);

  /* ---- CS on PA4 ---- */
  GPIO_PORTA_DEN_R |= (1u<<4);
  GPIO_PORTA_DIR_R |= (1u<<4);
  CS_HIGH();

  /* ---- DC/RST/BL on PB0/PB1/PB2 ---- */
  GPIO_PORTB_DEN_R |= (1u<<0) | (1u<<1) | (1u<<2);
  GPIO_PORTB_DIR_R |= (1u<<0) | (1u<<1) | (1u<<2);
  DC_HIGH();
  RST_HIGH();
  BL_ON();
}

void ssi3_init_mode3_txonly_4mhz(void){
  // Disable SSI3
  SSI3_CR1_R &= ~SSI_CR1_SSE;

  // Master mode
  SSI3_CR1_R = 0;

  // CR0:
  // DSS=0x7 (8-bit), FRF=0 (SPI), SPO=1, SPH=1, SCR=1  => Mode3, 4MHz with CPSDVSR=2
  SSI3_CR0_R =
      (1u<<SSI_CR0_SCR_S) |           // SCR=1
      SSI_CR0_SPO | SSI_CR0_SPH |     // CPOL=1, CPHA=1
      (0x7u);                         // DSS=7 => 8-bit

  // SSIClk = SysClk / (CPSDVSR * (1+SCR)) = 16MHz / (2 * 2) = 4MHz
  SSI3_CPSR_R = 2;

  // Enable SSI3
  SSI3_CR1_R |= SSI_CR1_SSE;
}
