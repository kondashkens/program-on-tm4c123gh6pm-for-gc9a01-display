#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "driverlib/ssi.h"
#include "periph.h"

#include "GC9A01.h"
#include "fonts.h"




static inline void put2(char* p, uint32_t v);
extern FontDef_t Font_11x18;
/* ================== main ================== */
int main(void){
  clock_init_16mhz_piosc();

  SysTick_Init();
  gpio_init();
  ssi3_init_mode3_txonly_4mhz();

  GC9A01_Init();
    GC9A01_FillScreen(0xF800); delay_ms(300);
    GC9A01_FillScreen(0x07E0); delay_ms(300);
    GC9A01_FillScreen(0x001F); delay_ms(300);
    GC9A01_FillScreen(0x0000);

      uint32_t last = 0;
    char t[9] = "00:00:00";

    while(1){
        //Only color
      /*  GC9A01_FillScreen(0xF800); delay_ms(1000);
          GC9A01_FillScreen(0x07E0); delay_ms(1000);
          GC9A01_FillScreen(0x001F); delay_ms(1000);
          GC9A01_FillScreen(0x0000);  delay_ms(1000);*/

        //Time
        uint32_t now = millis();

          if ((now - last) >= 1000U)
          {
            last += 1000U;

            uint32_t sec = now / 1000U;
            uint32_t hh  = (sec / 3600U) % 100U;
            uint32_t mm  = (sec % 3600U) / 60U;
            uint32_t ss  =  sec % 60U;

            put2(&t[0], hh);
            put2(&t[3], mm);
            put2(&t[6], ss);

            //GC9A01_print(10, 10, GC9A01_WHITE, GC9A01_BLACK, 1, &Font11x18, 2, t);
                  GC9A01_print(40, 100, GC9A01_WHITE, GC9A01_BLACK, 1, &Font_11x18, 2, t);

          }
      }
}

static inline void put2(char* p, uint32_t v) {
  p[0] = (char)('0' + ((v / 10U) % 10U));
  p[1] = (char)('0' + (v % 10U));
}

