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

#include <QDebug>
#include "icuconverter.h"

ICUConverter::ICUConverter(const QByteArray &name):
    m_name(name), m_conv(nullptr), m_error(U_ZERO_ERROR)
{
    qDebug() << Q_FUNC_INFO << name;
    m_conv = ucnv_open(m_name.constData(), &m_error);
    if (!U_SUCCESS(m_error)) {
        qWarning("ucnv_open() failed: %s", u_errorName(m_error));
    }
}

ICUConverter::~ICUConverter()
{
    qDebug() << Q_FUNC_INFO << m_name;
    ucnv_close(m_conv);
}

QString ICUConverter::convertToUnicode(const char *bytes)
{
    return convertToUnicode(QByteArray(bytes));
}

QString ICUConverter::convertToUnicode(const QByteArray &bytes)
{
    QString string(bytes.length() + 2, Qt::Uninitialized);

    const char *chars = bytes.constData();
    const char *end = chars + bytes.length();
    int convertedChars = 0;
    while (1) {
        UChar *uc = (UChar *)string.data();
        UChar *ucEnd = uc + string.length();
        uc += convertedChars;
        UErrorCode error = U_ZERO_ERROR;
        ucnv_toUnicode(m_conv,
                       &uc, ucEnd,
                       &chars, end,
                       nullptr, false, &error);
        if (!U_SUCCESS(error) && error != U_BUFFER_OVERFLOW_ERROR) {
            qDebug("convertToUnicode failed: %s", u_errorName(error));
            break;
        }

        convertedChars = uc - (UChar *)string.data();
        if (chars >= end)
            break;
        string.resize(string.length()*2);
    }
    string.resize(convertedChars);
    return string;
}

QList<QByteArray> ICUConverter::charsetNames()
{
    int max = ucnv_countAvailable();
    QList<QByteArray> result;
    for(int i=0; i<max; ++i) {
        const char *name = ucnv_getAvailableName(i);

        UErrorCode error = U_ZERO_ERROR;
        const char *standardName = ucnv_getStandardName(name, "MIME", &error);
        if (U_FAILURE(error) || !standardName) {
            error = U_ZERO_ERROR;
            standardName = ucnv_getStandardName(name, "IANA", &error);
        }
        if (U_FAILURE(error))
            continue;

        if (standardName && !result.contains(standardName)) {
            result += standardName;
        }
    }
    return result;
}
