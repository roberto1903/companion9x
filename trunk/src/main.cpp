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

#include <QApplication>
#include <QTranslator>
#include <QLocale>
#include <QString>
#include <QDir>
#include <QFileInfo>
#include <QSettings>
#include <QSplashScreen>
#include <QThread>
#include <iostream>
#include "mainwindow.h"
#include "eeprominterface.h"

#if defined WIN32 || !defined __GNUC__
#include <windows.h>
#define sleep(x) Sleep(x*1000)
#endif

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(companion9x);
    QApplication app(argc, argv);
    QString dir;
    if(argc) dir = QFileInfo(argv[0]).canonicalPath() + "/lang";

    QSettings settings("companion9x", "companion9x");
    QString locale = settings.value("locale",QLocale::system().name()).toString();
    bool showSplash = settings.value("show_splash", true).toBool();

    QTranslator companion9xTranslator;
    companion9xTranslator.load(":/companion9x_" + locale);
    QTranslator qtTranslator;
    qtTranslator.load((QString)QT_TRANSLATIONS_DIR + "/qt_" + locale);
    app.installTranslator(&companion9xTranslator);
    app.installTranslator(&qtTranslator);
    
    QPixmap pixmap(":/images/splash.png");
    QSplashScreen *splash = new QSplashScreen(pixmap);

    RegisterFirmwares();

    QVariant firmware_id = settings.value("firmware", default_firmware->id);
    FirmwareInfo *firmware = GetFirmware(firmware_id.toString());
    current_firmware = firmware ? firmware : default_firmware;

    if(showSplash)
    {
        splash->show();
        splash->showMessage(QObject::tr(""));
        sleep(SPLASH_TIME);
    }
    MainWindow mainWin;
    mainWin.show();
    splash->finish(&mainWin);
    return app.exec();
}