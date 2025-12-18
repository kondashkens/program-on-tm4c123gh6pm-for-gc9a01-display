/*

  ******************************************************************************
  * @file           ( С„Р°РёР» ):           GC9A01.c
  * @brief      ( РѕРїРёСЃР°РЅРёРµ ):
  ******************************************************************************
  * @attention  ( РІРЅРёРјР°РЅРёРµ ):       author: Golinskiy Konstantin    e-mail: golinskiy.konstantin@gmail.com
  ******************************************************************************

*/

#include "GC9A01.h"


uint16_t GC9A01_X_Start = GC9A01_XSTART;
uint16_t GC9A01_Y_Start = GC9A01_YSTART;

uint16_t GC9A01_Width, GC9A01_Height;

#if FRAME_BUFFER
// РјР°СЃСЃРёРІ Р±СѓС„РµСЂ РєР°РґСЂР°
    uint16_t buff_frame[GC9A01_WIDTH*GC9A01_HEIGHT] = { 0x0000, };
#endif


//##############################################################################

static void GC9A01_Unselect(void);
static void GC9A01_Select(void);
static void GC9A01_SendCmd(uint8_t Cmd);
static void GC9A01_SendData(uint8_t Data );
static void GC9A01_SendDataMASS(uint8_t* buff, size_t buff_size);
static void GC9A01_SetWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
static void GC9A01_RamWrite(uint16_t *pBuff, uint32_t Len);
static void GC9A01_ColumnSet(uint16_t ColumnStart, uint16_t ColumnEnd);
static void GC9A01_RowSet(uint16_t RowStart, uint16_t RowEnd);
static void SwapInt16Values(int16_t *pValue1, int16_t *pValue2);
static void GC9A01_DrawLine_Slow(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);


//==============================================================================
// РџСЂРѕС†РµРґСѓСЂР° РёРЅРёС†РёР°Р»РёР·Р°С†РёРё РґРёСЃРїР»РµСЏ
//==============================================================================
//==== РґР°РЅРЅС‹Рµ РґР»СЏ РёРЅРёС†РёР°Р»РёР·Р°С†РёРё РґРёСЃРїР»РµСЏ GC9A01_240X240 ==========
void GC9A01_Init(void){

        // Р—Р°РґРµСЂР¶РєР° РїРѕСЃР»Рµ РїРѕРґР°С‡Рё РїРёС‚Р°РЅРёСЏ
        // РµСЃР»Рё РїСЂРё СЃС‚Р°СЂС‚Рµ РЅРµ РІСЃРµРіРґР° Р·Р°РїСѓСЃРєР°РµС‚СЊСЃСЏ РґРёСЃРїР»РµР№ СѓРІРµР»РёС‡РёРІР°РµРј РІСЂРµРјСЏ Р·Р°РґРµСЂР¶РєРё
        delay_ms(100);
        GC9A01_Width = GC9A01_WIDTH;
        GC9A01_Height = GC9A01_HEIGHT;

        GC9A01_HardReset();

    GC9A01_Select();

        GC9A01_SendCmd(GC9A01_InnerReg2Enable);
        GC9A01_SendCmd(0xEB);
        GC9A01_SendData(0x14);

        GC9A01_SendCmd(GC9A01_InnerReg1Enable);
        GC9A01_SendCmd(GC9A01_InnerReg2Enable);

        GC9A01_SendCmd(0xEB);
        GC9A01_SendData(0x14);

        GC9A01_SendCmd(0x84);
        GC9A01_SendData(0x40);

        GC9A01_SendCmd(0x85);
        GC9A01_SendData(0xFF);

        GC9A01_SendCmd(0x86);
        GC9A01_SendData(0xFF);

        GC9A01_SendCmd(0x87);
        GC9A01_SendData(0xFF);

        GC9A01_SendCmd(0x88);
        GC9A01_SendData(0x0A);

        GC9A01_SendCmd(0x89);
        GC9A01_SendData(0x21);

        GC9A01_SendCmd(0x8A);
        GC9A01_SendData(0x00);

        GC9A01_SendCmd(0x8B);
        GC9A01_SendData(0x80);

        GC9A01_SendCmd(0x8C);
        GC9A01_SendData(0x01);

        GC9A01_SendCmd(0x8D);
        GC9A01_SendData(0x01);

        GC9A01_SendCmd(0x8E);
        GC9A01_SendData(0xFF);

        GC9A01_SendCmd(0x8F);
        GC9A01_SendData(0xFF);

        GC9A01_SendCmd(GC9A01_DisplayFunctionControl);
        GC9A01_SendData(0x00);
        GC9A01_SendData(0x20); // Scan direction S360 -> S1

        // def rotation
        GC9A01_SendCmd(GC9A01_MADCTL);
        GC9A01_SendData(GC9A01_DEF_ROTATION);

        // ColorMode
        GC9A01_SendCmd(GC9A01_COLMOD);
        GC9A01_SendData(ColorMode_MCU_16bit & 0x77);

        GC9A01_SendCmd(0x90);
        GC9A01_SendData(0x08);
        GC9A01_SendData(0x08);
        GC9A01_SendData(0x08);
        GC9A01_SendData(0x08);

        GC9A01_SendCmd(0xBD);
        GC9A01_SendData(0x06);

        GC9A01_SendCmd(0xBC);
        GC9A01_SendData(0x00);

        GC9A01_SendCmd(0xFF);
        GC9A01_SendData(0x60);
        GC9A01_SendData(0x01);
        GC9A01_SendData(0x04);

        GC9A01_SendCmd(GC9A01_PWCTR2);  // Power control 2
        GC9A01_SendData(0x13);       // 5.18 V
        GC9A01_SendCmd(GC9A01_PWCTR3);  // Power control 3
        GC9A01_SendData(0x13);       // VREG2A = -3.82 V, VREG2B = 0.68 V
        GC9A01_SendCmd(GC9A01_PWCTR4);  // Power control 4
        GC9A01_SendData(0x22);       // VREG2A = 5.88 V, VREG2B = -2.88 V

        GC9A01_SendCmd(0xBE);
        GC9A01_SendData(0x11);

        GC9A01_SendCmd(0xE1);
        GC9A01_SendData(0x10);
        GC9A01_SendData(0x0E);

        GC9A01_SendCmd(0xDF);
        GC9A01_SendData(0x21);
        GC9A01_SendData(0x0c);
        GC9A01_SendData(0x02);

        GC9A01_SendCmd(GC9A01_GAMMA1);
        GC9A01_SendData(0x45);
        GC9A01_SendData(0x09);
        GC9A01_SendData(0x08);
        GC9A01_SendData(0x08);
        GC9A01_SendData(0x26);
        GC9A01_SendData(0x2A);

        GC9A01_SendCmd(GC9A01_GAMMA2);
        GC9A01_SendData(0x43);
        GC9A01_SendData(0x70);
        GC9A01_SendData(0x72);
        GC9A01_SendData(0x36);
        GC9A01_SendData(0x37);
        GC9A01_SendData(0x6F);

        GC9A01_SendCmd(GC9A01_GAMMA3);
        GC9A01_SendData(0x45);
        GC9A01_SendData(0x09);
        GC9A01_SendData(0x08);
        GC9A01_SendData(0x08);
        GC9A01_SendData(0x26);
        GC9A01_SendData(0x2A);

        GC9A01_SendCmd(GC9A01_GAMMA4);
        GC9A01_SendData(0x43);
        GC9A01_SendData(0x70);
        GC9A01_SendData(0x72);
        GC9A01_SendData(0x36);
        GC9A01_SendData(0x37);
        GC9A01_SendData(0x6F);

        GC9A01_SendCmd(0xED);
        GC9A01_SendData(0x1B);
        GC9A01_SendData(0x0B);

        GC9A01_SendCmd(0xAE);
        GC9A01_SendData(0x77);

        GC9A01_SendCmd(0xCD);
        GC9A01_SendData(0x63);

        GC9A01_SendCmd(0x70);
        GC9A01_SendData(0x07);
        GC9A01_SendData(0x07);
        GC9A01_SendData(0x04);
        GC9A01_SendData(0x0E);
        GC9A01_SendData(0x0F);
        GC9A01_SendData(0x09);
        GC9A01_SendData(0x07);
        GC9A01_SendData(0x08);
        GC9A01_SendData(0x03);

        GC9A01_SendCmd(GC9A01_FRAMERATE);       // Frame rate
        GC9A01_SendData(0x34);                  // 4 dot inversion

        GC9A01_SendCmd(0x62);
        GC9A01_SendData(0x18);
        GC9A01_SendData(0x0D);
        GC9A01_SendData(0x71);
        GC9A01_SendData(0xED);
        GC9A01_SendData(0x70);
        GC9A01_SendData(0x70);
        GC9A01_SendData(0x18);
        GC9A01_SendData(0x0F);
        GC9A01_SendData(0x71);
        GC9A01_SendData(0xEF);
        GC9A01_SendData(0x70);
        GC9A01_SendData(0x70);

        GC9A01_SendCmd(0x63);
        GC9A01_SendData(0x18);
        GC9A01_SendData(0x11);
        GC9A01_SendData(0x71);
        GC9A01_SendData(0xF1);
        GC9A01_SendData(0x70);
        GC9A01_SendData(0x70);
        GC9A01_SendData(0x18);
        GC9A01_SendData(0x13);
        GC9A01_SendData(0x71);
        GC9A01_SendData(0xF3);
        GC9A01_SendData(0x70);
        GC9A01_SendData(0x70);

        GC9A01_SendCmd(0x64);
        GC9A01_SendData(0x28);
        GC9A01_SendData(0x29);
        GC9A01_SendData(0xF1);
        GC9A01_SendData(0x01);
        GC9A01_SendData(0xF1);
        GC9A01_SendData(0x00);
        GC9A01_SendData(0x07);

        GC9A01_SendCmd(0x66);
        GC9A01_SendData(0x3C);
        GC9A01_SendData(0x00);
        GC9A01_SendData(0xCD);
        GC9A01_SendData(0x67);
        GC9A01_SendData(0x45);
        GC9A01_SendData(0x45);
        GC9A01_SendData(0x10);
        GC9A01_SendData(0x00);
        GC9A01_SendData(0x00);
        GC9A01_SendData(0x00);

        GC9A01_SendCmd(0x67);
        GC9A01_SendData(0x00);
        GC9A01_SendData(0x3C);
        GC9A01_SendData(0x00);
        GC9A01_SendData(0x00);
        GC9A01_SendData(0x00);
        GC9A01_SendData(0x01);
        GC9A01_SendData(0x54);
        GC9A01_SendData(0x10);
        GC9A01_SendData(0x32);
        GC9A01_SendData(0x98);

        GC9A01_SendCmd(0x74);
        GC9A01_SendData(0x10);
        GC9A01_SendData(0x85);
        GC9A01_SendData(0x80);
        GC9A01_SendData(0x00);
        GC9A01_SendData(0x00);
        GC9A01_SendData(0x4E);
        GC9A01_SendData(0x00);

        GC9A01_SendCmd(0x98);
        GC9A01_SendData(0x3e);
        GC9A01_SendData(0x07);

        GC9A01_SendCmd(GC9A01_TEON);        // Tearing effect line on

        // Inversion Mode 1;
        GC9A01_SendCmd(GC9A01_INVON);

        // Sleep Mode Exit
        GC9A01_SendCmd(GC9A01_SLPOUT);

        delay_ms(120);

        // Display Power on
        GC9A01_SendCmd(GC9A01_DISPON);

        GC9A01_Unselect();

        GC9A01_FillRect(0, 0, GC9A01_Width, GC9A01_Height, GC9A01_BLACK);

#if FRAME_BUFFER    // РµСЃР»Рё РІРєР»СЋС‡РµРЅ Р±СѓС„РµСЂ РєР°РґСЂР°
        GC9A01_Update();
#endif
}
//==============================================================================


//==============================================================================
// РџСЂРѕС†РµРґСѓСЂР° СѓРїСЂР°РІР»РµРЅРёСЏ SPI
//==============================================================================
static void GC9A01_Select(void) {
    CS_LOW();
}
//==============================================================================


//==============================================================================
// РџСЂРѕС†РµРґСѓСЂР° СѓРїСЂР°РІР»РµРЅРёСЏ SPI
//==============================================================================
static void GC9A01_Unselect(void) {
    CS_HIGH();
}
//==============================================================================


//==============================================================================
// РџСЂРѕС†РµРґСѓСЂР° РІС‹РІРѕРґР° С†РІРµС‚РЅРѕРіРѕ РёР·РѕР±СЂР°Р¶РµРЅРёСЏ РЅР° РґРёСЃРїР»РµР№
//==============================================================================
void GC9A01_DrawImage(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint16_t* data) {

  if((x >= GC9A01_Width) || (y >= GC9A01_Height)){
        return;
    }

  if((x + w - 1) >= GC9A01_Width){
        return;
    }

  if((y + h - 1) >= GC9A01_Height){
        return;
    }

#if FRAME_BUFFER    // РµСЃР»Рё РІРєР»СЋС‡РµРЅ Р±СѓС„РµСЂ РєР°РґСЂР°
    for( uint16_t i = 0; i < h; i++ ){
        for( uint16_t j = 0; j < w; j++ ){
            buff_frame[( y + i ) * GC9A01_Width + x + j] = *data;
            data++;
        }
    }
#else   //РµСЃР»Рё РїРѕРїРёРєСЃРµР»СЊРЅС‹Р№ РІС‹РІРѕРґ
    GC9A01_SetWindow(x, y, x+w-1, y+h-1);

    GC9A01_Select();

  GC9A01_SendDataMASS((uint8_t*)data, sizeof(uint16_t)*w*h);

  GC9A01_Unselect();
#endif

}
//==============================================================================


//==============================================================================
// РџСЂРѕС†РµРґСѓСЂР° Р°РїРїР°СЂР°С‚РЅРѕРіРѕ СЃР±СЂРѕСЃР° РґРёСЃРїР»РµСЏ (РЅРѕР¶РєРѕР№ RESET)
//==============================================================================
void GC9A01_HardReset(void){
    RST_LOW();
    delay_ms(50);
    RST_HIGH();
    delay_ms(150);

}
//==============================================================================


//==============================================================================
// РџСЂРѕС†РµРґСѓСЂР° РѕС‚РїСЂР°РІРєРё РєРѕРјР°РЅРґС‹ РІ РґРёСЃРїР»РµР№
//==============================================================================
__inline static void GC9A01_SendCmd(uint8_t Cmd){

    DC_LOW();
    ssi3_write8(Cmd);
    DC_HIGH();
}
//==============================================================================


//==============================================================================
// РџСЂРѕС†РµРґСѓСЂР° РѕС‚РїСЂР°РІРєРё РґР°РЅРЅС‹С… (РїР°СЂР°РјРµС‚СЂРѕРІ) РІ РґРёСЃРїР»РµР№ 1 BYTE
//==============================================================================
__inline static void GC9A01_SendData(uint8_t Data ){
    ssi3_write8(Data);
}
//==============================================================================


//==============================================================================
// РџСЂРѕС†РµРґСѓСЂР° РѕС‚РїСЂР°РІРєРё РґР°РЅРЅС‹С… (РїР°СЂР°РјРµС‚СЂРѕРІ) РІ РґРёСЃРїР»РµР№ MASS
//==============================================================================
__inline static void GC9A01_SendDataMASS(uint8_t* buff, size_t buff_size){

        if( buff_size <= 0xFFFF ){
            ssi3_write_buf(buff, buff_size);
        }
        else{
            while( buff_size > 0xFFFF ){
                ssi3_write_buf(buff, 0xFFFF);
                buff_size-=0xFFFF;
                buff+=0xFFFF;
            }
            ssi3_write_buf(buff, buff_size);
        }
}
//==============================================================================


//==============================================================================
// РџСЂРѕС†РµРґСѓСЂР° РІРєР»СЋС‡РµРЅРёСЏ СЂРµР¶РёРјР° СЃРЅР°
//==============================================================================
void GC9A01_SleepModeEnter( void ){

    GC9A01_Select();

    GC9A01_SendCmd(GC9A01_SLPIN);

    GC9A01_Unselect();

    delay_ms(150);
}
//==============================================================================


//==============================================================================
// РџСЂРѕС†РµРґСѓСЂР° РѕС‚РєР»СЋС‡РµРЅРёСЏ СЂРµР¶РёРјР° СЃРЅР°
//==============================================================================
void GC9A01_SleepModeExit( void ){

    GC9A01_Select();

    GC9A01_SendCmd(GC9A01_SLPOUT);

    GC9A01_Unselect();

    delay_ms(150);
}
//==============================================================================


//==============================================================================
// РџСЂРѕС†РµРґСѓСЂР° РІРєР»СЋС‡РµРЅРёСЏ/РѕС‚РєР»СЋС‡РµРЅРёСЏ СЂРµР¶РёРјР° С‡Р°СЃС‚РёС‡РЅРѕРіРѕ Р·Р°РїРѕР»РЅРµРЅРёСЏ СЌРєСЂР°РЅР°
//==============================================================================
void GC9A01_InversionMode(uint8_t Mode){

  GC9A01_Select();

  if (Mode){
    GC9A01_SendCmd(GC9A01_INVON);
  }
  else{
    GC9A01_SendCmd(GC9A01_INVOFF);
  }

  GC9A01_Unselect();
}
//==============================================================================


//==============================================================================
// РџСЂРѕС†РµРґСѓСЂР° Р·Р°РєСЂР°С€РёРІР°РµС‚ СЌРєСЂР°РЅ С†РІРµС‚РѕРј color
//==============================================================================
void GC9A01_FillScreen(uint16_t color){

  GC9A01_FillRect(0, 0,  GC9A01_Width, GC9A01_Height, color);
}
//==============================================================================


//==============================================================================
// РџСЂРѕС†РµРґСѓСЂР° РѕС‡РёСЃС‚РєРё СЌРєСЂР°РЅР° - Р·Р°РєСЂР°С€РёРІР°РµС‚ СЌРєСЂР°РЅ С†РІРµС‚РѕРј С‡РµСЂРЅС‹Р№
//==============================================================================
void GC9A01_Clear(void){

  GC9A01_FillRect(0, 0,  GC9A01_Width, GC9A01_Height, 0);
}
//==============================================================================


//==============================================================================
// РџСЂРѕС†РµРґСѓСЂР° Р·Р°РїРѕР»РЅРµРЅРёСЏ РїСЂСЏРјРѕСѓРіРѕР»СЊРЅРёРєР° С†РІРµС‚РѕРј color
//==============================================================================
void GC9A01_FillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color){

  if ((x >= GC9A01_Width) || (y >= GC9A01_Height)){
      return;
  }

  if ((x + w) > GC9A01_Width){
      w = GC9A01_Width - x;
  }

  if ((y + h) > GC9A01_Height){
      h = GC9A01_Height - y;
  }

#if FRAME_BUFFER    // РµСЃР»Рё РІРєР»СЋС‡РµРЅ Р±СѓС„РµСЂ РєР°РґСЂР°
    if( x >=0 && y >=0 ){
        for( uint16_t i = 0; i < h; i++ ){
            for( uint16_t j = 0; j < w; j++ ){
                buff_frame[( y + i ) * GC9A01_Width + x + j] = ((color & 0xFF)<<8) | (color >> 8 );
            }
        }
    }
#else   //РµСЃР»Рё РїРѕРїРёРєСЃРµР»СЊРЅС‹Р№ РІС‹РІРѕРґ
    GC9A01_SetWindow(x, y, x + w - 1, y + h - 1);

  GC9A01_RamWrite(&color, (h * w));
#endif


}
//==============================================================================


//==============================================================================
// РџСЂРѕС†РµРґСѓСЂР° СѓСЃС‚Р°РЅРѕРІРєР° РіСЂР°РЅРёС† СЌРєСЂР°РЅР° РґР»СЏ Р·Р°РїРѕР»РЅРµРЅРёСЏ
//==============================================================================
static void GC9A01_SetWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1){

    GC9A01_Select();

    GC9A01_ColumnSet(x0, x1);
    GC9A01_RowSet(y0, y1);

    // write to RAM
    GC9A01_SendCmd(GC9A01_RAMWR);

    GC9A01_Unselect();

}
//==============================================================================


//==============================================================================
// РџСЂРѕС†РµРґСѓСЂР° Р·Р°РїРёСЃРё РґР°РЅРЅС‹С… РІ РґРёСЃРїР»РµР№
//==============================================================================
static void GC9A01_RamWrite(uint16_t *pBuff, uint32_t Len){

  GC9A01_Select();

  uint8_t buff[2];
  buff[0] = *pBuff >> 8;
  buff[1] = *pBuff & 0xFF;

  while (Len--){
      GC9A01_SendDataMASS( (uint8_t*)buff, 2);
  }

  GC9A01_Unselect();
}
//==============================================================================


//==============================================================================
// РџСЂРѕС†РµРґСѓСЂР° СѓСЃС‚Р°РЅРѕРІРєРё РЅР°С‡Р°Р»СЊРЅРѕРіРѕ Рё РєРѕРЅРµС‡РЅРѕРіРѕ Р°РґСЂРµСЃРѕРІ РєРѕР»РѕРЅРѕРє
//==============================================================================
static void GC9A01_ColumnSet(uint16_t ColumnStart, uint16_t ColumnEnd){

  if (ColumnStart > ColumnEnd){
    return;
  }

  if (ColumnEnd > GC9A01_Width){
    return;
  }

  ColumnStart += GC9A01_X_Start;
  ColumnEnd += GC9A01_X_Start;

  GC9A01_SendCmd(GC9A01_CASET);
  GC9A01_SendData(ColumnStart >> 8);
  GC9A01_SendData(ColumnStart & 0xFF);
  GC9A01_SendData(ColumnEnd >> 8);
  GC9A01_SendData(ColumnEnd & 0xFF);

}
//==============================================================================


//==============================================================================
// РџСЂРѕС†РµРґСѓСЂР° СѓСЃС‚Р°РЅРѕРІРєРё РЅР°С‡Р°Р»СЊРЅРѕРіРѕ Рё РєРѕРЅРµС‡РЅРѕРіРѕ Р°РґСЂРµСЃРѕРІ СЃС‚СЂРѕРє
//==============================================================================
static void GC9A01_RowSet(uint16_t RowStart, uint16_t RowEnd){

  if (RowStart > RowEnd){
    return;
  }

  if (RowEnd > GC9A01_Height){
    return;
  }

  RowStart += GC9A01_Y_Start;
  RowEnd += GC9A01_Y_Start;

  GC9A01_SendCmd(GC9A01_RASET);
  GC9A01_SendData(RowStart >> 8);
  GC9A01_SendData(RowStart & 0xFF);
  GC9A01_SendData(RowEnd >> 8);
  GC9A01_SendData(RowEnd & 0xFF);

}
//==============================================================================


//==============================================================================
// РџСЂРѕС†РµРґСѓСЂР° СѓРїСЂР°РІР»РµРЅРёСЏ РїРѕРґСЃРІРµС‚РєРѕР№ (РЁР�Рњ)
//==============================================================================
void GC9A01_SetBL(uint8_t Value){

//  if (Value > 100)
//    Value = 100;

//  tmr2_PWM_set(ST77xx_PWM_TMR2_Chan, Value);

}
//==============================================================================


//==============================================================================
// РџСЂРѕС†РµРґСѓСЂР° РІРєР»СЋС‡РµРЅРёСЏ/РѕС‚РєР»СЋС‡РµРЅРёСЏ РїРёС‚Р°РЅРёСЏ РґРёСЃРїР»РµСЏ
//==============================================================================
void GC9A01_DisplayPower(uint8_t On){

  GC9A01_Select();

  if (On){
    GC9A01_SendCmd(GC9A01_DISPON);
  }
  else{
    GC9A01_SendCmd(GC9A01_DISPOFF);
  }

  GC9A01_Unselect();
}
//==============================================================================


//==============================================================================
// РџСЂРѕС†РµРґСѓСЂР° СЂРёСЃРѕРІР°РЅРёСЏ РїСЂСЏРјРѕСѓРіРѕР»СЊРЅРёРєР° ( РїСѓСЃС‚РѕС‚РµР»С‹Р№ )
//==============================================================================
void GC9A01_DrawRectangle(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color) {

  GC9A01_DrawLine(x1, y1, x1, y2, color);
  GC9A01_DrawLine(x2, y1, x2, y2, color);
  GC9A01_DrawLine(x1, y1, x2, y1, color);
  GC9A01_DrawLine(x1, y2, x2, y2, color);

}
//==============================================================================


//==============================================================================
// РџСЂРѕС†РµРґСѓСЂР° РІСЃРїРѕРјРѕРіР°С‚РµР»СЊРЅР°СЏ РґР»СЏ --- РџСЂРѕС†РµРґСѓСЂР° СЂРёСЃРѕРІР°РЅРёСЏ РїСЂСЏРјРѕСѓРіРѕР»СЊРЅРёРєР° ( Р·Р°РїРѕР»РЅРµРЅС‹Р№ )
//==============================================================================
static void SwapInt16Values(int16_t *pValue1, int16_t *pValue2){

  int16_t TempValue = *pValue1;
  *pValue1 = *pValue2;
  *pValue2 = TempValue;
}
//==============================================================================


//==============================================================================
// РџСЂРѕС†РµРґСѓСЂР° СЂРёСЃРѕРІР°РЅРёСЏ РїСЂСЏРјРѕСѓРіРѕР»СЊРЅРёРєР° ( Р·Р°РїРѕР»РЅРµРЅС‹Р№ )
//==============================================================================
void GC9A01_DrawRectangleFilled(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t fillcolor) {

  if (x1 > x2){
    SwapInt16Values(&x1, &x2);
  }

  if (y1 > y2){
    SwapInt16Values(&y1, &y2);
  }

  GC9A01_FillRect(x1, y1, x2 - x1, y2 - y1, fillcolor);
}
//==============================================================================


//==============================================================================
// РџСЂРѕС†РµРґСѓСЂР° РІСЃРїРѕРјРѕРіР°С‚РµР»СЊРЅР°СЏ РґР»СЏ --- РџСЂРѕС†РµРґСѓСЂР° СЂРёСЃРѕРІР°РЅРёСЏ Р»РёРЅРёРё
//==============================================================================
static void GC9A01_DrawLine_Slow(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color) {

  const int16_t deltaX = abs(x2 - x1);
  const int16_t deltaY = abs(y2 - y1);
  const int16_t signX = x1 < x2 ? 1 : -1;
  const int16_t signY = y1 < y2 ? 1 : -1;

  int16_t error = deltaX - deltaY;

  GC9A01_DrawPixel(x2, y2, color);

  while (x1 != x2 || y1 != y2) {

    GC9A01_DrawPixel(x1, y1, color);
    const int16_t error2 = error * 2;

    if (error2 > -deltaY) {

      error -= deltaY;
      x1 += signX;
    }
    if (error2 < deltaX){

      error += deltaX;
      y1 += signY;
    }
  }
}
//==============================================================================


//==============================================================================
// РџСЂРѕС†РµРґСѓСЂР° СЂРёСЃРѕРІР°РЅРёСЏ Р»РёРЅРёРё
//==============================================================================
void GC9A01_DrawLine(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color) {

  if (x1 == x2){

    if (y1 > y2){
      GC9A01_FillRect(x1, y2, 1, y1 - y2 + 1, color);
    }
    else{
      GC9A01_FillRect(x1, y1, 1, y2 - y1 + 1, color);
    }

    return;
  }

  if (y1 == y2){

    if (x1 > x2){
      GC9A01_FillRect(x2, y1, x1 - x2 + 1, 1, color);
    }
    else{
      GC9A01_FillRect(x1, y1, x2 - x1 + 1, 1, color);
    }

    return;
  }

  GC9A01_DrawLine_Slow(x1, y1, x2, y2, color);
}
//==============================================================================


//==============================================================================
// РџСЂРѕС†РµРґСѓСЂР° СЂРёСЃРѕРІР°РЅРёСЏ Р»РёРЅРёРё СЃ СѓРєР°Р·Р°РЅС‹Рј СѓРіР»РѕРј Рё РґР»РёРЅРѕР№
//==============================================================================
void GC9A01_DrawLineWithAngle(int16_t x, int16_t y, uint16_t length, double angle_degrees, uint16_t color) {
    // РџСЂРµРѕР±СЂР°Р·РѕРІР°РЅРёРµ СѓРіР»Р° РІ СЂР°РґРёР°РЅС‹
    double angle_radians = (360.0 - angle_degrees) * PI / 180.0;

    // Р’С‹С‡РёСЃР»РµРЅРёРµ РєРѕРЅРµС‡РЅС‹С… РєРѕРѕСЂРґРёРЅР°С‚
    int16_t x2 = x + length * cos(angle_radians) + 0.5;
    int16_t y2 = y + length * sin(angle_radians) + 0.5;

    // Р�СЃРїРѕР»СЊР·СѓРµРј СЃСѓС‰РµСЃС‚РІСѓСЋС‰СѓСЋ С„СѓРЅРєС†РёСЋ РґР»СЏ СЂРёСЃРѕРІР°РЅРёСЏ Р»РёРЅРёРё
    GC9A01_DrawLine(x, y, x2, y2, color);
}
//==============================================================================


//==============================================================================
// РџСЂРѕС†РµРґСѓСЂР° СЂРёСЃРѕРІР°РЅРёСЏ С‚СЂРµСѓРіРѕР»СЊРЅРёРєР° ( РїСѓСЃС‚РѕС‚РµР»С‹Р№ )
//==============================================================================
void GC9A01_DrawTriangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, uint16_t color){
    /* Draw lines */
    GC9A01_DrawLine(x1, y1, x2, y2, color);
    GC9A01_DrawLine(x2, y2, x3, y3, color);
    GC9A01_DrawLine(x3, y3, x1, y1, color);
}
//==============================================================================


//==============================================================================
// РџСЂРѕС†РµРґСѓСЂР° СЂРёСЃРѕРІР°РЅРёСЏ С‚СЂРµСѓРіРѕР»СЊРЅРёРєР° ( Р·Р°РїРѕР»РЅРµРЅС‹Р№ )
//==============================================================================
void GC9A01_DrawFilledTriangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, uint16_t color){

    int16_t deltax = 0, deltay = 0, x = 0, y = 0, xinc1 = 0, xinc2 = 0,
    yinc1 = 0, yinc2 = 0, den = 0, num = 0, numadd = 0, numpixels = 0,
    curpixel = 0;

    deltax = abs(x2 - x1);
    deltay = abs(y2 - y1);
    x = x1;
    y = y1;

    if (x2 >= x1) {
        xinc1 = 1;
        xinc2 = 1;
    }
    else {
        xinc1 = -1;
        xinc2 = -1;
    }

    if (y2 >= y1) {
        yinc1 = 1;
        yinc2 = 1;
    }
    else {
        yinc1 = -1;
        yinc2 = -1;
    }

    if (deltax >= deltay){
        xinc1 = 0;
        yinc2 = 0;
        den = deltax;
        num = deltax / 2;
        numadd = deltay;
        numpixels = deltax;
    }
    else {
        xinc2 = 0;
        yinc1 = 0;
        den = deltay;
        num = deltay / 2;
        numadd = deltax;
        numpixels = deltay;
    }

    for (curpixel = 0; curpixel <= numpixels; curpixel++) {
        GC9A01_DrawLine(x, y, x3, y3, color);

        num += numadd;
        if (num >= den) {
            num -= den;
            x += xinc1;
            y += yinc1;
        }
        x += xinc2;
        y += yinc2;
    }
}
//==============================================================================


//==============================================================================
// РџСЂРѕС†РµРґСѓСЂР° РѕРєСЂР°С€РёРІР°РµС‚ 1 РїРёРєСЃРµР»СЊ РґРёСЃРїР»РµСЏ
//==============================================================================
void GC9A01_DrawPixel(int16_t x, int16_t y, uint16_t color){

  if ((x < 0) ||(x >= GC9A01_Width) || (y < 0) || (y >= GC9A01_Height)){
    return;
  }

#if FRAME_BUFFER    // РµСЃР»Рё РІРєР»СЋС‡РµРЅ Р±СѓС„РµСЂ РєР°РґСЂР°
    buff_frame[y * GC9A01_Width + x] = ((color & 0xFF)<<8) | (color >> 8 );
#else   //РµСЃР»Рё РїРѕРїРёРєСЃРµР»СЊРЅС‹Р№ РІС‹РІРѕРґ
    GC9A01_SetWindow(x, y, x, y);
  GC9A01_RamWrite(&color, 1);
#endif
}
//==============================================================================


//==============================================================================
// РџСЂРѕС†РµРґСѓСЂР° СЂРёСЃРѕРІР°РЅРёСЏ РєСЂСѓРі ( Р·Р°РїРѕР»РЅРµРЅС‹Р№ )
//==============================================================================
void GC9A01_DrawCircleFilled(int16_t x0, int16_t y0, int16_t radius, uint16_t fillcolor) {

  int x = 0;
  int y = radius;
  int delta = 1 - 2 * radius;
  int error = 0;

  while (y >= 0){

    GC9A01_DrawLine(x0 + x, y0 - y, x0 + x, y0 + y, fillcolor);
    GC9A01_DrawLine(x0 - x, y0 - y, x0 - x, y0 + y, fillcolor);
    error = 2 * (delta + y) - 1;

    if (delta < 0 && error <= 0) {

      ++x;
      delta += 2 * x + 1;
      continue;
    }

    error = 2 * (delta - x) - 1;

    if (delta > 0 && error > 0) {

      --y;
      delta += 1 - 2 * y;
      continue;
    }

    ++x;
    delta += 2 * (x - y);
    --y;
  }
}
//==============================================================================


//==============================================================================
// РџСЂРѕС†РµРґСѓСЂР° СЂРёСЃРѕРІР°РЅРёСЏ РєСЂСѓРі ( РїСѓСЃС‚РѕС‚РµР»С‹Р№ )
//==============================================================================
void GC9A01_DrawCircle(int16_t x0, int16_t y0, int16_t radius, uint16_t color) {

  int x = 0;
  int y = radius;
  int delta = 1 - 2 * radius;
  int error = 0;

  while (y >= 0){

    GC9A01_DrawPixel(x0 + x, y0 + y, color);
    GC9A01_DrawPixel(x0 + x, y0 - y, color);
    GC9A01_DrawPixel(x0 - x, y0 + y, color);
    GC9A01_DrawPixel(x0 - x, y0 - y, color);
    error = 2 * (delta + y) - 1;

    if (delta < 0 && error <= 0) {

      ++x;
      delta += 2 * x + 1;
      continue;
    }

    error = 2 * (delta - x) - 1;

    if (delta > 0 && error > 0) {

      --y;
      delta += 1 - 2 * y;
      continue;
    }

    ++x;
    delta += 2 * (x - y);
    --y;
  }
}
//==============================================================================


//==============================================================================
// СЂРёСЃСѓРµРј СЌР»РёРїСЃ
//==============================================================================
void GC9A01_DrawEllipse(int16_t x0, int16_t y0, int16_t radiusX, int16_t radiusY, uint16_t color) {
    int x, y;
    for (float angle = 0; angle <= 360; angle += 0.1) {
        x = x0 + radiusX * cos(angle * PI / 180);
        y = y0 + radiusY * sin(angle * PI / 180);
        GC9A01_DrawPixel(x, y, color);
    }
}
//==============================================================================


//==============================================================================
// СЂРёСЃСѓРµРј СЌР»РёРїСЃ РїРѕРґ СѓРєР°Р·Р°РЅС‹Рј СѓРіР»РѕРј РЅР°РєР»РѕРЅР°
//==============================================================================
void GC9A01_DrawEllipseWithAngle(int16_t x0, int16_t y0, int16_t radiusX, int16_t radiusY, float angle_degrees, uint16_t color) {
    float cosAngle = cos((360.0 - angle_degrees) * PI / 180);
    float sinAngle = sin((360.0 - angle_degrees) * PI / 180);

    for (int16_t t = 0; t <= 360; t++) {
        float radians = t * PI / 180.0;
        int16_t x = radiusX * cos(radians);
        int16_t y = radiusY * sin(radians);

        int16_t xTransformed = x0 + cosAngle * x - sinAngle * y;
        int16_t yTransformed = y0 + sinAngle * x + cosAngle * y;

        GC9A01_DrawPixel(xTransformed, yTransformed, color);
    }
}
//==============================================================================


//==============================================================================
// СЂРёСЃСѓРµРј СЌР»РёРїСЃ Р·Р°РєСЂР°С€РµРЅРЅС‹Р№
//==============================================================================
void GC9A01_DrawEllipseFilled(int16_t x0, int16_t y0, int16_t radiusX, int16_t radiusY, uint16_t color) {
    int x, y;

    for (y = -radiusY; y <= radiusY; y++) {
            for (x = -radiusX; x <= radiusX; x++) {
                    if ((x * x * radiusY * radiusY + y * y * radiusX * radiusX) <= (radiusX * radiusX * radiusY * radiusY)) {
                            GC9A01_DrawPixel(x0 + x, y0 + y, color);
                    }
            }
    }
}
//==============================================================================


//==============================================================================
// СЂРёСЃСѓРµРј СЌР»РёРїСЃ Р·Р°РєСЂР°С€РµРЅРЅС‹Р№ РїРѕРґ СѓРєР°Р·Р°РЅС‹Рј СѓРіР»РѕРј РЅР°РєР»РѕРЅР°
//==============================================================================
void GC9A01_DrawEllipseFilledWithAngle(int16_t x0, int16_t y0, int16_t radiusX, int16_t radiusY, float angle_degrees, uint16_t color) {
   float cosAngle = cos((360.0 - angle_degrees) * PI / 180.0);
    float sinAngle = sin((360.0 - angle_degrees) * PI / 180.0);

    for (int16_t y = -radiusY; y <= radiusY; y++) {
        for (int16_t x = -radiusX; x <= radiusX; x++) {
          float xTransformed = cosAngle * x - sinAngle * y;
          float yTransformed = sinAngle * x + cosAngle * y;

                    if ((x * x * radiusY * radiusY + y * y * radiusX * radiusX) <= (radiusX * radiusX * radiusY * radiusY)){
             GC9A01_DrawPixel(x0 + xTransformed, y0  + yTransformed, color);
          }
        }
    }
}
//==============================================================================


//==============================================================================
// РџСЂРѕС†РµРґСѓСЂР° СЂРёСЃРѕРІР°РЅРёСЏ СЃРёРјРІРѕР»Р° ( 1 Р±СѓРєРІР° РёР»Рё Р·РЅР°Рє )
//==============================================================================
void GC9A01_DrawChar(uint16_t x, uint16_t y, uint16_t TextColor, uint16_t BgColor, uint8_t TransparentBg, FontDef_t* Font, uint8_t multiplier, unsigned char ch){

    uint32_t i, b, j;

    uint32_t X = x, Y = y;

    uint8_t xx, yy;

    if( multiplier < 1 ){
        multiplier = 1;
    }

    /* Check available space in LCD */
    if (GC9A01_Width >= ( x + Font->FontWidth) || GC9A01_Height >= ( y + Font->FontHeight)){


            /* Go through font */
            for (i = 0; i < Font->FontHeight; i++) {

                if( ch < 127 ){
                    b = Font->data[(ch - 32) * Font->FontHeight + i];
                }

                else if( (uint8_t) ch > 191 ){
                    // +96 СЌС‚Рѕ С‚Р°Рє РєР°Рє Р»Р°С‚РёРЅСЃРєРёРµ СЃРёРјРІРѕР»С‹ Рё Р·РЅР°РєРё РІ С€СЂРёС„С‚Р°С… Р·Р°РЅРёРјР°СЋС‚ 96 РїРѕР·РёС†РёР№
                    // Рё РµСЃР»Рё РІ С€СЂРёС„С‚Рµ РєРѕС‚РѕСЂС‹Р№ СЃРѕРґРµСЂР¶РёС‚ СЃРїРµСЂРІР° Р»Р°С‚РёРЅРёС†Сѓ Рё СЃРїРµС† СЃРёРјРІРѕР»С‹ Рё РїРѕС‚РѕРј
                    // С‚РѕР»СЊРєРѕ РєРёСЂРёР»РёС†Сѓ С‚Рѕ РЅСѓР¶РЅРѕ РґРѕР±Р°РІР»СЏС‚СЊ 95 РµСЃР»Рё С€СЂРёС„С‚
                    // СЃРѕРґРµСЂР¶РёС‚ С‚РѕР»СЊРєРѕ РєРёСЂРёР»РёС†Сѓ С‚Рѕ +96 РЅРµ РЅСѓР¶РЅРѕ
                    b = Font->data[((ch - 192) + 96) * Font->FontHeight + i];
                }

                else if( (uint8_t) ch == 168 ){ // 168 СЃРёРјРІРѕР» РїРѕ ASCII - РЃ
                    // 160 СЌР»Р»РµРјРµРЅС‚ ( СЃРёРјРІРѕР» РЃ )
                    b = Font->data[( 160 ) * Font->FontHeight + i];
                }

                else if( (uint8_t) ch == 184 ){ // 184 СЃРёРјРІРѕР» РїРѕ ASCII - С‘
                    // 161 СЌР»Р»РµРјРµРЅС‚  ( СЃРёРјРІРѕР» С‘ )
                    b = Font->data[( 161 ) * Font->FontHeight + i];
                }
                //-------------------------------------------------------------------

                //----  РЈРєСЂР°РёРЅСЃРєР°СЏ СЂР°СЃРєР»Р°РґРєР° ----------------------------------------------------
                else if( (uint8_t) ch == 170 ){ // 168 СЃРёРјРІРѕР» РїРѕ ASCII - Р„
                    // 162 СЌР»Р»РµРјРµРЅС‚ ( СЃРёРјРІРѕР» Р„ )
                    b = Font->data[( 162 ) * Font->FontHeight + i];
                }
                else if( (uint8_t) ch == 175 ){ // 184 СЃРёРјРІРѕР» РїРѕ ASCII - Р‡
                    // 163 СЌР»Р»РµРјРµРЅС‚  ( СЃРёРјРІРѕР» Р‡ )
                    b = Font->data[( 163 ) * Font->FontHeight + i];
                }
                else if( (uint8_t) ch == 178 ){ // 168 СЃРёРјРІРѕР» РїРѕ ASCII - Р†
                    // 164 СЌР»Р»РµРјРµРЅС‚ ( СЃРёРјРІРѕР» Р† )
                    b = Font->data[( 164 ) * Font->FontHeight + i];
                }
                else if( (uint8_t) ch == 179 ){ // 184 СЃРёРјРІРѕР» РїРѕ ASCII - С–
                    // 165 СЌР»Р»РµРјРµРЅС‚  ( СЃРёРјРІРѕР» С– )
                    b = Font->data[( 165 ) * Font->FontHeight + i];
                }
                else if( (uint8_t) ch == 186 ){ // 184 СЃРёРјРІРѕР» РїРѕ ASCII - С”
                    // 166 СЌР»Р»РµРјРµРЅС‚  ( СЃРёРјРІРѕР» С” )
                    b = Font->data[( 166 ) * Font->FontHeight + i];
                }
                else if( (uint8_t) ch == 191 ){ // 168 СЃРёРјРІРѕР» РїРѕ ASCII - С—
                    // 167 СЌР»Р»РµРјРµРЅС‚ ( СЃРёРјРІРѕР» С— )
                    b = Font->data[( 167 ) * Font->FontHeight + i];
                }
                //-----------------------------------------------------------------------------

                for (j = 0; j < Font->FontWidth; j++) {

                    if ((b << j) & 0x8000) {

                        for (yy = 0; yy < multiplier; yy++){
                            for (xx = 0; xx < multiplier; xx++){
                                    GC9A01_DrawPixel(X+xx, Y+yy, TextColor);
                            }
                        }

                    }
                    else if( TransparentBg ){

                        for (yy = 0; yy < multiplier; yy++){
                            for (xx = 0; xx < multiplier; xx++){
                                    GC9A01_DrawPixel(X+xx, Y+yy, BgColor);
                            }
                        }

                    }
                    X = X + multiplier;
                }
                X = x;
                Y = Y + multiplier;
            }
    }
}
//==============================================================================


//==============================================================================
// РџСЂРѕС†РµРґСѓСЂР° СЂРёСЃРѕРІР°РЅРёСЏ СЃС‚СЂРѕРєРё
//==============================================================================
void GC9A01_print(uint16_t x, uint16_t y, uint16_t TextColor, uint16_t BgColor, uint8_t TransparentBg, FontDef_t* Font, uint8_t multiplier, char *str){

    if( multiplier < 1 ){
        multiplier = 1;
    }

    unsigned char buff_char;

    uint16_t len = strlen(str);

    while (len--) {

        //---------------------------------------------------------------------
        // РїСЂРѕРІРµСЂРєР° РЅР° РєРёСЂРёР»Р»РёС†Сѓ UTF-8, РµСЃР»Рё Р»Р°С‚РёРЅРёС†Р° С‚Рѕ РїСЂРѕРїСѓСЃРєР°РµРј if
        // Р Р°СЃС€РёСЂРµРЅРЅС‹Рµ СЃРёРјРІРѕР»С‹ ASCII Win-1251 РєРёСЂРёР»Р»РёС†Р° (РєРѕРґ СЃРёРјРІРѕР»Р° 128-255)
        // РїСЂРѕРІРµСЂСЏРµРј РїРµСЂРІС‹Р№ Р±Р°Р№С‚ РёР· РґРІСѓС… ( С‚Р°Рє РєР°Рє UTF-8 РµС‚Рѕ РґРІР° Р±Р°Р№С‚Р° )
        // РµСЃР»Рё РѕРЅ Р±РѕР»СЊС€Рµ Р»РёР±Рѕ СЂР°РІРµРЅ 0xC0 ( РїРµСЂРІС‹Р№ Р±Р°Р№С‚ РІ РєРёСЂРёР»Р»РµС†Рµ Р±СѓРґРµС‚ СЂР°РІРµРЅ 0xD0 Р»РёР±Рѕ 0xD1 РёРјРµРЅРЅРѕ РІ Р°Р»С„Р°РІРёС‚Рµ )
        if ( (uint8_t)*str >= 0xC0 ){   // РєРѕРґ 0xC0 СЃРѕРѕС‚РІРµС‚СЃС‚РІСѓРµС‚ СЃРёРјРІРѕР»Сѓ РєРёСЂРёР»Р»РёС†Р° 'A' РїРѕ ASCII Win-1251

            // РїСЂРѕРІРµСЂСЏРµРј РєР°РєРѕР№ РёРјРµРЅРЅРѕ Р±Р°Р№С‚ РїРµСЂРІС‹Р№ 0xD0 Р»РёР±Рѕ 0xD1---------------------------------------------
            switch ((uint8_t)*str) {
                case 0xD0: {
                    // СѓРІРµР»РёС‡РёРІР°РµРј РјР°СЃСЃРёРІ С‚Р°Рє РєР°Рє РЅР°Рј РЅСѓР¶РµРЅ РІС‚РѕСЂРѕР№ Р±Р°Р№С‚
                    str++;
                    // РїСЂРѕРІРµСЂСЏРµРј РІС‚РѕСЂРѕР№ Р±Р°Р№С‚ С‚Р°Рј СЃР°Рј СЃРёРјРІРѕР»
                    if ((uint8_t)*str >= 0x90 && (uint8_t)*str <= 0xBF){ buff_char = (*str) + 0x30; }   // Р±Р°Р№С‚ СЃРёРјРІРѕР»РѕРІ Рђ...РЇ Р°...Рї  РґРµР»Р°РµРј Р·РґРІРёРі РЅР° +48
                    else if ((uint8_t)*str == 0x81) { buff_char = 0xA8; break; }        // Р±Р°Р№С‚ СЃРёРјРІРѕР»Р° РЃ ( РµСЃР»Рё РЅСѓР¶РЅС„ РµС‰Рµ СЃРёРјРІРѕР»С‹ РґРѕР±Р°РІР»СЏРµРј С‚СѓС‚ Рё РІ С„СѓРЅРєС†РёРё DrawChar() )
                    else if ((uint8_t)*str == 0x84) { buff_char = 0xAA; break; }        // Р±Р°Р№С‚ СЃРёРјРІРѕР»Р° Р„ ( РµСЃР»Рё РЅСѓР¶РЅС„ РµС‰Рµ СЃРёРјРІРѕР»С‹ РґРѕР±Р°РІР»СЏРµРј С‚СѓС‚ Рё РІ С„СѓРЅРєС†РёРё DrawChar() )
                    else if ((uint8_t)*str == 0x86) { buff_char = 0xB2; break; }        // Р±Р°Р№С‚ СЃРёРјРІРѕР»Р° Р† ( РµСЃР»Рё РЅСѓР¶РЅС„ РµС‰Рµ СЃРёРјРІРѕР»С‹ РґРѕР±Р°РІР»СЏРµРј С‚СѓС‚ Рё РІ С„СѓРЅРєС†РёРё DrawChar() )
                    else if ((uint8_t)*str == 0x87) { buff_char = 0xAF; break; }        // Р±Р°Р№С‚ СЃРёРјРІРѕР»Р° Р‡ ( РµСЃР»Рё РЅСѓР¶РЅС„ РµС‰Рµ СЃРёРјРІРѕР»С‹ РґРѕР±Р°РІР»СЏРµРј С‚СѓС‚ Рё РІ С„СѓРЅРєС†РёРё DrawChar() )
                    break;
                }
                case 0xD1: {
                    // СѓРІРµР»РёС‡РёРІР°РµРј РјР°СЃСЃРёРІ С‚Р°Рє РєР°Рє РЅР°Рј РЅСѓР¶РµРЅ РІС‚РѕСЂРѕР№ Р±Р°Р№С‚
                    str++;
                    // РїСЂРѕРІРµСЂСЏРµРј РІС‚РѕСЂРѕР№ Р±Р°Р№С‚ С‚Р°Рј СЃР°Рј СЃРёРјРІРѕР»
                    if ((uint8_t)*str >= 0x80 && (uint8_t)*str <= 0x8F){ buff_char = (*str) + 0x70; }   // Р±Р°Р№С‚ СЃРёРјРІРѕР»РѕРІ Рї...СЏ    РµР»Р°РµРј Р·РґРІРёРі РЅР° +112
                    else if ((uint8_t)*str == 0x91) { buff_char = 0xB8; break; }        // Р±Р°Р№С‚ СЃРёРјРІРѕР»Р° С‘ ( РµСЃР»Рё РЅСѓР¶РЅС„ РµС‰Рµ СЃРёРјРІРѕР»С‹ РґРѕР±Р°РІР»СЏРµРј С‚СѓС‚ Рё РІ С„СѓРЅРєС†РёРё DrawChar() )
                    else if ((uint8_t)*str == 0x94) { buff_char = 0xBA; break; }        // Р±Р°Р№С‚ СЃРёРјРІРѕР»Р° С” ( РµСЃР»Рё РЅСѓР¶РЅС„ РµС‰Рµ СЃРёРјРІРѕР»С‹ РґРѕР±Р°РІР»СЏРµРј С‚СѓС‚ Рё РІ С„СѓРЅРєС†РёРё DrawChar() )
                    else if ((uint8_t)*str == 0x96) { buff_char = 0xB3; break; }        // Р±Р°Р№С‚ СЃРёРјРІРѕР»Р° С– ( РµСЃР»Рё РЅСѓР¶РЅС„ РµС‰Рµ СЃРёРјРІРѕР»С‹ РґРѕР±Р°РІР»СЏРµРј С‚СѓС‚ Рё РІ С„СѓРЅРєС†РёРё DrawChar() )
                    else if ((uint8_t)*str == 0x97) { buff_char = 0xBF; break; }        // Р±Р°Р№С‚ СЃРёРјРІРѕР»Р° С— ( РµСЃР»Рё РЅСѓР¶РЅС„ РµС‰Рµ СЃРёРјРІРѕР»С‹ РґРѕР±Р°РІР»СЏРµРј С‚СѓС‚ Рё РІ С„СѓРЅРєС†РёРё DrawChar() )
                    break;
                }
            }
            //------------------------------------------------------------------------------------------------
            // СѓРјРµРЅСЊС€Р°РµРј РµС‰Рµ РїРµСЂРµРјРµРЅРЅСѓСЋ С‚Р°Рє РєР°Рє РёР·СЂР°СЃС…РѕРґС‹РІР°Р»Рё 2 Р±Р°Р№С‚Р° РґР»СЏ РєРёСЂРёР»Р»РёС†С‹
            len--;

            GC9A01_DrawChar(x, y, TextColor, BgColor, TransparentBg, Font, multiplier, buff_char);
        }
        //---------------------------------------------------------------------
        else{
            GC9A01_DrawChar(x, y, TextColor, BgColor, TransparentBg, Font, multiplier, *str);
        }

        x = x + (Font->FontWidth * multiplier);
        /* Increase string pointer */
        str++;
    }
}
//==============================================================================


//==============================================================================
// РџСЂРѕС†РµРґСѓСЂР° СЂРёСЃРѕРІР°РЅРёСЏ СЃРёРјРІРѕР»Р° СЃ СѓРєР°Р·Р°РЅС‹Рј СѓРіР»РѕРј ( 1 Р±СѓРєРІР° РёР»Рё Р·РЅР°Рє )
//==============================================================================
void GC9A01_DrawCharWithAngle(uint16_t x, uint16_t y, uint16_t TextColor, uint16_t BgColor, uint8_t TransparentBg, FontDef_t* Font, uint8_t multiplier, double angle_degrees, unsigned char ch){

    uint32_t i, b, j;

    uint32_t X = x, Y = y;

    uint8_t xx, yy;

    // РџСЂРµРѕР±СЂР°Р·СѓРµРј СѓРіРѕР» РІ СЂР°РґРёР°РЅС‹
    double radians = (360.0 - angle_degrees) * PI / 180.0;

    // Р’С‹С‡РёСЃР»СЏРµРј РјР°С‚СЂРёС†Сѓ РїРѕРІРѕСЂРѕС‚Р°
    double cosTheta = cos(radians);
    double sinTheta = sin(radians);

    // РџРµСЂРµРјРµРЅРЅС‹Рµ РґР»СЏ РїСЂРµРѕР±СЂР°Р·РѕРІР°РЅРЅС‹С… РєРѕРѕСЂРґРёРЅР°С‚
    double newX, newY;

    if( multiplier < 1 ){
        multiplier = 1;
    }

    /* Check available space in LCD */
    if (GC9A01_Width >= ( x + Font->FontWidth) || GC9A01_Height >= ( y + Font->FontHeight)){

            /* Go through font */
            for (i = 0; i < Font->FontHeight; i++) {

                if( ch < 127 ){
                    b = Font->data[(ch - 32) * Font->FontHeight + i];
                }

                else if( (uint8_t) ch > 191 ){
                    // +96 СЌС‚Рѕ С‚Р°Рє РєР°Рє Р»Р°С‚РёРЅСЃРєРёРµ СЃРёРјРІРѕР»С‹ Рё Р·РЅР°РєРё РІ С€СЂРёС„С‚Р°С… Р·Р°РЅРёРјР°СЋС‚ 96 РїРѕР·РёС†РёР№
                    // Рё РµСЃР»Рё РІ С€СЂРёС„С‚Рµ РєРѕС‚РѕСЂС‹Р№ СЃРѕРґРµСЂР¶РёС‚ СЃРїРµСЂРІР° Р»Р°С‚РёРЅРёС†Сѓ Рё СЃРїРµС† СЃРёРјРІРѕР»С‹ Рё РїРѕС‚РѕРј
                    // С‚РѕР»СЊРєРѕ РєРёСЂРёР»РёС†Сѓ С‚Рѕ РЅСѓР¶РЅРѕ РґРѕР±Р°РІР»СЏС‚СЊ 95 РµСЃР»Рё С€СЂРёС„С‚
                    // СЃРѕРґРµСЂР¶РёС‚ С‚РѕР»СЊРєРѕ РєРёСЂРёР»РёС†Сѓ С‚Рѕ +96 РЅРµ РЅСѓР¶РЅРѕ
                    b = Font->data[((ch - 192) + 96) * Font->FontHeight + i];
                }

                else if( (uint8_t) ch == 168 ){ // 168 СЃРёРјРІРѕР» РїРѕ ASCII - РЃ
                    // 160 СЌР»Р»РµРјРµРЅС‚ ( СЃРёРјРІРѕР» РЃ )
                    b = Font->data[( 160 ) * Font->FontHeight + i];
                }

                else if( (uint8_t) ch == 184 ){ // 184 СЃРёРјРІРѕР» РїРѕ ASCII - С‘
                    // 161 СЌР»Р»РµРјРµРЅС‚  ( СЃРёРјРІРѕР» С‘ )
                    b = Font->data[( 161 ) * Font->FontHeight + i];
                }
                //-------------------------------------------------------------------

                //----  РЈРєСЂР°РёРЅСЃРєР°СЏ СЂР°СЃРєР»Р°РґРєР° ----------------------------------------------------
                else if( (uint8_t) ch == 170 ){ // 168 СЃРёРјРІРѕР» РїРѕ ASCII - Р„
                    // 162 СЌР»Р»РµРјРµРЅС‚ ( СЃРёРјРІРѕР» Р„ )
                    b = Font->data[( 162 ) * Font->FontHeight + i];
                }
                else if( (uint8_t) ch == 175 ){ // 184 СЃРёРјРІРѕР» РїРѕ ASCII - Р‡
                    // 163 СЌР»Р»РµРјРµРЅС‚  ( СЃРёРјРІРѕР» Р‡ )
                    b = Font->data[( 163 ) * Font->FontHeight + i];
                }
                else if( (uint8_t) ch == 178 ){ // 168 СЃРёРјРІРѕР» РїРѕ ASCII - Р†
                    // 164 СЌР»Р»РµРјРµРЅС‚ ( СЃРёРјРІРѕР» Р† )
                    b = Font->data[( 164 ) * Font->FontHeight + i];
                }
                else if( (uint8_t) ch == 179 ){ // 184 СЃРёРјРІРѕР» РїРѕ ASCII - С–
                    // 165 СЌР»Р»РµРјРµРЅС‚  ( СЃРёРјРІРѕР» С– )
                    b = Font->data[( 165 ) * Font->FontHeight + i];
                }
                else if( (uint8_t) ch == 186 ){ // 184 СЃРёРјРІРѕР» РїРѕ ASCII - С”
                    // 166 СЌР»Р»РµРјРµРЅС‚  ( СЃРёРјРІРѕР» С” )
                    b = Font->data[( 166 ) * Font->FontHeight + i];
                }
                else if( (uint8_t) ch == 191 ){ // 168 СЃРёРјРІРѕР» РїРѕ ASCII - С—
                    // 167 СЌР»Р»РµРјРµРЅС‚ ( СЃРёРјРІРѕР» С— )
                    b = Font->data[( 167 ) * Font->FontHeight + i];
                }
                //-----------------------------------------------------------------------------

                for (j = 0; j < Font->FontWidth; j++) {
                    if ((b << j) & 0x8000) {
                            // РџСЂРёРјРµРЅСЏРµРј РїРѕРІРѕСЂРѕС‚ Рє РєРѕРѕСЂРґРёРЅР°С‚Р°Рј
                            newX = cosTheta * (X - x) - sinTheta * (Y - y) + x;
                            newY = sinTheta * (X - x) + cosTheta * (Y - y) + y;

                            for (yy = 0; yy < multiplier; yy++) {
                                    for (xx = 0; xx < multiplier; xx++) {
                                            GC9A01_DrawPixel(newX + xx, newY + yy, TextColor);
                                    }
                            }
                    } else if (TransparentBg) {
                            // РђРЅР°Р»РѕРіРёС‡РЅРѕ РґР»СЏ С„РѕРЅР°
                            newX = cosTheta * (X - x) - sinTheta * (Y - y) + x + 0.5;
                            newY = sinTheta * (X - x) + cosTheta * (Y - y) + y + 0.5;

                            for (yy = 0; yy < multiplier; yy++) {
                                    for (xx = 0; xx < multiplier; xx++) {
                                            GC9A01_DrawPixel(newX + xx, newY + yy, BgColor);
                                    }
                            }
                    }
                    X = X + multiplier;
                }
                X = x;
                Y = Y + multiplier;
            }
    }
}
//==============================================================================


//==============================================================================
// РџСЂРѕС†РµРґСѓСЂР° СЂРёСЃРѕРІР°РЅРёСЏ СЃС‚СЂРѕРєРё СЃ СѓРєР°Р·Р°РЅС‹Рј СѓРіР»РѕРј
//==============================================================================
void GC9A01_printWithAngle(uint16_t x, uint16_t y, uint16_t TextColor, uint16_t BgColor, uint8_t TransparentBg, FontDef_t* Font, uint8_t multiplier, double angle_degrees, char *str){

    if( multiplier < 1 ){
        multiplier = 1;
    }

    unsigned char buff_char;

    uint16_t len = strlen(str);

    while (len--) {

        //---------------------------------------------------------------------
        // РїСЂРѕРІРµСЂРєР° РЅР° РєРёСЂРёР»Р»РёС†Сѓ UTF-8, РµСЃР»Рё Р»Р°С‚РёРЅРёС†Р° С‚Рѕ РїСЂРѕРїСѓСЃРєР°РµРј if
        // Р Р°СЃС€РёСЂРµРЅРЅС‹Рµ СЃРёРјРІРѕР»С‹ ASCII Win-1251 РєРёСЂРёР»Р»РёС†Р° (РєРѕРґ СЃРёРјРІРѕР»Р° 128-255)
        // РїСЂРѕРІРµСЂСЏРµРј РїРµСЂРІС‹Р№ Р±Р°Р№С‚ РёР· РґРІСѓС… ( С‚Р°Рє РєР°Рє UTF-8 РµС‚Рѕ РґРІР° Р±Р°Р№С‚Р° )
        // РµСЃР»Рё РѕРЅ Р±РѕР»СЊС€Рµ Р»РёР±Рѕ СЂР°РІРµРЅ 0xC0 ( РїРµСЂРІС‹Р№ Р±Р°Р№С‚ РІ РєРёСЂРёР»Р»РµС†Рµ Р±СѓРґРµС‚ СЂР°РІРµРЅ 0xD0 Р»РёР±Рѕ 0xD1 РёРјРµРЅРЅРѕ РІ Р°Р»С„Р°РІРёС‚Рµ )
        if ( (uint8_t)*str >= 0xC0 ){   // РєРѕРґ 0xC0 СЃРѕРѕС‚РІРµС‚СЃС‚РІСѓРµС‚ СЃРёРјРІРѕР»Сѓ РєРёСЂРёР»Р»РёС†Р° 'A' РїРѕ ASCII Win-1251

            // РїСЂРѕРІРµСЂСЏРµРј РєР°РєРѕР№ РёРјРµРЅРЅРѕ Р±Р°Р№С‚ РїРµСЂРІС‹Р№ 0xD0 Р»РёР±Рѕ 0xD1---------------------------------------------
            switch ((uint8_t)*str) {
                case 0xD0: {
                    // СѓРІРµР»РёС‡РёРІР°РµРј РјР°СЃСЃРёРІ С‚Р°Рє РєР°Рє РЅР°Рј РЅСѓР¶РµРЅ РІС‚РѕСЂРѕР№ Р±Р°Р№С‚
                    str++;
                    // РїСЂРѕРІРµСЂСЏРµРј РІС‚РѕСЂРѕР№ Р±Р°Р№С‚ С‚Р°Рј СЃР°Рј СЃРёРјРІРѕР»
                    if ((uint8_t)*str >= 0x90 && (uint8_t)*str <= 0xBF){ buff_char = (*str) + 0x30; }   // Р±Р°Р№С‚ СЃРёРјРІРѕР»РѕРІ Рђ...РЇ Р°...Рї  РґРµР»Р°РµРј Р·РґРІРёРі РЅР° +48
                    else if ((uint8_t)*str == 0x81) { buff_char = 0xA8; break; }        // Р±Р°Р№С‚ СЃРёРјРІРѕР»Р° РЃ ( РµСЃР»Рё РЅСѓР¶РЅС„ РµС‰Рµ СЃРёРјРІРѕР»С‹ РґРѕР±Р°РІР»СЏРµРј С‚СѓС‚ Рё РІ С„СѓРЅРєС†РёРё DrawChar() )
                    else if ((uint8_t)*str == 0x84) { buff_char = 0xAA; break; }        // Р±Р°Р№С‚ СЃРёРјРІРѕР»Р° Р„ ( РµСЃР»Рё РЅСѓР¶РЅС„ РµС‰Рµ СЃРёРјРІРѕР»С‹ РґРѕР±Р°РІР»СЏРµРј С‚СѓС‚ Рё РІ С„СѓРЅРєС†РёРё DrawChar() )
                    else if ((uint8_t)*str == 0x86) { buff_char = 0xB2; break; }        // Р±Р°Р№С‚ СЃРёРјРІРѕР»Р° Р† ( РµСЃР»Рё РЅСѓР¶РЅС„ РµС‰Рµ СЃРёРјРІРѕР»С‹ РґРѕР±Р°РІР»СЏРµРј С‚СѓС‚ Рё РІ С„СѓРЅРєС†РёРё DrawChar() )
                    else if ((uint8_t)*str == 0x87) { buff_char = 0xAF; break; }        // Р±Р°Р№С‚ СЃРёРјРІРѕР»Р° Р‡ ( РµСЃР»Рё РЅСѓР¶РЅС„ РµС‰Рµ СЃРёРјРІРѕР»С‹ РґРѕР±Р°РІР»СЏРµРј С‚СѓС‚ Рё РІ С„СѓРЅРєС†РёРё DrawChar() )
                    break;
                }
                case 0xD1: {
                    // СѓРІРµР»РёС‡РёРІР°РµРј РјР°СЃСЃРёРІ С‚Р°Рє РєР°Рє РЅР°Рј РЅСѓР¶РµРЅ РІС‚РѕСЂРѕР№ Р±Р°Р№С‚
                    str++;
                    // РїСЂРѕРІРµСЂСЏРµРј РІС‚РѕСЂРѕР№ Р±Р°Р№С‚ С‚Р°Рј СЃР°Рј СЃРёРјРІРѕР»
                    if ((uint8_t)*str >= 0x80 && (uint8_t)*str <= 0x8F){ buff_char = (*str) + 0x70; }   // Р±Р°Р№С‚ СЃРёРјРІРѕР»РѕРІ Рї...СЏ    РµР»Р°РµРј Р·РґРІРёРі РЅР° +112
                    else if ((uint8_t)*str == 0x91) { buff_char = 0xB8; break; }        // Р±Р°Р№С‚ СЃРёРјРІРѕР»Р° С‘ ( РµСЃР»Рё РЅСѓР¶РЅС„ РµС‰Рµ СЃРёРјРІРѕР»С‹ РґРѕР±Р°РІР»СЏРµРј С‚СѓС‚ Рё РІ С„СѓРЅРєС†РёРё DrawChar() )
                    else if ((uint8_t)*str == 0x94) { buff_char = 0xBA; break; }        // Р±Р°Р№С‚ СЃРёРјРІРѕР»Р° С” ( РµСЃР»Рё РЅСѓР¶РЅС„ РµС‰Рµ СЃРёРјРІРѕР»С‹ РґРѕР±Р°РІР»СЏРµРј С‚СѓС‚ Рё РІ С„СѓРЅРєС†РёРё DrawChar() )
                    else if ((uint8_t)*str == 0x96) { buff_char = 0xB3; break; }        // Р±Р°Р№С‚ СЃРёРјРІРѕР»Р° С– ( РµСЃР»Рё РЅСѓР¶РЅС„ РµС‰Рµ СЃРёРјРІРѕР»С‹ РґРѕР±Р°РІР»СЏРµРј С‚СѓС‚ Рё РІ С„СѓРЅРєС†РёРё DrawChar() )
                    else if ((uint8_t)*str == 0x97) { buff_char = 0xBF; break; }        // Р±Р°Р№С‚ СЃРёРјРІРѕР»Р° С— ( РµСЃР»Рё РЅСѓР¶РЅС„ РµС‰Рµ СЃРёРјРІРѕР»С‹ РґРѕР±Р°РІР»СЏРµРј С‚СѓС‚ Рё РІ С„СѓРЅРєС†РёРё DrawChar() )
                    break;
                }
            }
            //------------------------------------------------------------------------------------------------
            // СѓРјРµРЅСЊС€Р°РµРј РµС‰Рµ РїРµСЂРµРјРµРЅРЅСѓСЋ С‚Р°Рє РєР°Рє РёР·СЂР°СЃС…РѕРґС‹РІР°Р»Рё 2 Р±Р°Р№С‚Р° РґР»СЏ РєРёСЂРёР»Р»РёС†С‹
            len--;

            GC9A01_DrawCharWithAngle(x, y, TextColor, BgColor, TransparentBg, Font, multiplier, angle_degrees, buff_char);
        }
        //---------------------------------------------------------------------
        else{
            GC9A01_DrawCharWithAngle(x, y, TextColor, BgColor, TransparentBg, Font, multiplier, angle_degrees, *str);
        }
        // РЎРјРµС‰Р°РµРј РЅР°С‡Р°Р»СЊРЅС‹Рµ РєРѕРѕСЂРґРёРЅР°С‚С‹ СЃ РєР°Р¶РґС‹Рј СЃРёРјРІРѕР»РѕРј СЃ СѓС‡РµС‚РѕРј СѓРіР»Р°
    x += (Font->FontWidth * multiplier * cos((360.0 - angle_degrees) * PI / 180.0) + 0.5);
    y += (Font->FontWidth * multiplier * sin((360.0 - angle_degrees) * PI / 180.0) + 0.5);

        /* Increase string pointer */
        str++;
    }
}
//==============================================================================


//==============================================================================
// РџСЂРѕС†РµРґСѓСЂР° СЂРѕС‚Р°С†РёРё ( РїРѕР»РѕР¶РµРЅРёРµ ) РґРёСЃРїР»РµСЏ
//==============================================================================
// СѓСЃС‚Р°РЅРѕРІРєР° СЂРѕС‚Р°С†РёРё РґРёСЃРїР»РµСЏ, РѕС‚Р·РµСЂРєР°Р»РёРІР°РЅРёРµ РїРѕ РІРµСЂС‚РёРєР°Р»Рё Рё РіРѕСЂРёР·РѕРЅС‚Р°Р»Рё Рё СЂРµР¶РёРј С†РІРµС‚РѕРїРµСЂРµРґР°С†Рё
// РїРµСЂРІС‹Р№ РїР°СЂР°РјРµС‚СЂ СЂРѕС‚Р°С†РёСЏ Р·РЅР°С‡РµРЅРёСЏ РѕС‚ 0 РґРѕ 7
// РІС‚РѕСЂРѕР№ РїР°СЂР°РјРµС‚СЂ РѕС‚Р·РµСЂРєР°Р»РёРІР°РЅРёРµ РїРѕ РІРµСЂС‚РёРєР°Р»Рё Р·РЅР°С‡РµРЅРёСЏ 0-РІС‹РєР» 1-РІРєР»
// С‚СЂРµС‚РёР№ РїР°СЂР°РјРµС‚СЂ РѕС‚Р·РµСЂРєР°Р»РёРІР°РЅРёРµ РїРѕ РіРѕСЂРёР·РѕРЅС‚Р°Р»Рё Р·РЅР°С‡РµРЅРёСЏ 0-РІС‹РєР» 1-РІРєР»
// РїРѕ СѓРјРѕР»С‡Р°РЅРёСЋ СЃС‚РѕРёС‚ #define GC9A01_DEF_ROTATION  ( 0, 0, 0 )
void GC9A01_rotation(uint8_t Rotation, uint8_t VertMirror, uint8_t HorizMirror){

        GC9A01_Select();

        uint8_t Value;
        Rotation &= 7;

        GC9A01_SendCmd(GC9A01_MADCTL);

        switch (Rotation) {
        case 0:
            Value = 0;
            break;
        case 1:
            Value = GC9A01_MADCTL_MX;
            break;
        case 2:
            Value = GC9A01_MADCTL_MY;
            break;
        case 3:
            Value = GC9A01_MADCTL_MX | GC9A01_MADCTL_MY;
            break;
        case 4:
            Value = GC9A01_MADCTL_MV;
            break;
        case 5:
            Value = GC9A01_MADCTL_MV | GC9A01_MADCTL_MX;
            break;
        case 6:
            Value = GC9A01_MADCTL_MV | GC9A01_MADCTL_MY;
            break;
        case 7:
            Value = GC9A01_MADCTL_MV | GC9A01_MADCTL_MX | GC9A01_MADCTL_MY;
            break;
        }

        if (VertMirror){
            Value = GC9A01_MADCTL_ML;
        }

        if (HorizMirror){
            Value = GC9A01_MADCTL_MH;
        }

        // RGB or BGR
        Value |= GC9A01_DEF_ROTATION;

        GC9A01_SendData(Value);

        GC9A01_Unselect();
}
//==============================================================================


//==============================================================================
// РџСЂРѕС†РµРґСѓСЂР° СЂРёСЃРѕРІР°РЅРёСЏ РёРєРѕРЅРєРё РјРѕРЅРѕС…СЂРѕРјРЅРѕР№
//==============================================================================
void GC9A01_DrawBitmap(int16_t x, int16_t y, const unsigned char* bitmap, int16_t w, int16_t h, uint16_t color){

    int16_t byteWidth = (w + 7) / 8;    // Bitmap scanline pad = whole byte
    uint8_t byte = 0;

    for(int16_t j=0; j<h; j++, y++){

        for(int16_t i=0; i<w; i++){

            if(i & 7){
               byte <<= 1;
            }
            else{
               byte = (*(const unsigned char *)(&bitmap[j * byteWidth + i / 8]));
            }

            if(byte & 0x80){
                GC9A01_DrawPixel(x+i, y, color);
            }
        }
    }
}
//==============================================================================


//==============================================================================
// РџСЂРѕС†РµРґСѓСЂР° СЂРёСЃРѕРІР°РЅРёСЏ РёРєРѕРЅРєРё РјРѕРЅРѕС…СЂРѕРјРЅРѕР№ СЃ СѓРєР°Р·Р°РЅС‹Рј СѓРіР»РѕРј
//==============================================================================
void GC9A01_DrawBitmapWithAngle(int16_t x, int16_t y, const unsigned char* bitmap, int16_t w, int16_t h, uint16_t color, double angle_degrees) {
    // РџСЂРµРѕР±СЂР°Р·РѕРІР°РЅРёРµ СѓРіР»Р° РІ СЂР°РґРёР°РЅС‹
    double angle_radians = (360.0 - angle_degrees) * PI / 180.0;

    // Р’С‹С‡РёСЃР»РµРЅРёРµ РјР°С‚СЂРёС†С‹ РїРѕРІРѕСЂРѕС‚Р°
    double cosTheta = cos(angle_radians);
    double sinTheta = sin(angle_radians);

    // РЁРёСЂРёРЅР° Рё РІС‹СЃРѕС‚Р° РїРѕРІРµСЂРЅСѓС‚РѕРіРѕ РёР·РѕР±СЂР°Р¶РµРЅРёСЏ
    int16_t rotatedW = round(fabs(w * cosTheta) + fabs(h * sinTheta));
    int16_t rotatedH = round(fabs(h * cosTheta) + fabs(w * sinTheta));

    // Р’С‹С‡РёСЃР»РµРЅРёРµ С†РµРЅС‚СЂР°Р»СЊРЅС‹С… РєРѕРѕСЂРґРёРЅР°С‚ РїРѕРІРµСЂРЅСѓС‚РѕРіРѕ РёР·РѕР±СЂР°Р¶РµРЅРёСЏ
    int16_t centerX = x + w / 2;
    int16_t centerY = y + h / 2;

    // РџСЂРѕС…РѕРґРёРј РїРѕ РєР°Р¶РґРѕРјСѓ РїРёРєСЃРµР»СЋ РёР·РѕР±СЂР°Р¶РµРЅРёСЏ Рё СЂРёСЃСѓРµРј РµРіРѕ РїРѕРІРµСЂРЅСѓС‚С‹Рј
    for (int16_t j = 0; j < h; j++) {
        for (int16_t i = 0; i < w; i++) {
            // Р’С‹С‡РёСЃР»РµРЅРёРµ СЃРјРµС‰РµРЅРёСЏ РѕС‚ С†РµРЅС‚СЂР°
            int16_t offsetX = i - w / 2;
            int16_t offsetY = j - h / 2;

            // РџСЂРёРјРµРЅРµРЅРёРµ РјР°С‚СЂРёС†С‹ РїРѕРІРѕСЂРѕС‚Р°
            int16_t rotatedX = round(centerX + offsetX * cosTheta - offsetY * sinTheta);
            int16_t rotatedY = round(centerY + offsetX * sinTheta + offsetY * cosTheta);

            // РџСЂРѕРІРµСЂРєР° РЅР°С…РѕРґРёС‚СЃСЏ Р»Рё РїРёРєСЃРµР»СЊ РІ РїСЂРµРґРµР»Р°С… СЌРєСЂР°РЅР°
            if (rotatedX >= 0 && rotatedX < GC9A01_Width && rotatedY >= 0 && rotatedY < GC9A01_Height) {
                // РџРѕР»СѓС‡РµРЅРёРµ С†РІРµС‚Р° РїРёРєСЃРµР»СЏ РёР· РёСЃС…РѕРґРЅРѕРіРѕ РёР·РѕР±СЂР°Р¶РµРЅРёСЏ
                uint8_t byteWidth = (w + 7) / 8;
                uint8_t byte = (*(const unsigned char*)(&bitmap[j * byteWidth + i / 8]));
                if (byte & (0x80 >> (i & 7))) {
                    // Р РёСЃРѕРІР°РЅРёРµ РїРёРєСЃРµР»СЏ РЅР° СЌРєСЂР°РЅРµ
                    GC9A01_DrawPixel(rotatedX, rotatedY, color);
                }
            }
        }
    }
}
//==============================================================================


//==============================================================================
// РџСЂРѕС†РµРґСѓСЂР° СЂРёСЃРѕРІР°РЅРёСЏ РїСЂСЏРјРѕСѓРіРѕР»СЊРЅРёРє СЃ Р·Р°РєСЂСѓРіР»РµРЅРЅС–РјРё РєСЂР°СЏРјРё ( Р·Р°РїРѕР»РЅРµРЅС‹Р№ )
//==============================================================================
void GC9A01_DrawFillRoundRect(int16_t x, int16_t y, uint16_t width, uint16_t height, int16_t cornerRadius, uint16_t color) {

    int16_t max_radius = ((width < height) ? width : height) / 2; // 1/2 minor axis
  if (cornerRadius > max_radius){
    cornerRadius = max_radius;
    }

  GC9A01_DrawRectangleFilled(x + cornerRadius, y, x + cornerRadius + width - 2 * cornerRadius, y + height, color);
  // draw four corners
  GC9A01_DrawFillCircleHelper(x + width - cornerRadius - 1, y + cornerRadius, cornerRadius, 1, height - 2 * cornerRadius - 1, color);
  GC9A01_DrawFillCircleHelper(x + cornerRadius, y + cornerRadius, cornerRadius, 2, height - 2 * cornerRadius - 1, color);
}
//==============================================================================

//==============================================================================
// РџСЂРѕС†РµРґСѓСЂР° СЂРёСЃРѕРІР°РЅРёСЏ РїРѕР»РѕРІРёРЅС‹ РѕРєСЂСѓР¶РЅРѕСЃС‚Рё ( РїСЂР°РІР°СЏ РёР»Рё Р»РµРІР°СЏ ) ( Р·Р°РїРѕР»РЅРµРЅС‹Р№ )
//==============================================================================
void GC9A01_DrawFillCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t corners, int16_t delta, uint16_t color) {

  int16_t f = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x = 0;
  int16_t y = r;
  int16_t px = x;
  int16_t py = y;

  delta++; // Avoid some +1's in the loop

  while (x < y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;

    if (x < (y + 1)) {
      if (corners & 1){
        GC9A01_DrawLine(x0 + x, y0 - y, x0 + x, y0 - y - 1 + 2 * y + delta, color);
            }
      if (corners & 2){
        GC9A01_DrawLine(x0 - x, y0 - y, x0 - x, y0 - y - 1 + 2 * y + delta, color);
            }
    }
    if (y != py) {
      if (corners & 1){
        GC9A01_DrawLine(x0 + py, y0 - px, x0 + py, y0 - px - 1 + 2 * px + delta, color);
            }
      if (corners & 2){
        GC9A01_DrawLine(x0 - py, y0 - px, x0 - py, y0 - px - 1 + 2 * px + delta, color);
            }
            py = y;
    }
    px = x;
  }
}
//==============================================================================

//==============================================================================
// РџСЂРѕС†РµРґСѓСЂР° СЂРёСЃРѕРІР°РЅРёСЏ С‡РµС‚РІРµСЂС‚Рё РѕРєСЂСѓР¶РЅРѕСЃС‚Рё (Р·Р°РєСЂСѓРіР»РµРЅРёРµ, РґСѓРіР°) ( С€РёСЂРёРЅР° 1 РїРёРєСЃРµР»СЊ)
//==============================================================================
void GC9A01_DrawCircleHelper(int16_t x0, int16_t y0, int16_t radius, int8_t quadrantMask, uint16_t color)
{
    int16_t f = 1 - radius ;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * radius;
    int16_t x = 0;
    int16_t y = radius;

    while (x <= y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }

        x++;
        ddF_x += 2;
        f += ddF_x;

        if (quadrantMask & 0x4) {
            GC9A01_DrawPixel(x0 + x, y0 + y, color);
            GC9A01_DrawPixel(x0 + y, y0 + x, color);;
        }
        if (quadrantMask & 0x2) {
            GC9A01_DrawPixel(x0 + x, y0 - y, color);
            GC9A01_DrawPixel(x0 + y, y0 - x, color);
        }
        if (quadrantMask & 0x8) {
            GC9A01_DrawPixel(x0 - y, y0 + x, color);
            GC9A01_DrawPixel(x0 - x, y0 + y, color);
        }
        if (quadrantMask & 0x1) {
            GC9A01_DrawPixel(x0 - y, y0 - x, color);
            GC9A01_DrawPixel(x0 - x, y0 - y, color);
        }
    }
}
//==============================================================================

//==============================================================================
// РџСЂРѕС†РµРґСѓСЂР° СЂРёСЃРѕРІР°РЅРёСЏ РїСЂСЏРјРѕСѓРіРѕР»СЊРЅРёРє СЃ Р·Р°РєСЂСѓРіР»РµРЅРЅС–РјРё РєСЂР°СЏРјРё ( РїСѓСЃС‚РѕС‚РµР»С‹Р№ )
//==============================================================================
void GC9A01_DrawRoundRect(int16_t x, int16_t y, uint16_t width, uint16_t height, int16_t cornerRadius, uint16_t color) {

    int16_t max_radius = ((width < height) ? width : height) / 2; // 1/2 minor axis
  if (cornerRadius > max_radius){
    cornerRadius = max_radius;
    }

  GC9A01_DrawLine(x + cornerRadius, y, x + cornerRadius + width -1 - 2 * cornerRadius, y, color);         // Top
  GC9A01_DrawLine(x + cornerRadius, y + height - 1, x + cornerRadius + width - 1 - 2 * cornerRadius, y + height - 1, color); // Bottom
  GC9A01_DrawLine(x, y + cornerRadius, x, y + cornerRadius + height - 1 - 2 * cornerRadius, color);         // Left
  GC9A01_DrawLine(x + width - 1, y + cornerRadius, x + width - 1, y + cornerRadius + height - 1 - 2 * cornerRadius, color); // Right

  // draw four corners
    GC9A01_DrawCircleHelper(x + cornerRadius, y + cornerRadius, cornerRadius, 1, color);
  GC9A01_DrawCircleHelper(x + width - cornerRadius - 1, y + cornerRadius, cornerRadius, 2, color);
    GC9A01_DrawCircleHelper(x + width - cornerRadius - 1, y + height - cornerRadius - 1, cornerRadius, 4, color);
  GC9A01_DrawCircleHelper(x + cornerRadius, y + height - cornerRadius - 1, cornerRadius, 8, color);
}
//==============================================================================

//==============================================================================
// РџСЂРѕС†РµРґСѓСЂР° СЂРёСЃРѕРІР°РЅРёСЏ Р»РёРЅРёСЏ С‚РѕР»СЃС‚Р°СЏ ( РїРѕСЃР»РµРґРЅРёР№ РїР°СЂР°РјРµС‚СЂ С‚РѕР»С‰РёРЅР° )
//==============================================================================
void GC9A01_DrawLineThick(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color, uint8_t thick) {
    const int16_t deltaX = abs(x2 - x1);
    const int16_t deltaY = abs(y2 - y1);
    const int16_t signX = x1 < x2 ? 1 : -1;
    const int16_t signY = y1 < y2 ? 1 : -1;

    int16_t error = deltaX - deltaY;

    if (thick > 1){
        GC9A01_DrawCircleFilled(x2, y2, thick >> 1, color);
    }
    else{
        GC9A01_DrawPixel(x2, y2, color);
    }

    while (x1 != x2 || y1 != y2) {
        if (thick > 1){
            GC9A01_DrawCircleFilled(x1, y1, thick >> 1, color);
        }
        else{
            GC9A01_DrawPixel(x1, y1, color);
        }

        const int16_t error2 = error * 2;
        if (error2 > -deltaY) {
            error -= deltaY;
            x1 += signX;
        }
        if (error2 < deltaX) {
            error += deltaX;
            y1 += signY;
        }
    }
}
//==============================================================================


//==============================================================================
// Р»РёРЅРёСЏ С‚РѕР»СЃС‚Р°СЏ РЅСѓР¶РЅРѕР№ РґР»РёРЅС‹ Рё СѓРєР°Р·Р°РЅС‹Рј СѓРіР»РѕРј РїРѕРІРѕСЂРѕС‚Р° (0-360) ( РїРѕСЃР»РµРґРЅРёР№ РїР°СЂР°РјРµС‚СЂ С‚РѕР»С€РёРЅР° )
//==============================================================================
void GC9A01_DrawLineThickWithAngle(int16_t x, int16_t y, int16_t length, double angle_degrees, uint16_t color, uint8_t thick) {
    double angleRad = (360.0 - angle_degrees) * PI / 180.0;
    int16_t x2 = x + (int16_t)(cos(angleRad) * length) + 0.5;
    int16_t y2 = y + (int16_t)(sin(angleRad) * length) + 0.5;

    GC9A01_DrawLineThick(x, y, x2, y2, color, thick);
}
//==============================================================================


//==============================================================================
// РџСЂРѕС†РµРґСѓСЂР° СЂРёСЃРѕРІР°РЅРёСЏ РґСѓРіР° С‚РѕР»СЃС‚Р°СЏ ( С‡Р°СЃС‚СЊ РєСЂСѓРіР° )
//==============================================================================
void GC9A01_DrawArc(int16_t x0, int16_t y0, int16_t radius, int16_t startAngle, int16_t endAngle, uint16_t color, uint8_t thick) {

    int16_t xLast = -1, yLast = -1;

    if (startAngle > endAngle) {
        // Р РёСЃРѕРІР°РЅРёРµ РїРµСЂРІРѕР№ С‡Р°СЃС‚Рё РґСѓРіРё РѕС‚ startAngle РґРѕ 360 РіСЂР°РґСѓСЃРѕРІ
        for (int16_t angle = startAngle; angle <= 360; angle += 2) {
            float angleRad = (float)(360 - angle) * PI / 180;
            int x = cos(angleRad) * radius + x0;
            int y = sin(angleRad) * radius + y0;

            if (xLast != -1 && yLast != -1) {
                if (thick > 1) {
                    GC9A01_DrawLineThick(xLast, yLast, x, y, color, thick);
                } else {
                    GC9A01_DrawLine(xLast, yLast, x, y, color);
                }
            }

            xLast = x;
            yLast = y;
        }

        // Р РёСЃРѕРІР°РЅРёРµ РІС‚РѕСЂРѕР№ С‡Р°СЃС‚Рё РґСѓРіРё РѕС‚ 0 РґРѕ endAngle
        for (int16_t angle = 0; angle <= endAngle; angle += 2) {
            float angleRad = (float)(360 - angle) * PI / 180;
            int x = cos(angleRad) * radius + x0;
            int y = sin(angleRad) * radius + y0;

            if (xLast != -1 && yLast != -1) {
                if (thick > 1) {
                    GC9A01_DrawLineThick(xLast, yLast, x, y, color, thick);
                } else {
                    GC9A01_DrawLine(xLast, yLast, x, y, color);
                }
            }

            xLast = x;
            yLast = y;
        }
    } else {
        // Р РёСЃРѕРІР°РЅРёРµ РґСѓРіРё РѕС‚ startAngle РґРѕ endAngle
        for (int16_t angle = startAngle; angle <= endAngle; angle += 2) {
            float angleRad = (float)(360 - angle) * PI / 180;
            int x = cos(angleRad) * radius + x0;
            int y = sin(angleRad) * radius + y0;

            if (xLast != -1 && yLast != -1) {
                if (thick > 1) {
                    GC9A01_DrawLineThick(xLast, yLast, x, y, color, thick);
                } else {
                    GC9A01_DrawLine(xLast, yLast, x, y, color);
                }
            }

            xLast = x;
            yLast = y;
        }
    }
}
//==============================================================================


#if FRAME_BUFFER
    //==============================================================================
    // РџСЂРѕС†РµРґСѓСЂР° РІС‹РІРѕРґР° Р±СѓС„С„РµСЂР° РєР°РґСЂР° РЅР° РґРёСЃРїР»РµР№
    //==============================================================================
    void GC9A01_Update(void){

            GC9A01_SetWindow(0, 0, GC9A01_Width-1, GC9A01_Height-1);

            GC9A01_Select();

            GC9A01_SendDataMASS((uint8_t*)buff_frame, sizeof(uint16_t)*GC9A01_Width*GC9A01_Height);

            GC9A01_Unselect();
    }
    //==============================================================================

    //==============================================================================
    // РџСЂРѕС†РµРґСѓСЂР° РѕС‡РёСЃС‚РєР° С‚РѕР»СЊРєРѕ Р±СѓС„РµСЂР° РєР°РґСЂР°  ( РїСЂРё РµС‚РѕРј СЃР°Рј СЌРєСЂР°РЅ РЅРµ РѕС‡РёС‰Р°РµС‚СЊСЃСЏ )
    //==============================================================================
    void GC9A01_ClearFrameBuffer(void){
        memset((uint8_t*)buff_frame, 0x00, GC9A01_Width*GC9A01_Height*2 );
    }
    //==============================================================================
#endif

//#########################################################################################################################
//#########################################################################################################################



/************************ (C) COPYRIGHT GKP *****END OF FILE****/
