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

int RawSource::getDecimals(const ModelData & Model)
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

double RawSource::getMin(const ModelData & Model)
{
  switch (type) {
    case SOURCE_TYPE_TIMER:
      return 0;
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
    default:
      return (Model.extendedLimits ? 125 :100);
  }
}

double RawSource::getOffset(const ModelData & Model)
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

  QString telemetry[] = { QObject::tr("A1"), QObject::tr("A2"), QObject::tr("TX"), QObject::tr("RX"), QObject::tr("Alt"), QObject::tr("Rpm"), QObject::tr("Fuel"), QObject::tr("T1"), QObject::tr("T2"), QObject::tr("Speed"), QObject::tr("Dist"), QObject::tr("GPS Alt"), QObject::tr("Cell") };

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
  bool custom = GetEepromInterface()->getCapability(CustomCurves);
  for (int i=0; i<MAX_CURVES; i++) {
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
QString default_firmware_id;
FirmwareInfo * default_firmware = NULL;
QString current_firmware_id;
FirmwareInfo * current_firmware = NULL;

const char * ER9X_STAMP = "http://er9x.googlecode.com/svn/trunk/src/stamp-er9x.h";
const char * ERSKY9X_STAMP = "http://ersky9x.googlecode.com/svn/trunk/src/stamp-ersky9x.h";

#define OPEN9X_PREFIX_URL "http://85.18.253.250/getfw.php?fw="
const char * OPEN9X_STOCK_STAMP = "http://85.18.253.250/binaries/stamp-open9x-stock.txt";
const char * OPEN9X_V4_STAMP = "http://85.18.253.250/binaries/stamp-open9x-v4.txt";
const char * OPEN9X_ARM_STAMP = "http://85.18.253.250/binaries/stamp-open9x-arm.txt";

void RegisterFirmwares()
{
  firmwares.push_back(new FirmwareInfo("th9x", QObject::tr("th9x"), new Th9xInterface(), "http://th9x.googlecode.com/svn/trunk/%1.bin", "http://th9x.googlecode.com/svn/trunk/src/stamp-th9x.h"));

  firmwares.push_back(new FirmwareInfo("er9x", QObject::tr("er9x"), new Er9xInterface(), "http://er9x.googlecode.com/svn/trunk/%1.hex", ER9X_STAMP));
  FirmwareInfo * er9x = firmwares.last();
  const char *er9x_options[] = {"frsky", "jeti", "ardupilot", "nmea", NULL};
  er9x->addOptions(er9x_options);
  er9x->addOption("noht");

  FirmwareInfo * gruvin9x = new FirmwareInfo("gruvin9x-stock", QObject::tr("gruvin9x stable for stock board"), new Gruvin9xInterface(BOARD_STOCK), "http://gruvin9x.googlecode.com/svn/branches/frsky/%1.hex");
  firmwares.push_back(gruvin9x);
  gruvin9x->addOption("frsky");
  gruvin9x->addOption("speaker");
  firmwares.push_back(new FirmwareInfo("gruvin9x-trunk-stock", QObject::tr("gruvin9x trunk for stock board"), new Gruvin9xInterface(BOARD_STOCK)));
  firmwares.push_back(new FirmwareInfo("gruvin9x-v4", QObject::tr("gruvin9x stable for v4 board"), new Gruvin9xInterface(BOARD_GRUVIN9X), "http://gruvin9x.googlecode.com/svn/branches/frsky/gruvin9x.hex"));
  firmwares.push_back(new FirmwareInfo("gruvin9x-trunk-v4", QObject::tr("gruvin9x trunk for v4 board"), new Gruvin9xInterface(BOARD_GRUVIN9X)));

  Open9xFirmware * open9x = new Open9xFirmware("open9x-stock", QObject::tr("open9x for stock board"), new Open9xInterface(BOARD_STOCK), OPEN9X_PREFIX_URL "%1.hex", OPEN9X_STOCK_STAMP);
  open9x->addLanguage("en");
  open9x->addLanguage("fr");
  open9x->addLanguage("se");
  open9x->addLanguage("de");
  open9x->addLanguage("it");
  open9x->addLanguage("cz");
  const char *ext_options[] = {"frsky", "jeti", "ardupilot", "nmea", NULL};
  open9x->addOptions(ext_options);
  open9x->addOption("heli");
  open9x->addOption("templates");
  open9x->addOption("nosplash");
  open9x->addOption("nofp");
  open9x->addOption("nocurves");
  open9x->addOption("audio");
  open9x->addOption("voice");
  open9x->addOption("haptic");
  open9x->addOption("PXX");
  open9x->addOption("DSM2");
  open9x->addOption("ppmca");
  open9x->addOption("potscroll");
  open9x->addOption("sp22");
  open9x->addOption("autoswitch");
  open9x->addOption("nographics");
  open9x->addOption("nobold");
  open9x->addOption("pgbar");
  open9x->addOption("imperial");
  firmwares.push_back(open9x);

  open9x = new Open9xFirmware("open9x-v4", QObject::tr("open9x for gruvin9x board"), new Open9xInterface(BOARD_GRUVIN9X), OPEN9X_PREFIX_URL "%1.hex", OPEN9X_V4_STAMP);
  open9x->addLanguage("en");
  open9x->addLanguage("fr");
  open9x->addLanguage("se");
  open9x->addLanguage("de");
  open9x->addLanguage("it");
  open9x->addLanguage("cz");
  open9x->addOption("heli");
  open9x->addOption("templates");
  open9x->addOption("nofp");
  open9x->addOption("nocurves");
  open9x->addOption("sdcard");
  open9x->addOption("voice");
  open9x->addOption("ppmca");
  open9x->addOption("autoswitch");
  open9x->addOption("nographics");
  open9x->addOption("nobold");
  open9x->addOption("pgbar");
  open9x->addOption("imperial");
  firmwares.push_back(open9x);

  open9x = new Open9xFirmware("open9x-arm", QObject::tr("open9x for ersky9x board"), new Open9xInterface(BOARD_ERSKY9X), OPEN9X_PREFIX_URL "%1.bin", OPEN9X_STOCK_STAMP);
  open9x->addLanguage("en");
  open9x->addLanguage("fr");
  open9x->addLanguage("se");
  open9x->addLanguage("de");
  open9x->addLanguage("it");
  open9x->addLanguage("cz");
  open9x->addOption("heli");
  open9x->addOption("templates");
  open9x->addOption("nofp");
  open9x->addOption("nocurves");
  open9x->addOption("autoswitch");
  open9x->addOption("nographics");
  open9x->addOption("nobold");
  open9x->addOption("imperial");
  firmwares.push_back(open9x);

  firmwares.push_back(new FirmwareInfo("ersky9x", QObject::tr("ersky9x"), new Ersky9xInterface(), "http://ersky9x.googlecode.com/svn/trunk/ersky9x_rom.bin", ERSKY9X_STAMP));

  default_firmware_id = "open9x-stock-heli-templates-en";
  default_firmware = GetFirmware(default_firmware_id);

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
    if (id.contains(firmware->id))
      return firmware;
  }
  return NULL;
}

void FirmwareInfo::addOption(const char *option)
{
  const char *options[] = { option, NULL };
  addOptions(options);
}

void FirmwareInfo::addLanguage(const char *lang)
{
  languages.push_back(lang);
}

void FirmwareInfo::addOptions(const char *options[])
{
  QList<const char *> opts;
  for (int i=0; options[i]; i++) {
    opts.push_back(options[i]);
  }
  this->opts.push_back(opts);
}
