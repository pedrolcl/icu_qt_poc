/*
    ICUQtPOC - Integrating libICU for Unicode Conversion in Qt apps
    Copyright (C) 2022 Pedro Lopez-Cabanillas
    
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    
    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef ICUCONVERTER_H
#define ICUCONVERTER_H

#include <QList>
#include <QByteArray>
#include <unicode/ucnv.h>

class ICUConverter
{
public:
    ICUConverter(const QByteArray& name);
   ~ICUConverter();
   
    QString convertToUnicode(const char* bytes);
    QString convertToUnicode(const QByteArray& bytes);
    
    static QList<QByteArray> charsetNames();
    
private:
    QByteArray m_name;    
    UConverter *m_conv;
    UErrorCode m_error;
};

#endif // ICUCONVERTER_H
