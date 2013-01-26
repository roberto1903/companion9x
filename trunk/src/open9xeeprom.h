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

#ifndef open9xeeprom_h
#define open9xeeprom_h

#include <inttypes.h>
#include "eeprominterface.h"
#include "eepromimportexport.h"
#include <qbytearray.h>

#define GVARS_VARIANT 0x0001
#define FRSKY_VARIANT 0x0002
#define M128_VARIANT  0x8000

#define SIMU_STOCK_VARIANTS      (GVARS_VARIANT|FRSKY_VARIANT)
#define SIMU_M128_VARIANTS       (M128_VARIANT)
#define SIMU_GRUVIN9X_VARIANTS   (0)
#define SIMU_ARM_VARIANTS        (0)

#define O9X_MAX_TIMERS     2
#define O9X_MAX_PHASES 5
#define O9X_MAX_MIXERS 32
#define O9X_MAX_EXPOS  14
#define O9X_NUM_CHNOUT 16 // number of real output channels CH1-CH16
#define O9X_NUM_CSW    12 // number of custom switches
#define O9X_NUM_FSW    16 // number of functions assigned to switches
#define O9X_MAX_CURVES 8
#define O9X_NUM_POINTS (112-O9X_MAX_CURVES)
#define O9X_MAX_GVARS  5

class Open9xModelDataNew: public StructField {
  public:
    Open9xModelDataNew(ModelData & modelData, BoardEnum board, unsigned int variant);

  protected:
    BoardEnum board;
    unsigned int variant;
};

#endif
