#
#    Copyright 2013-2014 Kai Pastor
#    
#    This file is part of OpenOrienteering.
# 
#    OpenOrienteering is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
# 
#    OpenOrienteering is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
# 
#    You should have received a copy of the GNU General Public License
#    along with OpenOrienteering.  If not, see <http://www.gnu.org/licenses/>.

TEMPLATE = lib
TARGET   = ocd
CONFIG  += staticlib

QMAKE_CFLAGS += -std=c99 -Wno-sign-compare -Wno-type-limits -Wno-unused-parameter -Wno-format -Wno-pointer-to-int-cast -Wno-int-to-pointer-cast

HEADERS = \
  libocad.h \
  array.h \
  geometry.h \
  types.h

SOURCES = \
  types.c \
  array.c \
  geometry.c \
  path.c \
  file.c \
  color.c \
  setup.c \
  ocad_symbol.c \
  ocad_object.c \
  string.c

LIBOCD_PRI = \
  "$$LITERAL_HASH Generated by $$_PRO_FILE_" \
  "DEPENDPATH  += $$PWD" \
  "INCLUDEPATH += $$PWD" \
  "LIBS        += \"-L$$OUT_PWD\""

write_file($$OUT_PWD/libocd.pri, LIBOCD_PRI)