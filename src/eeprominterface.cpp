#include <stdio.h>
#include <list>
#include "eeprominterface.h"
#include "er9xinterface.h"
#include "th9xinterface.h"
#include "gruvin9xinterface.h"
#include "gruvin9xstableinterface.h"
#include "open9xinterface.h"
#include "ersky9xinterface.h"
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

int RawSource::toValue()
{
  return index >= 0 ? (type * 65536 + index) : -(type * 65536 - index);
}

int RawSource::getDecimals(ModelData Model)
{
  if(type==SOURCE_TYPE_TELEMETRY) {
    switch (index) {
      case 0:
      case 1:
        return (Model.frsky.channels[index].type==0 ? 2: 0);
      case 12:
        return 2;
      default:
        return 0;
    }
  }
  return 0;
}

double RawSource::getMin(ModelData Model)
{
  switch (type) {
    case SOURCE_TYPE_TELEMETRY:
      switch (index) {
        case 0:
        case 1:
          if (Model.frsky.channels[index].type==0) {
            return (Model.frsky.channels[index].offset*Model.frsky.channels[index].ratio)/2550.0;
          } else {
            return (Model.frsky.channels[index].offset*Model.frsky.channels[index].ratio)/255.0;
          }
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:    
          return 0;
        case 7:
        case 8:
          return -30;
        case 9:
        case 10:
        case 11:
        case 12:
          return 0;
        default:
          return -125;
      }
      break;
    case SOURCE_TYPE_STICK:
    case SOURCE_TYPE_ROTARY_ENCODER:
    case SOURCE_TYPE_MAX:
    case SOURCE_TYPE_3POS:
    case SOURCE_TYPE_SWITCH:
      return -100;
      break;
    case SOURCE_TYPE_CH:
      return (Model.extendedLimits ? -125 :-100);
      break;
    default:
      return -125;
  }
}

double RawSource::getMax(ModelData Model)
{
  switch (type) {
    case SOURCE_TYPE_TELEMETRY:
      switch (index) {
        case 0:
        case 1:
          if (Model.frsky.channels[index].type==0) {
            return (Model.frsky.channels[index].ratio-(Model.frsky.channels[index].offset*Model.frsky.channels[index].ratio)/255.0)/10;
          } else {
            return Model.frsky.channels[index].ratio-(Model.frsky.channels[index].offset*Model.frsky.channels[index].ratio)/255.0;
          }
        case 2:
        case 3:
          return 100;
        case 4:
          return 1020;
        case 5:
          return 12750;
        case 6:
          return 100;        
        case 7:
        case 8:
          return 225;
        case 9:
          return 944;
        case 10:
          return 2040;
        case 11:
          return 1020;
        case 12:
          return 5.1;
        default:
          return 125;
      }
      break;
    case SOURCE_TYPE_STICK:
    case SOURCE_TYPE_ROTARY_ENCODER:
    case SOURCE_TYPE_MAX:
    case SOURCE_TYPE_3POS:
    case SOURCE_TYPE_SWITCH:
      return 100;
      break;      
    case SOURCE_TYPE_CH:
      return (Model.extendedLimits ? 125 :100);
      break;
    default:
      return 125;
  }
}

double RawSource::getOffset(ModelData Model)
{
  if(type==SOURCE_TYPE_TELEMETRY) {
    switch (index) {
      case 0:
      case 1:
        if (Model.frsky.channels[index].type==0) {
          return (Model.frsky.channels[index].offset*Model.frsky.channels[index].ratio)/2550.0;
        } else {
          return (Model.frsky.channels[index].offset*Model.frsky.channels[index].ratio)/255.0;
        }
      case 2:
      case 3:
        return 0;
      case 4:
        return 512;
      case 5:
        return 6400;
      case 6:
        return 0;
      case 7:
      case 8:
        return 98;
      case 9:
        return 474;
      case 10:
        return 1024;
      case 11:
        return 512;
      case 12:
        return 2.56;
      default:
        return 0;
    }
  }
  return 0;
}

int RawSource::getRawOffset(ModelData Model)
{
  if(type==SOURCE_TYPE_TELEMETRY) {
    switch (index) {
      case 0:
      case 1:
      case 2:
      case 3:
      case 6:
        return 128;        
      default:
        return 0;
    }
  }
  return 0;
}

double RawSource::getStep(ModelData Model)
{
  if(type==SOURCE_TYPE_TELEMETRY) {
    switch (index) {
      case 0:
      case 1:
        if (Model.frsky.channels[index].type==0) {
          return (Model.frsky.channels[index].ratio/2550.0);
        } else {
          return (Model.frsky.channels[index].ratio/255.0);
        }
      case 3:
        return 1;
      case 4:
        return 4;
      case 5:
        return 50;
      case 6:
        return 1;
      case 7:
      case 8:
        return 1;
      case 9:
        return 4;
      case 10:
        return 8;
      case 11:
        return 4;
      case 12:
        return 0.02;
      default:
        return 1;
    }
  }
  return 1;
}

QString RawSource::toString()
{
  QString sticks[] = { QObject::tr("Rud"), QObject::tr("Ele"), QObject::tr("Thr"), QObject::tr("Ail"),
                       QObject::tr("P1"), QObject::tr("P2"), QObject::tr("P3")
                     };

  QString rotary[] = { QObject::tr("REa"), QObject::tr("REb") };

  QString telemetry[] = { QObject::tr("A1"), QObject::tr("A2"), QObject::tr("TX"), QObject::tr("RX"), QObject::tr("Alt"), QObject::tr("Rpm"), QObject::tr("Fuel"), QObject::tr("T1"), QObject::tr("T2"), QObject::tr("Speed"), QObject::tr("Dist"), QObject::tr("GPS Alt"), QObject::tr("Cell") };

  switch(type) {
    case SOURCE_TYPE_STICK:
      return sticks[index];
    case SOURCE_TYPE_ROTARY_ENCODER:
      return rotary[index];
    case SOURCE_TYPE_MAX:
      return QObject::tr("MAX");
    case SOURCE_TYPE_3POS:
      return QObject::tr("3POS");
    case SOURCE_TYPE_SWITCH:
      return RawSwitch(index).toString();
    case SOURCE_TYPE_CYC:
      return QObject::tr("CYC%1").arg(index+1);
    case SOURCE_TYPE_PPM:
      return QObject::tr("PPM%1").arg(index+1);
    case SOURCE_TYPE_CH:
      return QObject::tr("CH%1").arg(index+1);
    case SOURCE_TYPE_TIMER:
      return QObject::tr("Timer%1").arg(index+1);
    case SOURCE_TYPE_TELEMETRY:
      return telemetry[index];
    default:
      return QObject::tr("----");
  }
}

int RawSwitch::toValue()
{
  return index >= 0 ? (type * 256 + index) : -(type * 256 - index);
}

QString RawSwitch::toString()
{
  QString switches[] = { QObject::tr("THR"), QObject::tr("RUD"), QObject::tr("ELE"),
                         QObject::tr("ID0"), QObject::tr("ID1"), QObject::tr("ID2"),
                         QObject::tr("AIL"), QObject::tr("GEA"), QObject::tr("TRN")
                       };

  switch(type) {
    case SWITCH_TYPE_SWITCH:
      // TODO assert(index != 0);
      return index > 0 ? switches[index-1] : QString("!") + switches[-index-1];
    case SWITCH_TYPE_VIRTUAL:
    {
      QString neg = QString("");
      if (index < 0) { neg = QString("!"); index = -index; }
      if (index < 10)
        return neg+QObject::tr("SW%1").arg(index);
      else
        return neg+QObject::tr("SW%1").arg(QChar('A'+index-10));
    }
    case SWITCH_TYPE_ON:
      return QObject::tr("ON");
    case SWITCH_TYPE_OFF:
      return QObject::tr("OFF");
    default:
      return QObject::tr("----");
  }
}

GeneralSettings::GeneralSettings()
{
  memset(this, 0, sizeof(GeneralSettings));
  contrast  = 25;
  vBatWarn  = 90;
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
    mixData[i].srcRaw = RawSource(SOURCE_TYPE_STICK, i);
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
  eepromInterfaces.push_back(new Open9xInterface(BOARD_STOCK));
  eepromInterfaces.push_back(new Open9xInterface(BOARD_GRUVIN9X));
  eepromInterfaces.push_back(new Open9xInterface(BOARD_ERSKY9X));
  eepromInterfaces.push_back(new Gruvin9xInterface(BOARD_STOCK));
  eepromInterfaces.push_back(new Gruvin9xInterface(BOARD_GRUVIN9X));
  eepromInterfaces.push_back(new Ersky9xInterface());
  eepromInterfaces.push_back(new Th9xInterface());
  eepromInterfaces.push_back(new Er9xInterface());
}

QList<FirmwareInfo *> firmwares;
FirmwareInfo * default_firmware = NULL;

const char * ER9X_STAMP = "http://er9x.googlecode.com/svn/trunk/src/stamp-er9x.h";
const char * ERSKY9X_STAMP = "http://ersky9x.googlecode.com/svn/trunk/src/stamp-ersky9x.h";

extern const char *open9x_stock_binaries[];
extern const char *open9x_v4_binaries[];
extern const char *open9x_arm_binaries[];

void RegisterFirmwares()
{
  firmwares.push_back(new FirmwareInfo("th9x", QObject::tr("th9x"), new Th9xInterface(), "http://th9x.googlecode.com/svn/trunk/th9x.bin","http://th9x.googlecode.com/svn/trunk/src/stamp-th9x.h"));

  firmwares.push_back(new FirmwareInfo("er9x", QObject::tr("er9x"), new Er9xInterface(), "http://er9x.googlecode.com/svn/trunk/er9x.hex", ER9X_STAMP));
  FirmwareInfo * er9x = firmwares.last();
  er9x->add_option(new FirmwareInfo("er9x-noht", new Er9xInterface(), "http://er9x.googlecode.com/svn/trunk/er9x-noht.hex", ER9X_STAMP));
  er9x->add_option(new FirmwareInfo("er9x-jeti", new Er9xInterface(), "http://er9x.googlecode.com/svn/trunk/er9x-jeti.hex", ER9X_STAMP));
  er9x->add_option(new FirmwareInfo("er9x-ardupilot", new Er9xInterface(), "http://er9x.googlecode.com/svn/trunk/er9x-ardupilot.hex", ER9X_STAMP));
  er9x->add_option(new FirmwareInfo("er9x-frsky", new Er9xInterface(), "http://er9x.googlecode.com/svn/trunk/er9x-frsky.hex", ER9X_STAMP));
  er9x->add_option(new FirmwareInfo("er9x-frsky-noht", new Er9xInterface(), "http://er9x.googlecode.com/svn/trunk/er9x-frsky-noht.hex", ER9X_STAMP));
  er9x->add_option(new FirmwareInfo("er9x-nmea", new Er9xInterface(), "http://er9x.googlecode.com/svn/trunk/er9x-nmea.hex", ER9X_STAMP));

  firmwares.push_back(new FirmwareInfo("gruvin9x-stable-stock", QObject::tr("gruvin9x stable for stock board"), new Gruvin9xInterface(BOARD_STOCK), "http://gruvin9x.googlecode.com/svn/branches/frsky/gruvin9x-stock.hex"));
  FirmwareInfo * gruvin9x = firmwares.last();
  gruvin9x->add_option(new FirmwareInfo("gruvin9x-stable-stock-speaker", new Gruvin9xInterface(BOARD_STOCK), "http://gruvin9x.googlecode.com/svn/branches/frsky/gruvin9x-std-speaker.hex",ER9X_STAMP));
  gruvin9x->add_option(new FirmwareInfo("gruvin9x-stable-stock-frsky", new Gruvin9xInterface(BOARD_STOCK), "http://gruvin9x.googlecode.com/svn/branches/frsky/gruvin9x-frsky-nospeaker.hex",ER9X_STAMP));
  gruvin9x->add_option(new FirmwareInfo("gruvin9x-stable-stock-frsky-speaker", new Gruvin9xInterface(BOARD_STOCK), "http://gruvin9x.googlecode.com/svn/branches/frsky/gruvin9x-frsky-speaker.hex",ER9X_STAMP));

  firmwares.push_back(new FirmwareInfo("gruvin9x-trunk-stock", QObject::tr("gruvin9x trunk for stock board"), new Gruvin9xInterface(BOARD_STOCK)));
  firmwares.push_back(new FirmwareInfo("gruvin9x-stable-v4", QObject::tr("gruvin9x stable for v4 board"), new Gruvin9xInterface(BOARD_GRUVIN9X), "http://gruvin9x.googlecode.com/svn/branches/frsky/gruvin9x.hex"));
  firmwares.push_back(new FirmwareInfo("gruvin9x-trunk-v4", QObject::tr("gruvin9x trunk for v4 board"), new Gruvin9xInterface(BOARD_GRUVIN9X)));

  Open9xFirmware * open9x = new Open9xFirmware("open9x-stock", QObject::tr("open9x for stock board"), new Open9xInterface(BOARD_STOCK));
  open9x->addOptions(open9x_stock_binaries);
  firmwares.push_back(open9x);

  open9x = new Open9xFirmware("open9x-v4", QObject::tr("open9x for gruvin9x board"), new Open9xInterface(BOARD_GRUVIN9X));
  open9x->addOptions(open9x_v4_binaries);
  firmwares.push_back(open9x);

  open9x = new Open9xFirmware("open9x-arm", QObject::tr("open9x for ersky9x board"), new Open9xInterface(BOARD_ERSKY9X));
  open9x->addOptions(open9x_arm_binaries);
  firmwares.push_back(open9x);

  firmwares.push_back(new FirmwareInfo("ersky9x", QObject::tr("ersky9x"), new Ersky9xInterface(), "http://ersky9x.googlecode.com/svn/trunk/ersky9x_rom.bin", ERSKY9X_STAMP));

  default_firmware = GetFirmware("open9x-stock-heli-templates-en");

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

bool LoadEepromXml(RadioData &radioData, QDomDocument &doc)
{
  foreach(EEPROMInterface *eepromInterface, eepromInterfaces) {
    if (eepromInterface->loadxml(radioData, doc))
      return true;
  }

  return false;
}

FirmwareInfo * GetFirmware(QString id)
{
  foreach(FirmwareInfo * firmware, firmwares) {
    if (firmware->id == id)
      return firmware;
    foreach(FirmwareInfo * option, firmware->options) {
      if (option->id == id)
        return option;
    }
  }
  return NULL;
}

FirmwareInfo * GetCurrentFirmware()
{
  QSettings settings("companion9x", "companion9x");
  QVariant firmware_id = settings.value("firmware", default_firmware->id);
  FirmwareInfo *firmware = GetFirmware(firmware_id.toString());
  return firmware ? firmware : default_firmware;
}

EEPROMInterface *GetEepromInterface()
{
  return GetCurrentFirmware()->eepromInterface;
}



