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
  date = "";
  time = "";
  svn = "";
  build = "";
  isValidFlag = true;
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
      SeekSvn();
      SeekDate();
      SeekTime();
      SeekBuild();
      SeekSplash();
    }
    else {
      isValidFlag = false;
    }
  }
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
    start = flash.indexOf(QString(VAR_MARK));
    if (start > 0) {
      start += QString(VAR_MARK).length();
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
  splash.append((char *)gr9x_splash, sizeof(gr9x_splash));
  int start = flash.indexOf(splash);
  if (start>0) {
    splash_offset=start;
    splash_type=1;
    splash_size=sizeof(gr9x_splash);
  } 
  if (start==-1) {
    splash.clear();
    splash.append((char *)er9x_splash, sizeof(er9x_splash));
    start = flash.indexOf(splash);
    if (start>0) {
      splash_offset=start;
      splash_type=2;
      splash_size=sizeof(er9x_splash);
    }
  }
  if (start==-1) {
    splash.clear();
    splash.append((char *)open9x_splash, sizeof(open9x_splash));
    start = flash.indexOf(splash);
    if (start>0) {
      splash_offset=start;
      splash_type=3;
      splash_size=sizeof(open9x_splash);
    }
  }
  if (start==-1) {
    splash.clear();
    splash.append((char *)open9xx9d_splash, sizeof(open9xx9d_splash));
    start = flash.indexOf(splash);
    if (start>0) {
      splash_offset=start;
      splash_type=5;
      splash_size=sizeof(open9xx9d_splash);
    }
  }
  if (start==-1) {
    splash.clear();
    splash.append((char *)gr9xv4_splash, sizeof(gr9xv4_splash));
    start = flash.indexOf(splash);
    if (start>0) {
      splash_offset=start;
      splash_type=1;
      splash_size=sizeof(gr9xv4_splash);
    }
  }
  if (start==-1) {
    splash.clear();
    splash.append((char *)ersky9x_splash, sizeof(ersky9x_splash));
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
            if (diff==1702) {
              splash_offset=start+O9X_OFFSET;
              splash_type=5;
              splash_size=sizeof(open9xx9d_splash);
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
  if (splash_offset>0) {
    if (splash_type==5) {
      splash_width=SPLASHX9D_WIDTH;
      splash_height=SPLASHX9D_HEIGHT;
    } else {
      splash_width=SPLASH_WIDTH;
      splash_height=SPLASH_HEIGHT;
    }
  }
}

bool FlashInterface::setSplash(QImage newsplash) 
{
  char b[SPLASHX9D_SIZE]= {0};
  uint qrgb, black;
  black=QColor(0,0,0).rgb();
  QByteArray splash;
  if (splash_offset==0) {
    return false;
  }
  else {
    for(uint y=0; y<splash_height; y++)
        for(uint x=0; x<splash_width; x++) {
          qrgb=newsplash.pixel(x,y);
          b[splash_width*(y/8) + x] |= ((qrgb==black) ? 1 : 0)<<(y % 8);
        }

    splash.clear();
    splash.append(b, splash_size);
    flash.replace(splash_offset,splash_size,splash);
    return true;
  }
}

uint FlashInterface::getSplashWidth()
{
  return splash_width;
}

uint FlashInterface::getSplashHeight()
{
  return splash_height;
}

QImage FlashInterface::getSplash()
{
  uint k=0;
  QImage image(splash_width, splash_height, QImage::Format_Mono);
  uchar b[SPLASHX9D_SIZE] = {0};
  if (splash_offset>0) {
    for (k=0; k<sizeof(b); k++)
      b[k]=flash.at(splash_offset+k);
    for(uint y=0; y<splash_height; y++)
      for(uint x=0; x<splash_width; x++)
        image.setPixel(x,y,((b[splash_width*(y/8) + x]) & (1<<(y % 8))) ? 0 : 1  );
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
