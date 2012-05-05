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
#include <QtGui>
#include "hexinterface.h"
#include "splash.h"
#include "flashinterface.h"

int getFileType(const QString &fullFileName)
{
  if(QFileInfo(fullFileName).suffix().toUpper()=="HEX")  return FILE_TYPE_HEX;
  if(QFileInfo(fullFileName).suffix().toUpper()=="BIN")  return FILE_TYPE_BIN;
  if(QFileInfo(fullFileName).suffix().toUpper()=="EEPM") return FILE_TYPE_EEPM;
  if(QFileInfo(fullFileName).suffix().toUpper()=="EEPE") return FILE_TYPE_EEPE;
  if(QFileInfo(fullFileName).suffix().toUpper()=="XML") return FILE_TYPE_XML;
  return 0;
}

FlashInterface::FlashInterface(QString fileName)
{
  uint8_t temp[MAX_FSIZE];
  version = "";
  date = "";
  time = "";
  svn = "";
  build = "";
  isValidFlag=true;
  QFile file(fileName);
  flash_size=0;
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) { //reading HEX TEXT file
    isValidFlag = false;
  }
  else {
    QTextStream inputStream(&file);
    flash_size = HexInterface(inputStream).load(temp, MAX_FSIZE);
    file.close();
    inputStream.reset();
    if (flash_size == 0) {
      QFile file(fileName);
      file.open(QIODevice::ReadOnly);
      char bin_flash[MAX_FSIZE];
      flash_size = file.read(bin_flash, MAX_FSIZE);
      flash = QByteArray::fromRawData(bin_flash, flash_size);
    }
    else {
      flash = QByteArray::fromRawData((const char *) temp, flash_size);
    }
    if (flash_size > 0) {
      SeekVer();
      SeekSvn();
      SeekDate();
      SeekTime();
      SeekBuild();
      SeekSplash();
    } else {
      isValidFlag=false;
    }
  }
}

QString FlashInterface::getVers(void)
{
  return version;
}

QString FlashInterface::getDate(void)
{
  return date;
}

QString FlashInterface::getTime(void)
{
  return time;
}

QString FlashInterface::getSvn(void)
{
  return svn;
}

QString FlashInterface::getBuild(void)
{
  return build;
}

int FlashInterface::getSize()
{
  return flash_size;
}

void FlashInterface::SeekVer(void)
{
  int i, start = -1, end = -1;
  start = flash.indexOf(QString(VERS_MARK));
  if (start > 0) {
    start += QString(VERS_MARK).length();
    for (i = start; i < (start + 20); i++) {
      if (flash.at(i) == 0) {
        end = i;
        break;
      }
    }
    if (end > 0) {
      version = QString(flash.mid(start, (end - start))).trimmed();
    }
    else {
      version = QString("");
    }
  } else {
    isValidFlag=false;
  }
}

void FlashInterface::SeekSvn(void) 
{
  int i, start = -1, end = -1;
  start = flash.indexOf(QString(SVN_MARK));
  if (start > 0) {
    start += QString(SVN_MARK).length();
    for (i = start; i < (start + 20); i++) {
      if (flash.at(i) == 0) {
        end = i;
        break;
      }
    }
    if (end > 0) {
      svn = QString(flash.mid(start, (end - start))).trimmed();
    }
    else {
      svn = QString("");
    }
  }
}

void FlashInterface::SeekDate(void) 
{
  int i, start = -1, end = -1, startsvn=0;
  startsvn = flash.indexOf(QString(SVN_MARK));
  if (startsvn>0) {
    start = flash.indexOf(QString(DATE_MARK),startsvn);
  } else {
    start = flash.indexOf(QString(DATE_MARK));
  }
  if (start > 0) {
    start += QString(DATE_MARK).length();
    for (i = start; i < (start + 20); i++) {
      if (flash.at(i) == 0) {
        end = i;
        break;
      }
    }
    if (end > 0) {
      date = QString(flash.mid(start, (end - start))).trimmed();
    }
    else {
      date = QString("");
    }
  }
}

void FlashInterface::SeekTime(void)
{
  int i, start = -1, end = -1, startsvn=0;
  startsvn = flash.indexOf(QString(SVN_MARK));
  if (startsvn>0) {
    start = flash.indexOf(QString(TIME_MARK),startsvn);
  } else {
    start = flash.indexOf(QString(TIME_MARK));
  }
  if (start > 0) {
    start += QString(TIME_MARK).length();
    for (i = start; i < (start + 20); i++) {
      if (flash.at(i) == 0) {
        end = i;
        break;
      }
    }
    if (end > 0) {
      time = QString(flash.mid(start, (end - start))).trimmed();
    }
    else {
      time = QString("");
    }
  }
}

void FlashInterface::SeekBuild(void) 
{
  int i, start = -1, end = -1;
  start = flash.indexOf(QString(BLD_MARK));
  if (start > 0) {
    start += QString(BLD_MARK).length();
    for (i = start; i < (start + 20); i++) {
      if (flash.at(i) == 0) {
        end = i;
        break;
      }
    }
    if (end > 0) {
      build = QString(flash.mid(start, (end - start))).trimmed();
    }
    else {
      build = QString("");
    }
  }
  else {
    start = flash.indexOf(QString(MOD_MARK));
    if (start > 0) {
      start += QString(MOD_MARK).length();
      for (i = start; i < (start + 20); i++) {
        if (flash.at(i) == 0) {
          end = i;
          break;
        }
      }
      if (end > 0) {
        build = QString(flash.mid(start, (end - start))).trimmed();
      }
      else {
        build = QString("");
      }
    }
  }
}

void FlashInterface::SeekSplash(void) 
{
  QByteArray splash,spe;
  splash_offset=0;
  splash.clear();
  splash.append(gr9x_splash, sizeof(gr9x_splash));
  int start = flash.indexOf(splash);
  if (start>0) {
    splash_offset=start;
    splash_type=1;
    splash_size=sizeof(gr9x_splash);
  } 
  if (start==-1) {
    splash.clear();
    splash.append(er9x_splash, sizeof(er9x_splash));
    start = flash.indexOf(splash);
    if (start>0) {
      splash_offset=start;
      splash_type=2;
      splash_size=sizeof(er9x_splash);
    }
  }
  if (start==-1) {
    splash.clear();
    splash.append(open9x_splash, sizeof(open9x_splash));
    start = flash.indexOf(splash);
    if (start>0) {
      splash_offset=start;
      splash_type=3;
      splash_size=sizeof(open9x_splash);
    }
  }
  if (start==-1) {
    splash.clear();
    splash.append(gr9xv4_splash, sizeof(gr9xv4_splash));
    start = flash.indexOf(splash);
    if (start>0) {
      splash_offset=start;
      splash_type=1;
      splash_size=sizeof(gr9xv4_splash);
    }
  }
  if (start==-1) {
    splash.clear();
    splash.append(ersky9x_splash, sizeof(ersky9x_splash));
    start = flash.indexOf(splash);
    if (start>0) {
      splash_offset=start;
      splash_type=4;
      splash_size=sizeof(ersky9x_splash);
    }
  }
  if (start==-1) {
    start=0;
    splash.clear();
    splash.append(O9X_SPS);
    splash.append('\0');
    spe.clear();
    spe.append(O9X_SPE);
    spe.append('\0');   
    int diff=0;
    int end=0;
    while (start>=0) {
      start = flash.indexOf(splash,start+1);
      if (start>0) {
        end=flash.indexOf(spe,start+1);
        if (end>0) {
          diff=end-start;
          while (diff<1030 && end>0) {
            end=flash.indexOf(spe,end+1);
            diff=end-start;
          }
          if (end>0) {
            diff=end-start;
            if (diff==1030) {
              splash_offset=start+O9X_OFFSET;
              splash_type=2;
              splash_size=sizeof(open9x_splash);
              break;
            }
          }
        }
      }
    }
  }
  if (start==-1) {
    start=0;
    splash.clear();
    splash.append(ERSKY9X_SPS);
    splash.append('\0');
    spe.clear();
    spe.append(ERSKY9X_SPE);
    spe.append('\0');   
    int diff=0;
    int end=0;
    while (start>=0) {
      start = flash.indexOf(splash,start+1);
      if (start>0) {
        end=flash.indexOf(spe,start+1);
        if (end>0) {
          diff=end-start;
          while (diff<1031 && end>0) {
            end=flash.indexOf(spe,end+1);
            diff=end-start;
          }
          if (end>0) {
            diff=end-start;
            if (diff==1031) {
              splash_offset=start+ERSKY9X_OFFSET;
              splash_type=4;
              splash_size=sizeof(ersky9x_splash);
              break;
            }
          }
        }
      }
    }
  }
  if (start==-1) {
    splash.clear();
    splash.append(ERSPLASH_MARKER);
    splash.append('\0');
    start = flash.indexOf(splash);
    if (start>0) {
      splash_offset=start+ERSPLASH_OFFSET;
      splash_type=2;
      splash_size=sizeof(er9x_splash);
    }
  }

}

bool FlashInterface::setSplash(QImage newsplash) 
{
  char b[SPLASH_SIZE]= {0};
  quint8 * p = newsplash.bits();
  QByteArray splash;
  if (splash_offset==0) {
    return false;
  }
  else {
    for(int y=0; y<SPLASH_HEIGHT; y++)
        for(int x=0; x<SPLASH_WIDTH; x++)
            b[SPLASH_WIDTH*(y/8) + x] |= ((p[(y*SPLASH_WIDTH + x)/8] & (1<<(x%8))) ? 1 : 0)<<(y % 8);

    splash.clear();
    splash.append(b, sizeof(b));
    flash.replace(splash_offset,splash_size,splash);
    return true;
  }
}

QImage FlashInterface::getSplash()
{
  uint k=0;
  QImage image(128, 64, QImage::Format_Mono);
  uchar b[SPLASH_SIZE] = {0};
  if (splash_offset>0) {
    for (k=0; k<sizeof(b); k++)
      b[k]=flash.at(splash_offset+k);
    for(int y=0; y<SPLASH_HEIGHT; y++)
      for(int x=0; x<SPLASH_WIDTH; x++)
        image.setPixel(x,y,((b[SPLASH_WIDTH*(y/8) + x]) & (1<<(y % 8))) ? 0 : 1  );
  }
  return image;
}

bool FlashInterface::hasSplash()
{
  return (splash_offset > 0 ? true : false);
}

bool FlashInterface::isValid()
{
  return isValidFlag;
}


uint FlashInterface::saveFlash(QString fileName)
{
  uint8_t binflash[MAX_FSIZE];
  memcpy(&binflash, flash.constData(), flash_size);
  QFile file(fileName);
  
  int fileType = getFileType(fileName);

  if (fileType == FILE_TYPE_HEX) {
    if (!file.open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text)) { //reading HEX TEXT file
      return -1;
    }
    QTextStream outputStream(&file);
    HexInterface hex=HexInterface(outputStream);
    hex.save(binflash, flash_size);
  }
  else {
    if (!file.open(QIODevice::ReadWrite | QIODevice::Truncate)) { //reading HEX TEXT file
      return -1;
    }
    file.write((char*)binflash, flash_size);
  }

  file.close();

  return flash_size;
}
