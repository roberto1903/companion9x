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
extern uint8_t portb;

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

#define INP_B_Trainer 5
#define INP_B_KEY_LFT 6
#define INP_B_KEY_RGT 5
#define INP_B_KEY_UP  4
#define INP_B_KEY_DWN 3
#define INP_B_KEY_EXT 2
#define INP_B_KEY_MEN 1
#define INP_B_ID2     4

#define OUT_C_LIGHT   0
#define OUT_B_LIGHT   7

extern uint8_t eeprom[2048]; // TODO size 4096
extern int16_t g_anas[NUM_STICKS+NUM_POTS];
extern int16_t g_chans512[NUM_CHNOUT];

#ifdef PCBV4
extern volatile uint8_t g_rotenc[2];
#endif

extern uint8_t lcd_buf[128*64/8];
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
#endif

#ifdef SETVALUES_IMPORT
#undef SETVALUES_IMPORT
g_anas[0] = inputs.rud;
g_anas[1] = inputs.ele;
g_anas[2] = inputs.thr;
g_anas[3] = inputs.ail;
g_anas[4] = inputs.pot1;
g_anas[5] = inputs.pot2;
g_anas[6] = inputs.pot3;

#ifdef PCBV4
pinj = 0;
pinl = 0;
#endif

if (inputs.sRud) ping &= ~(1<<INP_G_RuddDR); else ping |= (1<<INP_G_RuddDR);

#ifdef PCBV4
if (inputs.sEle) pinc &= ~(1<<INP_C_ElevDR); else pinc |= (1<<INP_C_ElevDR);
if (inputs.sThr) ping &= ~(1<<INP_G_ThrCt); else ping |= (1<<INP_G_ThrCt);
if (inputs.sAil) pinc &= ~(1<<INP_C_AileDR); else pinc |= (1<<INP_C_AileDR);
if (inputs.sGea) ping &= ~(1<<INP_G_Gear); else ping |= (1<<INP_G_Gear);
if (inputs.sTrn) pinb &= ~(1<<INP_B_Trainer); else pinb |= (1<<INP_B_Trainer);
#else
if (inputs.sEle) pine &= ~(1<<INP_E_ElevDR); else pine |= (1<<INP_E_ElevDR);
if (inputs.sThr) pine &= ~(1<<INP_E_ThrCt); else pine |= (1<<INP_E_ThrCt);
if (inputs.sAil) pine &= ~(1<<INP_E_AileDR); else pine |= (1<<INP_E_AileDR);
if (inputs.sGea) pine &= ~(1<<INP_E_Gear); else pine |= (1<<INP_E_Gear);
if (inputs.sTrn) pine &= ~(1<<INP_E_Trainer); else pine |= (1<<INP_E_Trainer);
#endif

switch (inputs.sId0) {
  case 2:
    ping &= ~(1<<INP_G_ID1);
#ifdef PCBV4
    pinb |= (1<<INP_B_ID2);
#else
    pine |= (1<<INP_E_ID2);
#endif
    break;
  case 1:
    ping &= ~(1<<INP_G_ID1);
#ifdef PCBV4
    pinb &= ~(1<<INP_B_ID2);
#else
    pine &= ~(1<<INP_E_ID2);
#endif
    break;
  case 0:
    ping |=  (1<<INP_G_ID1);
#ifdef PCBV4
    ping |=  (1<<INP_G_ID1);
#else
    pine &= ~(1<<INP_E_ID2);
#endif
    break;
}

// keyboad
pinb &= ~ 0x7e;
pinl &= ~ 0x3f; // for v4

if (inputs.menu) { pinb |= (1<<INP_B_KEY_MEN); pinl |= (1<<INP_P_KEY_MEN); }
if (inputs.exit) { pinb |= (1<<INP_B_KEY_EXT); pinl |= (1<<INP_P_KEY_EXT); }
if (inputs.up) { pinb |= (1<<INP_B_KEY_UP); pinl |= (1<<INP_P_KEY_UP); }
if (inputs.down) { pinb |= (1<<INP_B_KEY_DWN); pinl |= (1<<INP_P_KEY_DWN); }
if (inputs.left) { pinb |= (1<<INP_B_KEY_LFT); pinl |= (1<<INP_P_KEY_LFT); }
if (inputs.right) { pinb |= (1<<INP_B_KEY_RGT); pinl |= (1<<INP_P_KEY_RGT); }

#ifdef PCBV4
// rotary encoders
pind = 0;
if (inputs.re1) pind |= 0x20;
#endif
#endif

#ifdef GETVALUES_IMPORT
#undef GETVALUES_IMPORT
memcpy(outputs.chans, g_chans512, sizeof(outputs.chans));
for (int i=0; i<12; i++)
  outputs.vsw[i] = getSwitch(DSW_SW1+i, 0);
#endif

#ifdef LCDCHANGED_IMPORT
#undef LCDCHANGED_IMPORT
if (lcd_refresh) {
  lightEnable = (portb & (1<<OUT_B_LIGHT));
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

#ifdef SETTRIM_IMPORT
#undef SETTRIM_IMPORT
uint8_t phase = getTrimFlightPhase(idx, getFlightPhase());
setTrimValue(phase, idx, value);
#endif

#ifdef GETTRIMS_IMPORT
#undef GETTRIMS_IMPORT
uint8_t phase = getFlightPhase();
for (uint8_t idx=0; idx<4; idx++) {
  trims.extended = hasExtendedTrims();
  trims.values[idx] = getTrimValue(getTrimFlightPhase(idx, phase), idx);
}
#endif

#ifdef GETERROR_IMPORT
#undef GETERROR_IMPORT
return main_thread_error;
#endif


