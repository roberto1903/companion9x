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
      case 14:
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
    case SOURCE_TYPE_TIMER:
      return 0;
    case SOURCE_TYPE_TELEMETRY:
      switch (index) {
        case 2:
        case 3:
          if (Model.frsky.channels[index-2].type==0) {
            return (Model.frsky.channels[index-2].offset*Model.frsky.channels[index-2].ratio)/2550.0;
          } else {
            return (Model.frsky.channels[index-2].offset*Model.frsky.channels[index-2].ratio)/255.0;
          }
        case 0:
        case 1:
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
        case 13:
        case 14:
        case 15:
        case 16:
        case 17:
          return 0;
        default:
          return -125;
      }
      break;
    default:
      return (Model.extendedLimits ? -125 :-100);
  }
}

double RawSource::getMax(const ModelData & Model)
{
  switch (type) {
    case SOURCE_TYPE_TIMER:
      return 765;
    case SOURCE_TYPE_TELEMETRY:
      switch (index) {
        case 0:
        case 1:
          return 100;
        case 2:
        case 3:
          if (Model.frsky.channels[index-2].type==0) {
            return (Model.frsky.channels[index-2].ratio-(Model.frsky.channels[index-2].offset*Model.frsky.channels[index-2].ratio)/255.0)/10;
          } else {
            return Model.frsky.channels[index-2].ratio-(Model.frsky.channels[index-2].offset*Model.frsky.channels[index-2].ratio)/255.0;
          }
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
        case 13:
        case 14:
          return 25.5;
        case 15:
          return 127.5;
        case 16:
          return 5100;
        case 17:
          return 1275;
        default:
          return 125;
      }
      break;
    default:
      return (Model.extendedLimits ? 125 :100);
  }
}
double RawSource::getCsMin(const ModelData & Model)
{
  switch (type) {
    case SOURCE_TYPE_TIMER:
      return 0;
    case SOURCE_TYPE_TELEMETRY:
      switch (index) {
        case 2:
        case 3:
          if (Model.frsky.channels[index-2].type==0) {
            return (Model.frsky.channels[index-2].offset*Model.frsky.channels[index-2].ratio)/2550.0;
          } else {
            return (Model.frsky.channels[index-2].offset*Model.frsky.channels[index-2].ratio)/255.0;
          }
        case 0:
        case 1:
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
        case 13:
        case 14:
        case 15:
        case 16:
        case 17:
          return 0;
        default:
          return -125;
      }
      break;
    default:
      return -125;
  }
}

double RawSource::getCsMax(const ModelData & Model)
{
  switch (type) {
    case SOURCE_TYPE_TIMER:
      return 765;
    case SOURCE_TYPE_TELEMETRY:
      switch (index) {
        case 0:
        case 1:
          return 100;
        case 2:
        case 3:
          if (Model.frsky.channels[index-2].type==0) {
            return (Model.frsky.channels[index-2].ratio-(Model.frsky.channels[index-2].offset*Model.frsky.channels[index-2].ratio)/255.0)/10;
          } else {
            return Model.frsky.channels[index-2].ratio-(Model.frsky.channels[index-2].offset*Model.frsky.channels[index-2].ratio)/255.0;
          }
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
        case 13:
        case 14:
          return 25.5;
        case 15:
          return 127.5;
        case 16:
          return 5100;
        case 17:
          return 1275;
        default:
          return 125;
      }
      break;
    default:
      return 125;
  }
}

double RawSource::getOffset(const ModelData & Model)
{
  if(type==SOURCE_TYPE_TELEMETRY) {
    switch (index) {
      case 0:
      case 1:
        return 0;
      case 2:
      case 3:
        if (Model.frsky.channels[index-2].type==0) {
          return (Model.frsky.channels[index-2].offset*Model.frsky.channels[index-2].ratio)/2550.0;
        } else {
          return (Model.frsky.channels[index-2].offset*Model.frsky.channels[index-2].ratio)/255.0;
        }
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

int RawSource::getRawOffset(const ModelData & Model)
{
  switch (type) {
    case SOURCE_TYPE_TELEMETRY:
      switch (index) {
        case 0:
        case 1:
        case 2:
        case 3:
        case 6:
        case 13:
        case 14:
        case 15:
        case 16:
        case 17:
          return 128;        
        default:
          return 0;
      }
      break;
   case SOURCE_TYPE_TIMER:
      return 128;
      break;
   default:
      return 0;
  }
}

double RawSource::getStep(const ModelData & Model)
{
  switch (type) {
    case SOURCE_TYPE_TELEMETRY:
      switch (index) {
        case 0:
        case 1:
          return 1;
        case 2:
        case 3:
          if (Model.frsky.channels[index-2].type==0) {
            return (Model.frsky.channels[index-2].ratio/2550.0);
          } else {
            return (Model.frsky.channels[index-2].ratio/255.0);
          }
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
        case 13:
        case 14:
          return 0.1;
        case 15:
          return 0.5;
        case 16:
          return 20;
        case 17:
          return 5;
        default:
          return 1;
      }
      break;
    case SOURCE_TYPE_TIMER:
      return 3;
      break;
   default:
      return 1;
  }
}

QString RawSource::toString()
{
  QString sticks[] = { QObject::tr("Rud"), QObject::tr("Ele"), QObject::tr("Thr"), QObject::tr("Ail"),
                       QObject::tr("P1"), QObject::tr("P2"), QObject::tr("P3")
                     };

  QString trims[] = { QObject::tr("TrmR"), QObject::tr("TrmE"), QObject::tr("TrmT"), QObject::tr("TrmA")};

  QString rotary[] = { QObject::tr("REa"), QObject::tr("REb") };

  QString telemetry[] = { QObject::tr("TX"), QObject::tr("RX"), QObject::tr("A1"), QObject::tr("A2"), QObject::tr("Alt"), QObject::tr("Rpm"), QObject::tr("Fuel"), QObject::tr("T1"), 
QObject::tr("T2"), QObject::tr("Speed"), QObject::tr("Dist"), QObject::tr("GPS Alt"), QObject::tr("Cell"), QObject::tr("Cels"), QObject::tr("Vfas"), QObject::tr("Curr"), 
QObject::tr("Cnsp"), QObject::tr("Powr"), QObject::tr("AccX"), QObject::tr("AccY"), QObject::tr("AccZ"), QObject::tr("HDG "), QObject::tr("VSpd"), QObject::tr("A1-"),
QObject::tr("A2-"), QObject::tr("Alt-"), QObject::tr("Alt+"), QObject::tr("Rpm+"), QObject::tr("T1+"), QObject::tr("T2+"), QObject::tr("Spd+"), QObject::tr("Dst+"),
QObject::tr("Cur+"), QObject::tr("ACC "), QObject::tr("Time") }; 
  switch(type) {
    case SOURCE_TYPE_STICK:
      return sticks[index];
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
    case SOURCE_TYPE_TIMER:
      return QObject::tr("Timer%1").arg(index+1);
    case SOURCE_TYPE_TELEMETRY:
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
  QString switches[] = { QObject::tr("THR"), QObject::tr("RUD"), QObject::tr("ELE"),
                         QObject::tr("ID0"), QObject::tr("ID1"), QObject::tr("ID2"),
                         QObject::tr("AIL"), QObject::tr("GEA"), QObject::tr("TRN")
                       };

  switch(type) {
    case SWITCH_TYPE_SWITCH:
      return index > 0 ? switches[index-1] : QString("!") + switches[-index-1];
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
      return index > 0 ? QString("m") + switches[index-1] : QString("!m") + switches[-index-1];
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

#define GVAR_MAX 1024

void ModelData::removeGlobalVar(int & var)
{
  if (var == 126)
    var = phaseData[0].gvars[0];
  else if (var == 127)
    var = phaseData[0].gvars[1];
  else if (var == -128)
    var = phaseData[0].gvars[2];
  else if (var == -127)
    var = phaseData[0].gvars[3];
  else if (var == -126)
    var = phaseData[0].gvars[4];
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
