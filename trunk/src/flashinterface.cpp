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

FlashInterface::FlashInterface(QString fileName) {
  uint8_t temp[MAX_FSIZE];
  version = "";
  date = "";
  time = "";
  svn = "";
  build = "";

  QFile file(fileName);

  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) { //reading HEX TEXT file
    isValid = false;
  }
  else {
    QTextStream inputStream(&file);
    int flash_size = HexInterface(inputStream).load(temp, MAX_FSIZE);
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
    }
  }
}

QString FlashInterface::getVers(void) {
  return version;
}

QString FlashInterface::getDate(void) {
  return date;
}

QString FlashInterface::getTime(void) {
  return time;
}

QString FlashInterface::getSvn(void) {
  return svn;
}

QString FlashInterface::getBuild(void) {
  return build;
}

void FlashInterface::SeekVer(void) {
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
  }
}

void FlashInterface::SeekSvn(void) {
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

void FlashInterface::SeekDate(void) {
  int i, start = -1, end = -1;
  start = flash.indexOf(QString(DATE_MARK));
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

void FlashInterface::SeekTime(void) {
  int i, start = -1, end = -1;
  start = flash.indexOf(QString(TIME_MARK));
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

void FlashInterface::SeekBuild(void) {
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

void FlashInterface::SeekSplash(void) {
  QByteArray splash;
  splash_offset=0;
  splash.clear();
  splash.append(gr9x_splash, sizeof(gr9x_splash));
  int start = flash.indexOf(splash);
  if (start>0) {
    splash_offset=start;
  } 
  if (start==-1) {
    splash.clear();
    splash.append(er9x_splash, sizeof(er9x_splash));
    start = flash.indexOf(splash);
    if (start>0) {
      splash_offset=start;
    }
  }
  if (start==-1) {
    splash.clear();
    splash.append(gr9xv4_splash, sizeof(gr9xv4_splash));
    start = flash.indexOf(splash);
    if (start>0) {
      splash_offset=start;
    }
  }
}
