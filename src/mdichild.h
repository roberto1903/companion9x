/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef MDICHILD_H
#define MDICHILD_H

#define FILE_TYPE_BIN  1
#define FILE_TYPE_HEX  2
#define FILE_TYPE_EEPE 3
#define FILE_TYPE_EEPM 4
#define FILE_TYPE_XML  5

#include <QtGui>
#include "eeprominterface.h"

namespace Ui {
class mdiChild;
}

#define EEPE_EEPROM_FILE_HEADER  "EEPE EEPROM FILE"
#define EEPE_MODEL_FILE_HEADER   "EEPE MODEL FILE"

#define XML_FILES_FILTER     "XML files (*.xml);;"
#define HEX_FILES_FILTER     "HEX files (*.hex);;"
#define BIN_FILES_FILTER     "BIN files (*.bin);;"
#define EEPE_FILES_FILTER    "EEPE EEPROM files (*.eepe);;"
#define EEPM_FILES_FILTER    "EEPE MODEL files (*.eepm);;"
// #define EEPROM_FILES_FILTER  "EEPE files (*.xml *.eepe *.eepm *.bin *.hex);;" XML_FILES_FILTER EEPE_FILES_FILTER EEPM_FILES_FILTER BIN_FILES_FILTER HEX_FILES_FILTER
#define EEPROM_FILES_FILTER  "EEPE files (*.eepe *.bin *.hex);;" EEPE_FILES_FILTER BIN_FILES_FILTER HEX_FILES_FILTER
#define FLASH_FILES_FILTER   "FLASH files (*.bin *.hex);;" BIN_FILES_FILTER HEX_FILES_FILTER
#define EXTERNAL_EEPROM_FILES_FILTER   "EEPROM files (*.bin *.hex);;" BIN_FILES_FILTER HEX_FILES_FILTER

class MdiChild : public QWidget
{
    friend class ModelsListWidget;

    Q_OBJECT

public:
    MdiChild();

    void newFile();
    bool loadFile(const QString &fileName, bool resetCurrentFile=true);
    bool save();
    bool saveAs();
    bool saveFile(const QString &fileName, bool setCurrent=true);
    bool hasSelection();
    QString userFriendlyCurrentFile();
    QString currentFile() { return curFile; }
    // void keyPressEvent(QKeyEvent *event);
    bool hasPasteData();
    static int getFileType(const QString &fullFileName);
    void viableModelSelected(bool viable);
    void eepromInterfaceChanged();
    void setEEpromAvail(int eavail);

signals:
    void copyAvailable(bool val);

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void documentWasModified();
    void on_SimulateTxButton_clicked();

public slots:
    void OpenEditWindow();

    void cut();
    void copy();
    void paste();
    void burnTo();
    void simulate();
    void print();
    void setModified();

private:
    bool maybeSave();
    void setCurrentFile(const QString &fileName);
    QString strippedName(const QString &fullFileName);
    void saveSelection();
    void restoreSelection();

    Ui::mdiChild *ui;

    QString curFile;
    RadioData radioData;

    bool isUntitled;
    bool fileChanged;
    int EEPromAvail;
};

#endif
