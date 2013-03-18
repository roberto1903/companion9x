#include <stdio.h>
#include <list>
#include "eeprominterface.h"
#include "er9xinterface.h"
#include "th9xinterface.h"
#include "gruvin9xinterface.h"
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

int RawSource::getDecimals(const ModelData & Model)
{
  if(type==SOURCE_TYPE_TELEMETRY) {
    switch (index) {
      case 2:
      case 3:
        return (Model.frsky.channels[index-2].type==0 ? 2: 0);
      case 12:
        return 2;
      case 13:
      case 15:
        return 1;
      default:
        return 0;
    }
  }
  return 0;
}

double RawSource::getMin(const ModelData & Model)
{
  switch (type) {
    case SOURCE_TYPE_TELEMETRY:
      switch (index) {
        /*case TELEMETRY_SOURCE_NONE:
        case TELEMETRY_SOURCE_TX_BATT:
        case TELEMETRY_SOURCE_TIMER1:
        case TELEMETRY_SOURCE_TIMER2:
          return 0; */
        case TELEMETRY_SOURCE_A1:
        case TELEMETRY_SOURCE_A2:
          if (Model.frsky.channels[index-2].type==0) {
            return (Model.frsky.channels[index-2].offset*Model.frsky.channels[index-2].ratio)/2550.0;
          }
          else {
            return (Model.frsky.channels[index-2].offset*Model.frsky.channels[index-2].ratio)/255.0;
          }
        case TELEMETRY_SOURCE_ALT:
        case TELEMETRY_SOURCE_GPS_ALT:
          return -500;
        case TELEMETRY_SOURCE_T1:
        case TELEMETRY_SOURCE_T2:
          return -30;
        default:
          return 0;
      }
      break;
    default:
      return (Model.extendedLimits ? -125 :-100);
  }
}

double RawSource::getMax(const ModelData & Model)
{
  switch (type) {
    case SOURCE_TYPE_TELEMETRY:
      switch (index) {
        case TELEMETRY_SOURCE_TIMER1:
        case TELEMETRY_SOURCE_TIMER2:
          return 765;
        case TELEMETRY_SOURCE_RSSI_TX:
        case TELEMETRY_SOURCE_RSSI_RX:
          return 100;
        case TELEMETRY_SOURCE_A1:
        case TELEMETRY_SOURCE_A2:
          if (Model.frsky.channels[index-2].type==0) {
            return (Model.frsky.channels[index-2].ratio-(Model.frsky.channels[index-2].offset*Model.frsky.channels[index-2].ratio)/255.0)/10;
          } else {
            return Model.frsky.channels[index-2].ratio-(Model.frsky.channels[index-2].offset*Model.frsky.channels[index-2].ratio)/255.0;
          }
        case TELEMETRY_SOURCE_ALT:
        case TELEMETRY_SOURCE_GPS_ALT:
          return 1540;
        case TELEMETRY_SOURCE_RPM:
          return 12750;
        case TELEMETRY_SOURCE_FUEL:
          return 100;        
        case TELEMETRY_SOURCE_T1:
        case TELEMETRY_SOURCE_T2:
          return 225;
        case TELEMETRY_SOURCE_SPEED:
          return 944;
        case TELEMETRY_SOURCE_DIST:
          return 2040;
        case TELEMETRY_SOURCE_CELL:
          return 5.1;
        case TELEMETRY_SOURCE_CELLS_SUM:
        case TELEMETRY_SOURCE_VFAS:
          return 25.5;
        case TELEMETRY_SOURCE_CURRENT:
          return 127.5;
        case TELEMETRY_SOURCE_CONSUMPTION:
          return 5100;
        case TELEMETRY_SOURCE_POWER:
          return 1275;
        default:
          return 125;
      }
      break;
    default:
      return (Model.extendedLimits ? 125 :100);
  }
}

double RawSource::getOffset(const ModelData & Model)
{
  if (type==SOURCE_TYPE_TELEMETRY) {
    switch (index) {
      case TELEMETRY_SOURCE_A1:
      case TELEMETRY_SOURCE_A2:
        if (Model.frsky.channels[index-2].type==0) {
          return (Model.frsky.channels[index-2].offset*Model.frsky.channels[index-2].ratio)/2550.0;
        }
        else {
          return (Model.frsky.channels[index-2].offset*Model.frsky.channels[index-2].ratio)/255.0;
        }
      case TELEMETRY_SOURCE_ALT:
      case TELEMETRY_SOURCE_GPS_ALT:
        return 524;
      case TELEMETRY_SOURCE_RPM:
        return 6400;
      case TELEMETRY_SOURCE_FUEL:
        return 0;
      case TELEMETRY_SOURCE_T1:
      case TELEMETRY_SOURCE_T2:
        return 98;
      case TELEMETRY_SOURCE_SPEED:
        return 474;
      case TELEMETRY_SOURCE_DIST:
        return 1024;
      case TELEMETRY_SOURCE_CELL:
        return 2.56;
      default:
        return 0;
    }
  }
  return 0;
}

int RawSource::getRawOffset(const ModelData & Model)
{
  switch (type) {
    case SOURCE_TYPE_TELEMETRY:
      switch (index) {
        case TELEMETRY_SOURCE_TIMER1:
        case TELEMETRY_SOURCE_TIMER2:
        case TELEMETRY_SOURCE_RSSI_TX:
        case TELEMETRY_SOURCE_RSSI_RX:
        case TELEMETRY_SOURCE_A1:
        case TELEMETRY_SOURCE_A2:
        case TELEMETRY_SOURCE_FUEL:
        case TELEMETRY_SOURCE_CELLS_SUM:
        case TELEMETRY_SOURCE_VFAS:
        case TELEMETRY_SOURCE_CURRENT:
        case TELEMETRY_SOURCE_CONSUMPTION:
        case TELEMETRY_SOURCE_POWER:
          return 128;
        default:
          return 0;
      }
   default:
      return 0;
  }
}

double RawSource::getStep(const ModelData & Model)
{
  switch (type) {
    case SOURCE_TYPE_TELEMETRY:
      switch (index) {
        case TELEMETRY_SOURCE_TIMER1:
        case TELEMETRY_SOURCE_TIMER2:
          return 3;
        case TELEMETRY_SOURCE_A1:
        case TELEMETRY_SOURCE_A2:
          if (Model.frsky.channels[index-2].type==0) {
            return (Model.frsky.channels[index-2].ratio/2550.0);
          }
          else {
            return (Model.frsky.channels[index-2].ratio/255.0);
          }
        case TELEMETRY_SOURCE_ALT:
        case TELEMETRY_SOURCE_GPS_ALT:
          return 8;
        case TELEMETRY_SOURCE_RPM:
          return 50;
        case TELEMETRY_SOURCE_SPEED:
          return 4;
        case TELEMETRY_SOURCE_DIST:
          return 8;
        case TELEMETRY_SOURCE_CELL:
          return 0.02;
        case TELEMETRY_SOURCE_CELLS_SUM:
        case TELEMETRY_SOURCE_VFAS:
          return 0.1;
        case TELEMETRY_SOURCE_CURRENT:
          return 0.5;
        case TELEMETRY_SOURCE_CONSUMPTION:
          return 20;
        case TELEMETRY_SOURCE_POWER:
          return 5;
        default:
          return 1;
      }
      break;
   default:
      return 1;
  }
}

QString RawSource::toString()
{
  QString sticks9X[] = { QObject::tr("Rud"), QObject::tr("Ele"), QObject::tr("Thr"), QObject::tr("Ail"),
                         QObject::tr("P1"), QObject::tr("P2"), QObject::tr("P3")
                       };

  QString sticksX9D[] = { QObject::tr("Rud"), QObject::tr("Ele"), QObject::tr("Thr"), QObject::tr("Ail"),
                          QObject::tr("S1"), QObject::tr("S2"), QObject::tr("LS"), QObject::tr("RS")
                        };

  QString trims[] = { QObject::tr("TrmR"), QObject::tr("TrmE"), QObject::tr("TrmT"), QObject::tr("TrmA")};

  QString rotary[] = { QObject::tr("REa"), QObject::tr("REb") };

  QString telemetry[] = { QObject::tr("Batt"), QObject::tr("Timer1"), QObject::tr("Timer2"),
                          QObject::tr("Tx"), QObject::tr("Rx"), QObject::tr("A1"), QObject::tr("A2"), QObject::tr("Alt"), QObject::tr("Rpm"), QObject::tr("Fuel"), QObject::tr("T1"),
                          QObject::tr("T2"), QObject::tr("Speed"), QObject::tr("Dist"), QObject::tr("GPS Alt"), QObject::tr("Cell"), QObject::tr("Cels"), QObject::tr("Vfas"), QObject::tr("Curr"),
                          QObject::tr("Cnsp"), QObject::tr("Powr"), QObject::tr("AccX"), QObject::tr("AccY"), QObject::tr("AccZ"), QObject::tr("HDG "), QObject::tr("VSpd"), QObject::tr("A1-"),
                          QObject::tr("A2-"), QObject::tr("Alt-"), QObject::tr("Alt+"), QObject::tr("Rpm+"), QObject::tr("T1+"), QObject::tr("T2+"), QObject::tr("Spd+"), QObject::tr("Dst+"),
                          QObject::tr("Cur+"), QObject::tr("ACC "), QObject::tr("Time") };
  if (index<0) {
    return QObject::tr("----");
  }
  switch(type) {
    case SOURCE_TYPE_STICK:
      return (GetEepromInterface()->getBoard() == BOARD_X9DA ? sticksX9D[index] : sticks9X[index]);
    case SOURCE_TYPE_TRIM:
      return trims[index];
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
      if (index < GetEepromInterface()->getCapability(Outputs))
        return QObject::tr("CH%1%2").arg((index+1)/10).arg((index+1)%10);
      else
        return QObject::tr("X%1").arg(index-GetEepromInterface()->getCapability(Outputs)+1);
    case SOURCE_TYPE_TELEMETRY:
      if (index<int(sizeof(telemetry)/sizeof(QString)))
        return telemetry[index];
      else
        return QObject::tr("----");
      return telemetry[index];
    case SOURCE_TYPE_GVAR:
      return QObject::tr("GV%1").arg(index+1);      
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
  QString switches9X[] = { QObject::tr("THR"), QObject::tr("RUD"), QObject::tr("ELE"),
                           QObject::tr("ID0"), QObject::tr("ID1"), QObject::tr("ID2"),
                           QObject::tr("AIL"), QObject::tr("GEA"), QObject::tr("TRN")
                         };

  QString switchesX9D[] = { QString::fromUtf8("SA\u2191"), QObject::tr("SA-"), QString::fromUtf8("SA\u2193"),
                            QString::fromUtf8("SB\u2191"), QObject::tr("SB-"), QString::fromUtf8("SB\u2193"),
                            QString::fromUtf8("SC\u2191"), QObject::tr("SC-"), QString::fromUtf8("SC\u2193"),
                            QString::fromUtf8("SD\u2191"), QObject::tr("SD-"), QString::fromUtf8("SD\u2193"),
                            QString::fromUtf8("SE\u2191"), QObject::tr("SE-"), QString::fromUtf8("SE\u2193"),
                            QString::fromUtf8("SF\u2191"), QString::fromUtf8("SF\u2193"),
                            QString::fromUtf8("SG\u2191"), QObject::tr("SG-"), QString::fromUtf8("SG\u2193"),
                            QString::fromUtf8("SH\u2191"), QString::fromUtf8("SH\u2193"),
                          };

  switch(type) {
    case SWITCH_TYPE_SWITCH:
      if (GetEepromInterface()->getBoard() == BOARD_X9DA)
        return index > 0 ? switchesX9D[index-1] : QString("!") + switchesX9D[-index-1];
      else
        return index > 0 ? switches9X[index-1] : QString("!") + switches9X[-index-1];
    case SWITCH_TYPE_VIRTUAL:
    {
      QString neg = QString("");
      int ind=index;
      if (index < 0) { 
        neg = QString("!"); 
        ind = -index;
      }
      if (ind < 10)
        return neg+QObject::tr("CS%1").arg(ind);
      else
        return neg+QObject::tr("CS%1").arg(QChar('A'+ind-10));
    }
    case SWITCH_TYPE_ON:
      return QObject::tr("ON");
    case SWITCH_TYPE_ONM:
      return QObject::tr("mON");
    case SWITCH_TYPE_OFF:
      return QObject::tr("OFF");
    case SWITCH_TYPE_MOMENT_SWITCH:
      // TODO assert(index != 0);
      if (GetEepromInterface()->getBoard() == BOARD_X9DA)
        return index > 0 ? QString("m") + switchesX9D[index-1] : QString("!m") + switchesX9D[-index-1];
      else
        return index > 0 ? QString("m") + switches9X[index-1] : QString("!m") + switches9X[-index-1];
    case SWITCH_TYPE_MOMENT_VIRTUAL:
    {
      QString neg = QString("m");
      int ind=index;
      if (index < 0) {
        neg = QString("!m"); 
        ind = -index;
      }
      if (ind < 10)
        return neg+QObject::tr("CS%1").arg(ind);
      else
        return neg+QObject::tr("CS%1").arg(QChar('A'+ind-10));
    }
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
  ppm2NCH = 8;
  ppm2SCH = 8;
  ppmDelay = 300;
  for (int i=0; i<C9X_MAX_PHASES; i++)
    phaseData[i].clear();
  for (int i=0; i<C9X_MAX_EXPOS; i++)
    expoData[i].clear();
  for (int i=0; i<C9X_MAX_MIXERS; i++)
    mixData[i].clear();
  for(int i=0; i<4; i++){
    mixData[i].destCh = i+1;
    mixData[i].srcRaw = RawSource(SOURCE_TYPE_STICK, i);
    mixData[i].weight = 100;
  }
  for (int i=0; i<C9X_NUM_CHNOUT; i++)
    limitData[i].clear();
  for (int i=0; i<NUM_STICKS; i++)
    expoData[i].clear();
  for (int i=0; i<C9X_NUM_CSW; i++)
    customSw[i].clear();
  bool custom = GetEepromInterface()->getCapability(CustomCurves);
  for (int i=0; i<C9X_MAX_CURVES; i++) {
    if (!custom && i>=8)
      curves[i].clear(9);
    else
      curves[i].clear(5);
  }

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

  for (uint8_t i=0; i<C9X_MAX_PHASES; i++) {
    if (phase == 0 || phaseData[phase].trimRef[idx] < 0) return phase;
    phase = phaseData[phase].trimRef[idx];
  }
  return 0;
}

void ModelData::removeGlobalVar(int & var)
{
  if (var >= 126 && var <= 130)
    var = phaseData[0].gvars[var-126];
  else if (var <= -126 && var >= -130)
    var = - phaseData[0].gvars[-126-var];
}

ModelData ModelData::removeGlobalVars()
{
  ModelData result = *this;

  for (int i=0; i<C9X_MAX_MIXERS; i++) {
    removeGlobalVar(mixData[i].weight);
    removeGlobalVar(mixData[i].differential);
    removeGlobalVar(mixData[i].sOffset);
  }

  for (int i=0; i<C9X_MAX_EXPOS; i++) {
    removeGlobalVar(expoData[i].weight);
    removeGlobalVar(expoData[i].expo);
  }

  return result;
}

QList<EEPROMInterface *> eepromInterfaces;
void RegisterEepromInterfaces()
{
  eepromInterfaces.push_back(new Open9xInterface(BOARD_STOCK));
  eepromInterfaces.push_back(new Open9xInterface(BOARD_M128));
  eepromInterfaces.push_back(new Open9xInterface(BOARD_GRUVIN9X));
  eepromInterfaces.push_back(new Open9xInterface(BOARD_SKY9X));
  eepromInterfaces.push_back(new Gruvin9xInterface(BOARD_STOCK));
  eepromInterfaces.push_back(new Gruvin9xInterface(BOARD_GRUVIN9X));
  eepromInterfaces.push_back(new Ersky9xInterface());
  eepromInterfaces.push_back(new Th9xInterface());
  eepromInterfaces.push_back(new Er9xInterface());
}

QList<FirmwareInfo *> firmwares;
FirmwareVariant default_firmware_variant;
FirmwareVariant current_firmware_variant;

const char * ER9X_STAMP = "http://er9x.googlecode.com/svn/trunk/src/stamp-er9x.h";
const char * ERSKY9X_STAMP = "http://ersky9x.googlecode.com/svn/trunk/src/stamp-ersky9x.h";

void RegisterFirmwares()
{
  firmwares.push_back(new FirmwareInfo("th9x", QObject::tr("th9x"), new Th9xInterface(), "http://th9x.googlecode.com/svn/trunk/%1.bin", "http://th9x.googlecode.com/svn/trunk/src/stamp-th9x.h"));

  firmwares.push_back(new FirmwareInfo("er9x", QObject::tr("er9x"), new Er9xInterface(), "http://er9x.googlecode.com/svn/trunk/%1.hex", ER9X_STAMP));
  FirmwareInfo * er9x = firmwares.last();

  Option er9x_options[] = { { "noht", "", 0 }, { "frsky", "", 0 }, { "frsky-noht", "", 0 }, { "jeti", "", 0 }, { "ardupilot", "", 0 }, { "nmea", "", 0 }, { NULL } };
  er9x->addOptions(er9x_options);
  er9x->addOption("noht");

  RegisterOpen9xFirmwares();

  firmwares.push_back(new FirmwareInfo("ersky9x", QObject::tr("ersky9x"), new Ersky9xInterface(), "http://ersky9x.googlecode.com/svn/trunk/ersky9x_rom.bin", ERSKY9X_STAMP));

  default_firmware_variant = GetFirmwareVariant("open9x-stock-heli-templates-en");

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

bool LoadBackup(RadioData &radioData, uint8_t *eeprom, int size, int index)
{
  foreach(EEPROMInterface *eepromInterface, eepromInterfaces) {
    if (eepromInterface->loadBackup(radioData, eeprom, size, index))
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


FirmwareVariant GetFirmwareVariant(QString id)
{
  FirmwareVariant result;

  foreach(FirmwareInfo * firmware, firmwares) {
    
    if (id.contains(firmware->id+"-") || (!id.contains("-") && id.contains(firmware->id))) {
      result.id = id;
      result.firmware = firmware;
      result.variant = firmware->getVariant(id);
      return result;
    }
  }

  return default_firmware_variant;
}

void FirmwareInfo::addOption(const char *option, QString tooltip, uint32_t variant)
{
  Option options[] = { { option, tooltip, variant }, { NULL } };
  addOptions(options);
}

unsigned int FirmwareInfo::getVariant(const QString & variantId)
{
  unsigned int variant = variantBase;
  QStringList options = variantId.mid(id.length()+1).split("-", QString::SkipEmptyParts);
  foreach(QString option, options) {
    foreach(QList<Option> group, opts) {
      foreach(Option opt, group) {
        if (opt.name == option) {
          variant += opt.variant;
        }
      }
    }
  }
  return variant;
}

void FirmwareInfo::addLanguage(const char *lang)
{
  languages.push_back(lang);
}

void FirmwareInfo::addTTSLanguage(const char *lang)
{
  ttslanguages.push_back(lang);
}

void FirmwareInfo::addOptions(Option options[])
{
  QList<Option> opts;
  for (int i=0; options[i].name; i++) {
    opts.push_back(options[i]);
  }
  this->opts.push_back(opts);
}
