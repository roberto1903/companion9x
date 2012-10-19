/*
 * Author - Romolo Manfredini <romolo.manfredini@gmail.com>
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

#ifndef FLASHINTERFACE_H
#define FLASHINTERFACE_H
#include <QDialog>
#include <QtGui>
#include <inttypes.h>
#include "file.h"
#define MAX_FSIZE (256*1024)
#define SPLASH_WIDTH (128)
#define SPLASH_HEIGHT (64)
#define SPLASH_SIZE (SPLASH_WIDTH*SPLASH_HEIGHT/8)
#define ERSPLASH_MARKER "Splash"
#define ERSPLASH_OFFSET (10)
#define O9X_SPS "SPS"
#define O9X_SPE "SPE"
#define O9X_OFFSET (6)
#define ERSKY9X_SPS "SPS"
#define ERSKY9X_SPE "SPE"
#define ERSKY9X_OFFSET (7)
#define VERS_MARK "VERS:"
#define SVN_MARK "SVN:"
#define DATE_MARK "DATE:"
#define TIME_MARK "TIME:"
#define BLD_MARK "BLD:"
#define VAR_MARK "EEPR:"

#define FILE_TYPE_BIN  1
#define FILE_TYPE_HEX  2
#define FILE_TYPE_EEPE 3
#define FILE_TYPE_EEPM 4
#define FILE_TYPE_XML  5

int getFileType(const QString &fullFileName);

class FlashInterface
{
public:
  FlashInterface(QString filename);
  QString getDate();
  QString getTime();
  QString getSvn();
  int getSize();
  QString getBuild();
  QImage getSplash();
  bool setSplash(QImage newsplash);
  bool hasSplash();
  uint saveFlash(QString fileName);
  bool isValid();

private:
  QByteArray flash;
  void SeekVer();
  void SeekSvn();
  void SeekDate();
  void SeekTime();
  void SeekBuild();
  void SeekSplash();
  QString filename;
  QString date;
  QString time;
  QString svn;
  QString build;
  QByteArray splash;
  uint splash_offset;
  uint splash_type;
  uint splash_size;
  uint flash_size;

protected:
  bool isValidFlag;
};
#endif	/* FLASHINTERFACE_H */

