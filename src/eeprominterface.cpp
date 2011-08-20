#include <stdio.h>
#include <list>
#include "eeprominterface.h"
#include "er9xinterface.h"
#include "gruvin9xinterface.h"
#include "qsettings.h"

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
}

ModelData::ModelData()
{
  clear();
}

void ModelData::clear()
{
  memset(this, 0, sizeof(ModelData));
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
  sprintf(name, "MODEL %02d", id+1);
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

std::list<EEPROMInterface *> eeprom_interfaces;

void RegisterEepromInterfaces()
{
  eeprom_interfaces.push_back(new Er9xInterface());
  eeprom_interfaces.push_back(new Gruvin9xInterface());
}

bool LoadEeprom(RadioData &radioData, uint8_t eeprom[EESIZE])
{
  for (std::list<EEPROMInterface *>::iterator i=eeprom_interfaces.begin(); i!=eeprom_interfaces.end(); i++) {
    if ((*i)->load(radioData, eeprom))
      return true;
  }

  return false;
}

EEPROMInterface *GetEepromInterface()
{
  static EEPROMInterface * eepromInterface = NULL;

  delete eepromInterface;

  QSettings settings("er9x-eePe", "eePe");
  if (settings.value("download-version", 0).toInt() == DNLD_VER_GRUVIN9X) {
    eepromInterface = new Gruvin9xInterface();
  }
  else {
    eepromInterface = new Er9xInterface();
  }

  return eepromInterface;
}
