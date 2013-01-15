/*
 * Author - Bertrand Songis <bsongis@gmail.com>
 * 
 * Based on th9x -> http://code.google.com/p/th9x/
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#ifdef NAMESPACE_IMPORT
#undef NAMESPACE_IMPORT

extern void StartEepromThread(const char *filename);
extern void StartMainThread(bool tests);
extern void StopEepromThread();
extern void StopMainThread();

extern volatile unsigned char pinb, pinc, pind, pine, ping, pinj, pinl;
extern uint8_t portb, portc;

#define INP_E_ID2     6
#define OUT_E_BUZZER  3
#define INP_E_AileDR  1
#define INP_E_ThrCt   0
#define INP_E_ElevDR  2
#define INP_E_Trainer 5
#define INP_E_Gear    4
#define INP_C_ThrCt   6
#define INP_C_ElevDR  6
#define INP_C_AileDR  7
#define INP_G_ID1     3
#define INP_G_RF_POW  1
#define INP_G_ThrCt   2
#define INP_G_RuddDR  0
#define INP_G_Gear    5

#define INP_P_KEY_EXT 5
#define INP_P_KEY_MEN 4
#define INP_P_KEY_LFT 3
#define INP_P_KEY_RGT 2
#define INP_P_KEY_UP  1
#define INP_P_KEY_DWN 0

#define INP_B_KEY_LFT 6
#define INP_B_Trainer 5
#define INP_B_KEY_RGT 5
#define INP_B_KEY_UP  4
#define INP_B_KEY_DWN 3
#define INP_B_KEY_EXT 2
#define INP_B_KEY_MEN 1
#define INP_B_ID2     4

#define OUT_C_LIGHT   0
#define OUT_B_LIGHT   7

extern uint8_t eeprom[];
extern int16_t g_anas[NUM_STICKS+NUM_POTS];
extern int16_t g_chans512[NUM_CHNOUT];

#if defined(PCBGRUVIN9X)
extern volatile uint8_t g_rotenc[2];
#endif

#if defined(PCBSKY9X)
extern volatile uint32_t g_rotenc[1];
#endif

#if defined(PCBX9DA)
extern uint8_t lcd_buf[128*64/8];
#else
extern uint8_t lcd_buf[212*64];
#endif

extern bool lcd_refresh;

extern void per10ms();
extern bool getSwitch(int8_t swtch, bool nc=0);

extern uint8_t getTrimFlightPhase(uint8_t idx, uint8_t phase);
extern void setTrimValue(uint8_t phase, uint8_t idx, int16_t trim);
extern int16_t getTrimValue(uint8_t phase, uint8_t idx);
extern uint8_t getFlightPhase();
extern bool hasExtendedTrims();
extern uint8_t main_thread_running;
extern char * main_thread_error;

#ifdef FRSKY
extern int8_t frskyStreaming;
#endif
#endif

#ifdef INIT_IMPORT
#undef INIT_IMPORT
#ifdef FRSKY
frskyStreaming = 20;
#endif
#endif

#ifdef SETVALUES_IMPORT
#undef SETVALUES_IMPORT
for (int i=0; i<4; i++)
  g_anas[i] = inputs.sticks[i];
for (int i=0; i<3; i++)
  g_anas[4+i] = inputs.pots[i];

#ifdef PCBGRUVIN9X
pinj = 0;
pinl = 0;
#endif

#if defined(PCBX9D)
// TODO
#elif defined(PCBSKY9X)
if (inputs.sThr) PIOC->PIO_PDSR |= (1<<20); else PIOC->PIO_PDSR &= ~(1<<20);
if (inputs.sRud) PIOA->PIO_PDSR |= (1<<15); else PIOA->PIO_PDSR &= ~(1<<15);
if (inputs.sEle) PIOC->PIO_PDSR |= (1<<31); else PIOC->PIO_PDSR &= ~(1<<31);
if (inputs.sAil) PIOA->PIO_PDSR |= (1<<2); else PIOA->PIO_PDSR &= ~(1<<2);
if (inputs.sGea) PIOC->PIO_PDSR |= (1<<16); else PIOC->PIO_PDSR &= ~(1<<16);
if (inputs.sTrn) PIOC->PIO_PDSR |= (1<<8); else PIOC->PIO_PDSR &= ~(1<<8);
#elif defined(PCBGRUVIN9X)
if (inputs.sRud) ping &= ~(1<<INP_G_RuddDR); else ping |= (1<<INP_G_RuddDR);
if (inputs.sEle) pinc &= ~(1<<INP_C_ElevDR); else pinc |= (1<<INP_C_ElevDR);
if (inputs.sThr) ping &= ~(1<<INP_G_ThrCt); else ping |= (1<<INP_G_ThrCt);
if (inputs.sAil) pinc &= ~(1<<INP_C_AileDR); else pinc |= (1<<INP_C_AileDR);
if (inputs.sGea) ping &= ~(1<<INP_G_Gear); else ping |= (1<<INP_G_Gear);
if (inputs.sTrn) pinb &= ~(1<<INP_B_Trainer); else pinb |= (1<<INP_B_Trainer);
#else
if (inputs.sRud) ping &= ~(1<<INP_G_RuddDR); else ping |= (1<<INP_G_RuddDR);
if (inputs.sEle) pine &= ~(1<<INP_E_ElevDR); else pine |= (1<<INP_E_ElevDR);
#if defined(JETI) || defined(FRSKY)
  if (inputs.sAil) pinc &= ~(1<<INP_C_AileDR); else pinc |= (1<<INP_C_AileDR);
  if (inputs.sThr) pinc &= ~(1<<INP_C_ThrCt); else pinc |= (1<<INP_C_ThrCt);
#else
  if (inputs.sAil) pine &= ~(1<<INP_E_AileDR); else pine |= (1<<INP_E_AileDR);
  if (inputs.sThr) pine &= ~(1<<INP_E_ThrCt); else pine |= (1<<INP_E_ThrCt);
#endif
if (inputs.sGea) pine &= ~(1<<INP_E_Gear); else pine |= (1<<INP_E_Gear);
if (inputs.sTrn) pine &= ~(1<<INP_E_Trainer); else pine |= (1<<INP_E_Trainer);
#endif

switch (inputs.sId0) {
  case 2:
#if defined(PCBSKY9X)
    PIOC->PIO_PDSR &= ~0x00000800;
    PIOC->PIO_PDSR |= 0x00004000;
#elif defined(PCBGRUVIN9X)
    ping &= ~(1<<INP_G_ID1);
    pinb |= (1<<INP_B_ID2);
#else
    ping &= ~(1<<INP_G_ID1);
    pine |= (1<<INP_E_ID2);
#endif
    break;
  case 1:
#if defined(PCBSKY9X)
    PIOC->PIO_PDSR |= 0x00004800;
#elif defined(PCBGRUVIN9X)
    ping &= ~(1<<INP_G_ID1);
    pinb &= ~(1<<INP_B_ID2);
#else
    ping &= ~(1<<INP_G_ID1);
    pine &= ~(1<<INP_E_ID2);
#endif
    break;
  case 0:
#if defined(PCBSKY9X)
    PIOC->PIO_PDSR &= ~0x00004000;
    PIOC->PIO_PDSR |= 0x00000800;
#elif defined(PCBGRUVIN9X)
    ping |=  (1<<INP_G_ID1);
    pinb &= ~(1<<INP_B_ID2);
#else
    ping |=  (1<<INP_G_ID1);
    pine &= ~(1<<INP_E_ID2);
#endif
    break;
}

// keyboard
#if defined(PCBX9D)
simuSetKey(KEY_MENU, inputs.menu);
simuSetKey(KEY_EXIT, inputs.exit);
simuSetKey(KEY_PLUS, inputs.right);
simuSetKey(KEY_MINUS, inputs.left);
simuSetKey(KEY_PAGE, inputs.up);
simuSetKey(KEY_ENTER, inputs.down);
#else
simuSetKey(KEY_MENU, inputs.menu);
simuSetKey(KEY_EXIT, inputs.exit);
simuSetKey(KEY_UP, inputs.up);
simuSetKey(KEY_DOWN, inputs.down);
simuSetKey(KEY_RIGHT, inputs.right);
simuSetKey(KEY_LEFT, inputs.left);
#endif

#ifdef PCBGRUVIN9X
// rotary encoders
pind = 0;
if (inputs.re1) pind |= 0x20;
#endif

#ifdef PCBSKY9X
PIOB->PIO_PDSR |= 0x40;
if (inputs.re1) PIOB->PIO_PDSR &= ~0x40;
#endif
#endif

#ifdef GETVALUES_IMPORT
#undef GETVALUES_IMPORT
memset(outputs.chans, 0, sizeof(outputs.chans));
memcpy(outputs.chans, g_chans512, sizeof(g_chans512));
for (int i=0; i<12; i++)
  outputs.vsw[i] = getSwitch(DSW_SW1+i, 0);
#endif

#ifdef LCDCHANGED_IMPORT
#undef LCDCHANGED_IMPORT
if (lcd_refresh) {
#if defined(PCBSKY9X)
  lightEnable = (PWM->PWM_CH_NUM[0].PWM_CDTY != 100);
#elif defined(PCBGRUVIN9X)
  lightEnable = (portc & (1<<OUT_C_LIGHT));
#else
  lightEnable = (portb & (1<<OUT_B_LIGHT));
#endif
  lcd_refresh = false;
  return true;
}
return false;
#endif

#ifdef TIMER10MS_IMPORT
#undef TIMER10MS_IMPORT
if (!main_thread_running)
  return false;
per10ms();
return true;
#endif

#ifdef GETLCD_IMPORT
#undef GETLCD_IMPORT
return lcd_buf;
#endif

#ifdef GETERROR_IMPORT
#undef GETERROR_IMPORT
return main_thread_error;
#endif


