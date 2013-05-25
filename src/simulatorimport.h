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
#ifndef PCBTARANIS
for (int i=0; i<3; i++)
  g_anas[4+i] = inputs.pots[i];
#else
for (int i=0; i<4; i++)
  g_anas[4+i] = inputs.pots[i];

#endif
// switches
for (int i=0; i<10/*TODO NUM_SWITCHES*/; i++)
  simuSetSwitch(i, inputs.switches[i]);

// keys
for (int i=0; i<6/*NUM_KEYS*/; i++)
  simuSetKey(i, inputs.keys[i]);

// trims
for (int i=0; i<NUM_STICKS*2; i++)
  simuSetTrim(i, 0);

#ifdef PCBGRUVIN9X
// rotary encoders
pind = 0;
if (inputs.rotenc) pind |= 0x20;
#endif

#ifdef PCBSKY9X
if (inputs.rotenc) PIOB->PIO_PDSR &= ~0x40; else PIOB->PIO_PDSR |= 0x40;
#endif
#endif

#ifdef GETVALUES_IMPORT
#undef GETVALUES_IMPORT
memset(outputs.chans, 0, sizeof(outputs.chans));
memcpy(outputs.chans, g_chans512, sizeof(g_chans512));
for (int i=0; i<NUM_CSW; i++)
  outputs.vsw[i] = getSwitch(SWSRC_SW1+i, 0);
#endif

#ifdef LCDCHANGED_IMPORT
#undef LCDCHANGED_IMPORT
if (lcd_refresh) {
  lightEnable = IS_BACKLIGHT_ON();
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


