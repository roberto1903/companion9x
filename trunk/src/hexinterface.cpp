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

#include "hexinterface.h"

HexInterface::HexInterface(QTextStream &stream):
  stream(stream)
{
}

int HexInterface::getValueFromLine(const QString &line, int pos, int len)
{
    bool ok;
    int hex = line.mid(pos,len).toInt(&ok, 16);
    return ok ? hex : -1;
}

int HexInterface::load(uint8_t *data)
{
  int result = 0;

  while (!stream.atEnd()) {
    QString line = stream.readLine();

    if(line.left(1)!=":") continue;

    int byteCount = getValueFromLine(line,1);
    int address = getValueFromLine(line,3,4);
    int recType = getValueFromLine(line,7);

    if(byteCount<0 || address<0 || recType<0)
      return 0;

    QByteArray ba;
    ba.clear();

    quint8 chkSum = 0;
    chkSum -= byteCount;
    chkSum -= recType;
    chkSum -= address & 0xFF;
    chkSum -= address >> 8;
    for(int i=0; i<byteCount; i++)
    {
        quint8 v = getValueFromLine(line,(i*2)+9) & 0xFF;
        chkSum -= v;
        ba.append(v);
    }

    quint8 retV = getValueFromLine(line,(byteCount*2)+9) & 0xFF;
    if(chkSum!=retV)
      return 0;

    if (recType == 0x00) { //data record - ba holds record
      memcpy(&data[address],ba.data(),byteCount);
      result = std::max(result, address+byteCount);
    }
  }
  
  return result;
}

bool HexInterface::save(uint8_t *data, const int size)
{
  for (int i=0; i<(size/32); i++) {
    QString str = QString(":20%1000").arg(i*32,4,16,QChar('0')); //write start, bytecount (32), address and record type
    quint8 chkSum = 0;
    chkSum = -32; //-bytecount; recordtype is zero
    chkSum -= (i*32) & 0xFF;
    chkSum -= (i*32) >> 8;
    for (int j=0; j<32; j++)
    {
        str += QString("%1").arg(data[i*32+j],2,16,QChar('0'));
        chkSum -= data[i*32+j];
    }

    str += QString("%1").arg(chkSum,2,16,QChar('0'));
    stream << str.toUpper() << "\n"; // output to file and lf;
  }

  stream << ":00000001FF";  // write EOF

  return true;
}


