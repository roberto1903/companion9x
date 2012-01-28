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
  contrast  = 25;
  vBatWarn  = 90;
  stickMode =  1;
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
  for (int i=0; i<MAX_EXPOS; i++)
    expoData[i].clear();
  for (int i=0; i<MAX_MIXERS; i++)
    mixData[i].clear();
  for(int i=0; i<4; i++){
    mixData[i].destCh = i+1;
    mixData[i].srcRaw = (RawSource)(i+1);
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
  eepromInterfaces.push_back(new Open9xInterface(EESIZE_STOCK));
  eepromInterfaces.push_back(new Open9xInterface(EESIZE_V4));
}

QList<FirmwareInfo *> firmwares;
FirmwareInfo * default_firmware = NULL;

#define OPEN9X_BIN_URL "http://open9x.googlecode.com/svn/trunk/binaries/"

void RegisterFirmwares()
{
  const char * ER9X_STAMP = "http://er9x.googlecode.com/svn/trunk/src/stamp-er9x.h";
  const char * OPEN9X_STAMP = "http://open9x.googlecode.com/svn/trunk/stamp-open9x.txt";

  firmwares.push_back(new FirmwareInfo("th9x", QObject::tr("th9x"), new Th9xInterface(), "http://th9x.googlecode.com/svn/trunk/th9x.bin","http://th9x.googlecode.com/svn/trunk/src/stamp-th9x.h"));

  firmwares.push_back(new FirmwareInfo("er9x", QObject::tr("er9x"), new Er9xInterface(), "http://er9x.googlecode.com/svn/trunk/er9x.hex", ER9X_STAMP));
  FirmwareInfo * er9x = firmwares.last();
  er9x->add_option(new FirmwareInfo("er9x-noht", new Er9xInterface(), "http://er9x.googlecode.com/svn/trunk/er9x-noht.hex", ER9X_STAMP));
  er9x->add_option(new FirmwareInfo("er9x-jeti", new Er9xInterface(), "http://er9x.googlecode.com/svn/trunk/er9x-jeti.hex", ER9X_STAMP));
  er9x->add_option(new FirmwareInfo("er9x-ardupilot", new Er9xInterface(), "http://er9x.googlecode.com/svn/trunk/er9x-ardupilot.hex", ER9X_STAMP));
  er9x->add_option(new FirmwareInfo("er9x-frsky", new Er9xInterface(), "http://er9x.googlecode.com/svn/trunk/er9x-frsky.hex", ER9X_STAMP));
  er9x->add_option(new FirmwareInfo("er9x-frsky-noht", new Er9xInterface(), "http://er9x.googlecode.com/svn/trunk/er9x-frsky-noht.hex", ER9X_STAMP));
  er9x->add_option(new FirmwareInfo("er9x-nmea", new Er9xInterface(), "http://er9x.googlecode.com/svn/trunk/er9x-nmea.hex", ER9X_STAMP));

  firmwares.push_back(new FirmwareInfo("gruvin9x-stable-stock", QObject::tr("gruvin9x stable for stock board"), new Gruvin9xInterface(EESIZE_STOCK), "http://gruvin9x.googlecode.com/svn/branches/frsky/gruvin9x-stock.hex"));
  FirmwareInfo * gruvin9x = firmwares.last();
  gruvin9x->add_option(new FirmwareInfo("gruvin9x-stable-stock-speaker", new Gruvin9xInterface(EESIZE_STOCK), "http://gruvin9x.googlecode.com/svn/branches/frsky/gruvin9x-std-speaker.hex",ER9X_STAMP));
  gruvin9x->add_option(new FirmwareInfo("gruvin9x-stable-stock-frsky", new Gruvin9xInterface(EESIZE_STOCK), "http://gruvin9x.googlecode.com/svn/branches/frsky/gruvin9x-frsky-nospeaker.hex",ER9X_STAMP));
  gruvin9x->add_option(new FirmwareInfo("gruvin9x-stable-stock-frsky-speaker", new Gruvin9xInterface(EESIZE_STOCK), "http://gruvin9x.googlecode.com/svn/branches/frsky/gruvin9x-frsky-speaker.hex",ER9X_STAMP));

  firmwares.push_back(new FirmwareInfo("gruvin9x-trunk-stock", QObject::tr("gruvin9x trunk for stock board"), new Gruvin9xInterface(EESIZE_STOCK)));
  firmwares.push_back(new FirmwareInfo("gruvin9x-stable-v4", QObject::tr("gruvin9x stable for v4 board"), new Gruvin9xInterface(EESIZE_V4), "http://gruvin9x.googlecode.com/svn/branches/frsky/gruvin9x.hex"));
  firmwares.push_back(new FirmwareInfo("gruvin9x-trunk-v4", QObject::tr("gruvin9x trunk for v4 board"), new Gruvin9xInterface(EESIZE_V4)));

  firmwares.push_back(new FirmwareInfo("open9x-stock", QObject::tr("open9x for stock board"), new Open9xInterface(EESIZE_STOCK)));
  FirmwareInfo * open9x = firmwares.last();

  open9x->add_option(new FirmwareInfo("open9x-stock-en", new Open9xInterface(EESIZE_STOCK), OPEN9X_BIN_URL "open9x-stock-en.hex", OPEN9X_STAMP));
  open9x->add_option(new FirmwareInfo("open9x-stock-speaker-en", new Open9xInterface(EESIZE_STOCK), OPEN9X_BIN_URL "open9x-stock-en-speaker.hex", OPEN9X_STAMP));
  open9x->add_option(new FirmwareInfo("open9x-stock-templates-en", new Open9xInterface(EESIZE_STOCK), OPEN9X_BIN_URL "open9x-stock-templates-en.hex", OPEN9X_STAMP));
  open9x->add_option(new FirmwareInfo("open9x-stock-templates-speaker-en", new Open9xInterface(EESIZE_STOCK), OPEN9X_BIN_URL "open9x-stock-templates-speaker-en.hex", OPEN9X_STAMP));
  open9x->add_option(new FirmwareInfo("open9x-stock-heli-en", new Open9xInterface(EESIZE_STOCK), OPEN9X_BIN_URL "open9x-stock-heli-en.hex", OPEN9X_STAMP));
  open9x->add_option(new FirmwareInfo("open9x-stock-heli-speaker-en", new Open9xInterface(EESIZE_STOCK), OPEN9X_BIN_URL "open9x-stock-heli-speaker-en.hex", OPEN9X_STAMP));
  open9x->add_option(new FirmwareInfo("open9x-stock-heli-templates-en", new Open9xInterface(EESIZE_STOCK), OPEN9X_BIN_URL "open9x-stock-heli-templates-en.hex", OPEN9X_STAMP));
  default_firmware = new FirmwareInfo("open9x-stock-heli-templates-speaker-en", new Open9xInterface(EESIZE_STOCK), OPEN9X_BIN_URL "open9x-stock-heli-templates-speaker-en.hex", OPEN9X_STAMP);
  open9x->add_option(default_firmware);
  open9x->add_option(new FirmwareInfo("open9x-stock-frsky-en", new Open9xInterface(EESIZE_STOCK), OPEN9X_BIN_URL "open9x-frsky-stock-en.hex", OPEN9X_STAMP));
  open9x->add_option(new FirmwareInfo("open9x-stock-frsky-speaker-en", new Open9xInterface(EESIZE_STOCK), OPEN9X_BIN_URL "open9x-frsky-stock-speaker-en.hex", OPEN9X_STAMP));
  open9x->add_option(new FirmwareInfo("open9x-stock-frsky-templates-en", new Open9xInterface(EESIZE_STOCK), OPEN9X_BIN_URL "open9x-stock-frsky-templates-en.hex", OPEN9X_STAMP));
  open9x->add_option(new FirmwareInfo("open9x-stock-frsky-templates-speaker-en", new Open9xInterface(EESIZE_STOCK), OPEN9X_BIN_URL "open9x-stock-frsky-templates-speaker-en.hex", OPEN9X_STAMP));
  open9x->add_option(new FirmwareInfo("open9x-stock-frsky-heli-en", new Open9xInterface(EESIZE_STOCK), OPEN9X_BIN_URL "open9x-stock-frsky-heli-en.hex", OPEN9X_STAMP));
  open9x->add_option(new FirmwareInfo("open9x-stock-frsky-heli-speaker-en", new Open9xInterface(EESIZE_STOCK), OPEN9X_BIN_URL "open9x-stock-frsky-heli-speaker-en.hex", OPEN9X_STAMP));
  open9x->add_option(new FirmwareInfo("open9x-stock-frsky-heli-templates-en", new Open9xInterface(EESIZE_STOCK), OPEN9X_BIN_URL "open9x-stock-frsky-heli-templates-en.hex", OPEN9X_STAMP));
  open9x->add_option(new FirmwareInfo("open9x-stock-frsky-heli-templates-speaker-en", new Open9xInterface(EESIZE_STOCK), OPEN9X_BIN_URL "open9x-stock-frsky-heli-templates-speaker-en.hex", OPEN9X_STAMP));

  open9x->add_option(new FirmwareInfo("open9x-stock-fr", new Open9xInterface(EESIZE_STOCK), OPEN9X_BIN_URL "open9x-stock-fr.hex", OPEN9X_STAMP));
  open9x->add_option(new FirmwareInfo("open9x-stock-speaker-fr", new Open9xInterface(EESIZE_STOCK), OPEN9X_BIN_URL "open9x-stock-fr-speaker.hex", OPEN9X_STAMP));
  open9x->add_option(new FirmwareInfo("open9x-stock-templates-fr", new Open9xInterface(EESIZE_STOCK), OPEN9X_BIN_URL "open9x-stock-templates-fr.hex", OPEN9X_STAMP));
  open9x->add_option(new FirmwareInfo("open9x-stock-templates-speaker-fr", new Open9xInterface(EESIZE_STOCK), OPEN9X_BIN_URL "open9x-stock-templates-speaker-fr.hex", OPEN9X_STAMP));
  open9x->add_option(new FirmwareInfo("open9x-stock-heli-fr", new Open9xInterface(EESIZE_STOCK), OPEN9X_BIN_URL "open9x-stock-heli-fr.hex", OPEN9X_STAMP));
  open9x->add_option(new FirmwareInfo("open9x-stock-heli-speaker-fr", new Open9xInterface(EESIZE_STOCK), OPEN9X_BIN_URL "open9x-stock-heli-speaker-fr.hex", OPEN9X_STAMP));
  open9x->add_option(new FirmwareInfo("open9x-stock-heli-templates-fr", new Open9xInterface(EESIZE_STOCK), OPEN9X_BIN_URL "open9x-stock-heli-templates-fr.hex", OPEN9X_STAMP));
  open9x->add_option(new FirmwareInfo("open9x-stock-heli-templates-speaker-fr", new Open9xInterface(EESIZE_STOCK), OPEN9X_BIN_URL "open9x-stock-heli-templates-speaker-fr.hex", OPEN9X_STAMP));
  open9x->add_option(new FirmwareInfo("open9x-stock-frsky-fr", new Open9xInterface(EESIZE_STOCK), OPEN9X_BIN_URL "open9x-frsky-stock-fr.hex", OPEN9X_STAMP));
  open9x->add_option(new FirmwareInfo("open9x-stock-frsky-speaker-fr", new Open9xInterface(EESIZE_STOCK), OPEN9X_BIN_URL "open9x-frsky-stock-speaker-fr.hex", OPEN9X_STAMP));
  open9x->add_option(new FirmwareInfo("open9x-stock-frsky-templates-fr", new Open9xInterface(EESIZE_STOCK), OPEN9X_BIN_URL "open9x-stock-frsky-templates-fr.hex", OPEN9X_STAMP));
  open9x->add_option(new FirmwareInfo("open9x-stock-frsky-templates-speaker-fr", new Open9xInterface(EESIZE_STOCK), OPEN9X_BIN_URL "open9x-stock-frsky-templates-speaker-fr.hex", OPEN9X_STAMP));
  open9x->add_option(new FirmwareInfo("open9x-stock-frsky-heli-fr", new Open9xInterface(EESIZE_STOCK), OPEN9X_BIN_URL "open9x-stock-frsky-heli-fr.hex", OPEN9X_STAMP));
  open9x->add_option(new FirmwareInfo("open9x-stock-frsky-heli-speaker-fr", new Open9xInterface(EESIZE_STOCK), OPEN9X_BIN_URL "open9x-stock-frsky-heli-speaker-fr.hex", OPEN9X_STAMP));
  open9x->add_option(new FirmwareInfo("open9x-stock-frsky-heli-templates-fr", new Open9xInterface(EESIZE_STOCK), OPEN9X_BIN_URL "open9x-stock-frsky-heli-templates-fr.hex", OPEN9X_STAMP));
  open9x->add_option(new FirmwareInfo("open9x-stock-frsky-heli-templates-speaker-fr", new Open9xInterface(EESIZE_STOCK), OPEN9X_BIN_URL "open9x-stock-frsky-heli-templates-speaker-fr.hex", OPEN9X_STAMP));

  firmwares.push_back(new FirmwareInfo("open9x-v4", QObject::tr("open9x for v4 board"), new Open9xInterface(EESIZE_V4)));
  open9x = firmwares.last();
  open9x->add_option(new FirmwareInfo("open9x-v4-en", new Open9xInterface(EESIZE_V4), OPEN9X_BIN_URL "open9x-v4-en.hex", OPEN9X_STAMP));
  open9x->add_option(new FirmwareInfo("open9x-v4-templates-en", new Open9xInterface(EESIZE_V4), OPEN9X_BIN_URL "open9x-v4-templates-en.hex", OPEN9X_STAMP));
  open9x->add_option(new FirmwareInfo("open9x-v4-heli-en", new Open9xInterface(EESIZE_V4), OPEN9X_BIN_URL "open9x-v4-heli-en.hex", OPEN9X_STAMP));
  open9x->add_option(new FirmwareInfo("open9x-v4-heli-templates-en", new Open9xInterface(EESIZE_V4), OPEN9X_BIN_URL "open9x-v4-heli-templates-en.hex", OPEN9X_STAMP));

  open9x->add_option(new FirmwareInfo("open9x-v4-fr", new Open9xInterface(EESIZE_V4), OPEN9X_BIN_URL "open9x-v4-fr.hex", OPEN9X_STAMP));
  open9x->add_option(new FirmwareInfo("open9x-v4-templates-fr", new Open9xInterface(EESIZE_V4), OPEN9X_BIN_URL "open9x-v4-templates-fr.hex", OPEN9X_STAMP));
  open9x->add_option(new FirmwareInfo("open9x-v4-heli-fr", new Open9xInterface(EESIZE_V4), OPEN9X_BIN_URL "open9x-v4-heli-fr.hex", OPEN9X_STAMP));
  open9x->add_option(new FirmwareInfo("open9x-v4-heli-templates-fr", new Open9xInterface(EESIZE_V4), OPEN9X_BIN_URL "open9x-v4-heli-templates-fr.hex", OPEN9X_STAMP));

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

FirmwareInfo * GetCurrentFirmware()
{
  QSettings settings("companion9x", "companion9x");
  QVariant firmware_id = settings.value("firmware", default_firmware->id);

  foreach(FirmwareInfo * firmware, firmwares) {
    if (firmware->id == firmware_id)
      return firmware;
    foreach(FirmwareInfo * option, firmware->options) {
      if (option->id == firmware_id)
        return option;
    }
  }

  return default_firmware;
}

EEPROMInterface *GetEepromInterface()
{
  return GetCurrentFirmware()->eepromInterface;
}



