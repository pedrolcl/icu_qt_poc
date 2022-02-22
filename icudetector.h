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

#ifndef ICUDETECTOR_H
#define ICUDETECTOR_H

#include <QByteArray>

class ICUDetector
{
public:
    void clear();
    void addBytes(const char* bytes);
    void addBytes(const QByteArray& bytes);
    QByteArray detect(const QByteArray& bytes);
    QByteArray detect();

private:
    QByteArray m_buffer;
};

#endif // ICUDETECTOR_H
