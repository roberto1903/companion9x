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
#define MAX_FSIZE 256*1024
#define SPLASH_WIDTH (128)
#define SPLASH_HEIGHT (64)
#define SPLASH_SIZE (SPLASH_WIDTH*SPLASH_HEIGHT/8)
#define VERS_MARK "VERS:"
#define SVN_MARK "SVN:"
#define DATE_MARK "DATE:"
#define TIME_MARK "TIME:"
#define BLD_MARK "BLD:"
#define MOD_MARK "MOD:"

class FlashInterface
{
public:
  FlashInterface(QString filename);
  QString getVers();
  QString getDate();
  QString getTime();
  QString getSvn();
  QString getBuild();
  QImage getSplash();
  bool setSplash(QImage newsplash);
  bool hasSplash();
  uint saveFlash(QString fileName);

private:
  QByteArray flash;
  void SeekVer();
  void SeekSvn();
  void SeekDate();
  void SeekTime();
  void SeekBuild();
  void SeekSplash();
  QString filename;
  QString version;
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
  bool isValid;
};
#endif	/* FLASHINTERFACE_H */

