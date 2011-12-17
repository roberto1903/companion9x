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
#define MAX_FSIZE 300000

class FlashInterface {
    public:
        FlashInterface(QString filename);
        QString getVers();
        QString getDate();
        QString getTime();
        QString getSvn();
        QString getBuild();

    private:
        uint8_t flash[MAX_FSIZE];
        int flash_size;
        void SeekVer();
        void SeekSvn();
        void SeekDate();
        void SeekTime();
        void SeekBuild();
        QString version;
        QString date;
        QString time;
        QString svn;
        QString build;
        
    protected:
        bool isValid;
};


#endif	/* FLASHINTERFACE_H */

