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
#include "flashinterface.h"

FlashInterface::FlashInterface(QString fileName) {
        version="";
        date="";
        time="";
        svn="";
        build="";
        
        QFile file(fileName);

        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {  //reading HEX TEXT file
            isValid=-1;
            return ;
        }
        QTextStream inputStream(&file);
        flash_size = HexInterface(inputStream).load(flash, MAX_FSIZE);
        file.close();
        inputStream.reset();
        if (flash_size==0) {
            QFile file(fileName);
            file.open(QIODevice::ReadOnly);
            char bin_flash[MAX_FSIZE];
            flash_size=file.read(bin_flash,MAX_FSIZE);
            for (int i=0; i<flash_size;i++)
                flash[i]=(uint8_t)bin_flash[i];
        }
        if (flash_size>0) {
            SeekVer();
            SeekSvn();
            SeekDate();
            SeekTime();
            SeekBuild();
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
    int i, start=-1, end=-1;
    char tmp[20];
    for( int i=0; i<flash_size-5; i++) {
        if (flash[i]=='V' && flash[i+1]=='E' && flash[i+2]=='R' && flash[i+3]=='S' && flash[i+4]==':') {
            start=i;
            break;
        }
    }
    if (start>0 ) {
        for (i=start; i<(start+20); i++) {
            if (flash[i]==0) {
                end=i;
                break;
            }
        }
        if (end>0) {
            for (i=(start+5); i<(end+1);i++) {
                tmp[i-(start+5)]=flash[i];
            }
            version=QString(tmp).trimmed();
        } else {
            version=QString("");
        }
    }
}

void FlashInterface::SeekSvn(void) {
    int i, start=-1, end=-1;
    char tmp[20];
    for( int i=0; i<flash_size-4; i++) {
        if (flash[i]=='S' && flash[i+1]=='V' && flash[i+2]=='N' && flash[i+3]==':' ) {
            start=i;
            break;
        }
    }
    if (start>0 ) {
        for (i=start; i<(start+20); i++) {
            if (flash[i]==0) {
                end=i;
                break;
            }
        }
        if (end>0) {
            for (i=(start+4); i<(end+1);i++) {
                tmp[i-(start+3)]=flash[i];
            }
            svn=QString(tmp).trimmed();
        } else {
            svn=QString("");
        }
    }
}

void FlashInterface::SeekDate(void) {
    int i, start=-1, end=-1;
    char tmp[20];
    for( int i=0; i<flash_size-5; i++) {
        if (flash[i]=='D' && flash[i+1]=='A' && flash[i+2]=='T' && flash[i+3]=='E' && flash[i+4]==':') {
            start=i;
            break;
        }
    }
    if (start>0 ) {
        for (i=start; i<(start+20); i++) {
            if (flash[i]==0) {
                end=i;
                break;
            }
        }
        if (end>0) {
            for (i=(start+5); i<(end+1);i++) {
                tmp[i-(start+5)]=flash[i];
            }
            date=QString(tmp).trimmed();
        } else {
            date=QString("");
        }
    }
}

void FlashInterface::SeekTime(void) {
    int i, start=-1, end=-1;
    char tmp[20];
    for( int i=0; i<flash_size-5; i++) {
        if (flash[i]=='T' && flash[i+1]=='I' && flash[i+2]=='M' && flash[i+3]=='E' && flash[i+4]==':') {
            start=i;
            break;
        }
    }
    if (start>0 ) {
        for (i=start; i<(start+20); i++) {
            if (flash[i]==0) {
                end=i;
                break;
            }
        }
        if (end>0) {
            for (i=(start+5); i<(end+1);i++) {
                tmp[i-(start+5)]=flash[i];
            }
            time=QString(tmp).trimmed();
        } else {
            time=QString("");
        }
    }
}

void FlashInterface::SeekBuild(void) {
    int i;
    int start=-1;
    int end=-1;
    char tmp[20]; 
    for( int i=0; i<flash_size-5; i++) {
        if (flash[i]=='B' && flash[i+1]=='L' && flash[i+2]=='D' && flash[i+3]==':' ) {
            start=i;
            break;
        }
    }
    if (start>0 ) {
        for (i=start; i<(start+20); i++) {
            if (flash[i]==0) {
                end=i;
                break;
            }
        }
        if (end>0) {
            for (i=(start+5); i<(end+1);i++) {
                tmp[i-(start+5)]=flash[i];
            }
            build=QString(tmp).trimmed();
        } else {
            build=QString("");
        }
    } else {
        for( i=0; i<flash_size-5; i++) {
            if (flash[i]=='M' && flash[i+1]=='O' && flash[i+2]=='D' && flash[i+3]==':' ) {
                start=i;
                break;
            }
        }
        if (start>0 ) {
            for (i=start; i<(start+20); i++) {
                if (flash[i]==0) {
                    end=i;
                    break;
                }
            }
            if (end>0) {
                for (i=(start+5); i<(end+1);i++) {
                    tmp[i-(start+5)]=flash[i];
                }
                build=QString(tmp).trimmed();
            } else {
                build=QString("");
            }
        }
    }
}
