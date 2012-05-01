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
#ifndef open9x_interface_h
#define open9x_interface_h

#include "eeprominterface.h"

class EFile;

class Open9xInterface : public EEPROMInterface
{
  public:

    Open9xInterface(BoardEnum board);

    virtual ~Open9xInterface();

    virtual const char * getName();
    
    virtual BoardEnum getBoard() { return board; }

    virtual const int getEEpromSize();

    virtual const int getMaxModels();

    virtual bool load(RadioData &, uint8_t *eeprom, int size);
    
    virtual bool loadxml(RadioData &radioData, QDomDocument &doc);

    virtual int save(uint8_t *eeprom, RadioData &radioData, uint8_t version=0);

    virtual int getSize(ModelData &);

    virtual int getSize(GeneralSettings &);
    
    virtual int getCapability(const Capability);
    
    virtual int hasProtocol(Protocol proto);
    
    virtual SimulatorInterface * getSimulator();

  protected:

    template <class T>
    void loadModel(ModelData &model, uint8_t index, unsigned int stickMode=0);

    template <class T>
    bool saveModel(unsigned int index, ModelData &model);

    template <class T>
    bool loadGeneral(GeneralSettings &settings);

    EFile *efile;

    BoardEnum board;

};

class Open9xFirmware: public FirmwareInfo
{
  public:
    Open9xFirmware(const char * id, const QString & name, EEPROMInterface * eepromInterface):
      FirmwareInfo(id, name, eepromInterface)
    {
    }

    Open9xFirmware(const char * id, EEPROMInterface * eepromInterface, QString url, const char * stamp):
      FirmwareInfo(id, eepromInterface, url, stamp)
    {
    }

    virtual unsigned int getEepromVersion(unsigned int revision) {
      switch(this->eepromInterface->getBoard()) {
        case BOARD_ERSKY9X:
          if (revision == 0)
            return 208;
          if (revision >= 641)
            return 208;
          break;
        case BOARD_GRUVIN9X:
          if (revision == 0)
            return 208;
          if (revision >= 641)
            return 208;
          if (revision >= 547)
            return 207;
          break;
        default:
          break;
      }
      if (revision >= 321)
        return 205;
      else if (revision >= 217)
        return 204;
      else if (revision >= 184)
        return 203;
      else
        return 202;
    }

    virtual void addOptions();

};

#endif
