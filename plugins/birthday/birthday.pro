# This file is part of Contacts daemon
#
# Copyright (c) 2010-2011 Nokia Corporation and/or its subsidiary(-ies).
#
# Contact:  Nokia Corporation (info@qt.nokia.com)
#
# GNU Lesser General Public License Usage
# This file may be used under the terms of the GNU Lesser General Public License
# version 2.1 as published by the Free Software Foundation and appearing in the
# file LICENSE.LGPL included in the packaging of this file.  Please review the
# following information to ensure the GNU Lesser General Public License version
# 2.1 requirements will be met:
# http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
#
# In addition, as a special exception, Nokia gives you certain additional rights.
# These rights are described in the Nokia Qt LGPL Exception version 1.1, included
# in the file LGPL_EXCEPTION.txt in this package.
#
# Other Usage
# Alternatively, this file may be used in accordance with the terms and
# conditions contained in a signed written agreement between you and Nokia.

TEMPLATE = lib
QT -= gui

# Hack: mkcal adds /usr/include/meegotouch to include path, and alphabetic CONFIG
# always puts that before mlocale, resulting in link errors. Force mlocale to be
# first.
INCLUDEPATH += /usr/include/mlocale

CONFIG += plugin qtsparql qtsparql-tracker-extensions mlocale mkcal mobility
MOBILITY += contacts

CONFIG(coverage):{
QMAKE_CXXFLAGS += -c -g  --coverage -ftest-coverage -fprofile-arcs
LIBS += -lgcov
}

DEFINES += QT_NO_CAST_TO_ASCII QT_NO_CAST_FROM_ASCII

INCLUDEPATH += $$TOP_SOURCEDIR/src
DEFINES += ENABLE_DEBUG

HEADERS  = cdbirthdaycalendar.h \
    cdbirthdaycontroller.h \
    cdbirthdayplugin.h

SOURCES  = cdbirthdaycalendar.cpp \
    cdbirthdaycontroller.cpp \
    cdbirthdayplugin.cpp

TARGET = birthdayplugin
target.path = $$LIBDIR/contactsd-1.0/plugins

INSTALLS += target
