#include <stdio.h>
#include <list>
#include "eeprominterface.h"
#include "er9xinterface.h"
#include "th9xinterface.h"
#include "gruvin9xinterface.h"
#include "gruvin9xstableinterface.h"
#include "open9xinterface.h"
#include "qsettings.h"

QString EEPROMWarnings;

void setEEPROMString(char *dst, const char *src, int size)
{
  memcpy(dst, src, size);
  for (int i=size-1; i>=0; i--) {
    if (dst[i] == '\0')
      dst[i] = ' ';
    else
      break;
  }
}

void getEEPROMString(char *dst, const char *src, int size)
{
  memcpy(dst, src, size);
  dst[size] = '\0';
  for (int i=size-1; i>=0; i--) {
    if (dst[i] == ' ')
      dst[i] = '\0';
    else
      break;
  }
}

GeneralSettings::GeneralSettings()
{
  memset(this, 0, sizeof(GeneralSettings));
  contrast = 25;
  vBatWarn = 90;
  stickMode=  1;
  for (int i=0; i<7; ++i) {
    calibMid[i]     = 0x200;
    calibSpanNeg[i] = 0x180;
    calibSpanPos[i] = 0x180;
  }
  QSettings settings("companion9x", "companion9x");
  templateSetup = settings.value("default_channel_order", 0).toInt();
  stickMode = settings.value("default_mode", 1).toInt();
}

ModelData::ModelData()
{
  clear();
}

void ModelData::clear()
{
  memset(this, 0, sizeof(ModelData));
  ppmNCH = 8;
  ppmDelay = 300;
  for (int i=0; i<MAX_PHASES; i++)
    phaseData[i].clear();
  for (int i=0; i<MAX_MIXERS; i++)
    mixData[i].clear();
  for(int i=0; i<4; i++){
    mixData[i].destCh = i+1;
    mixData[i].srcRaw = i+1;
    mixData[i].weight = 100;
  }
  for (int i=0; i<NUM_CHNOUT; i++)
    limitData[i].clear();
  for (int i=0; i<NUM_STICKS; i++)
    expoData[i].clear();
  for (int i=0; i<NUM_CSW; i++)
    customSw[i].clear();
  swashRingData.clear();
  frsky.clear();
}

bool ModelData::isempty()
{
  return !used;
}

void ModelData::setDefault(uint8_t id)
{
  clear();  
  used = true;
  sprintf(name, "MODEL%02d", id+1);
}

unsigned int ModelData::getTrimFlightPhase(uint8_t idx, int8_t phase)
{
  // if (phase == -1) phase = getFlightPhase();

  for (uint8_t i=0; i<MAX_PHASES; i++) {
    if (phase == 0 || phaseData[phase].trimRef[idx] < 0) return phase;
    phase = phaseData[phase].trimRef[idx];
  }
  return 0;
}

QList<EEPROMInterface *> eepromInterfaces;
void RegisterEepromInterfaces()
{
  eepromInterfaces.push_back(new Th9xInterface());
  eepromInterfaces.push_back(new Er9xInterface());
  eepromInterfaces.push_back(new Gruvin9xInterface(EESIZE_STOCK));
  eepromInterfaces.push_back(new Gruvin9xInterface(EESIZE_V4));
  eepromInterfaces.push_back(new Open9xInterface());
}

QList<FirmwareInfo> firmwares;
void RegisterFirmwares()
{
  firmwares.push_back(FirmwareInfo("th9x", QObject::tr("th9x"), new Th9xInterface(), "http://th9x.googlecode.com/svn/trunk/th9x.bin"));
  firmwares.push_back(FirmwareInfo("er9x", QObject::tr("er9x"), new Er9xInterface(), "http://er9x.googlecode.com/svn/trunk/er9x.hex"));
  firmwares.push_back(FirmwareInfo("er9x-noht", QObject::tr("er9x - No heli, no templates"), new Er9xInterface(), "http://er9x.googlecode.com/svn/trunk/er9x-noht.hex"));
  firmwares.push_back(FirmwareInfo("er9x-jeti", QObject::tr("er9x - JETI"), new Er9xInterface(), "http://er9x.googlecode.com/svn/trunk/er9x-jeti.hex"));
  firmwares.push_back(FirmwareInfo("er9x-ardupilot", QObject::tr("er9x - ArDuPilot"), new Er9xInterface(), "http://er9x.googlecode.com/svn/trunk/er9x-ardupilot.hex"));
  firmwares.push_back(FirmwareInfo("er9x-frsky", QObject::tr("er9x - FrSky"), new Er9xInterface(), "http://er9x.googlecode.com/svn/trunk/er9x-frsky.hex"));
  firmwares.push_back(FirmwareInfo("er9x-frsky-noht", QObject::tr("er9x - FrSky, no heli, no templates"), new Er9xInterface(), "http://er9x.googlecode.com/svn/trunk/er9x-frsky-noht.hex"));
  firmwares.push_back(FirmwareInfo("er9x-nmea", QObject::tr("er9x - NMEA"), new Er9xInterface(), "http://er9x.googlecode.com/svn/trunk/er9x-nmea.hex"));
  firmwares.push_back(FirmwareInfo("gruvin9x-stable-stock", QObject::tr("gruvin9x stable for stock board"), new Gruvin9xInterface(EESIZE_STOCK), "http://gruvin9x.googlecode.com/svn/branches/frsky/gruvin9x-stock.hex"));
  firmwares.push_back(FirmwareInfo("gruvin9x-stable-stock-speaker", QObject::tr("gruvin9x stable for stock board - Speaker mod"), new Gruvin9xInterface(EESIZE_STOCK), "http://gruvin9x.googlecode.com/svn/branches/frsky/gruvin9x-std-speaker.hex"));
  firmwares.push_back(FirmwareInfo("gruvin9x-stable-stock-frsky", QObject::tr("gruvin9x stable for stock board - FrSky"), new Gruvin9xInterface(EESIZE_STOCK), "http://gruvin9x.googlecode.com/svn/branches/frsky/gruvin9x-frsky-nospeaker.hex"));
  firmwares.push_back(FirmwareInfo("gruvin9x-stable-stock-frsky-speaker", QObject::tr("gruvin9x stable for stock board - Frsky, speaker"), new Gruvin9xInterface(EESIZE_STOCK), "http://gruvin9x.googlecode.com/svn/branches/frsky/gruvin9x-frsky-speaker.hex"));
  firmwares.push_back(FirmwareInfo("gruvin9x-trunk-stock", QObject::tr("gruvin9x trunk for stock board"), new Gruvin9xInterface(EESIZE_STOCK)));
  firmwares.push_back(FirmwareInfo("gruvin9x-trunk-stock-speaker", QObject::tr("gruvin9x trunk for stock board - Speaker mod"), new Gruvin9xInterface(EESIZE_STOCK)));
  firmwares.push_back(FirmwareInfo("gruvin9x-trunk-stock-frsky", QObject::tr("gruvin9x trunk for stock board - FrSky"), new Gruvin9xInterface(EESIZE_STOCK)));
  firmwares.push_back(FirmwareInfo("gruvin9x-trunk-stock-frsky-speaker", QObject::tr("gruvin9x trunk for stock board - Frsky, speaker"), new Gruvin9xInterface(EESIZE_STOCK)));
  firmwares.push_back(FirmwareInfo("gruvin9x-trunk-v4", QObject::tr("gruvin9x trunk for v4 board"), new Gruvin9xInterface(EESIZE_V4), "http://gruvin9x.googlecode.com/svn/branches/frsky/gruvin9x.hex"));
  firmwares.push_back(FirmwareInfo("open9x", QObject::tr("open9x - trunk"), new Open9xInterface()));

  RegisterEepromInterfaces();
}

bool LoadEeprom(RadioData &radioData, uint8_t *eeprom, int size)
{
  foreach(EEPROMInterface *eepromInterface, eepromInterfaces) {
    if (eepromInterface->load(radioData, eeprom, size))
      return true;
  }

  return false;
}

EEPROMInterface *GetEepromInterface()
{
  static EEPROMInterface * defaultEepromInterface = new Gruvin9xInterface(EESIZE_STOCK);

  QSettings settings("companion9x", "companion9x");
  QVariant current_firmware = settings.value("firmware", "gruvin9x");

  foreach(FirmwareInfo firmware, firmwares) {
    if (firmware.id == current_firmware)
      return firmware.eepromInterface;
  }

  return defaultEepromInterface;
}

