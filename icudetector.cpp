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
#include <unicode/utypes.h>
#include <unicode/ucsdet.h>
#include "icudetector.h"

void ICUDetector::clear()
{
    //qDebug() << Q_FUNC_INFO;                
    m_buffer.clear();
}

void ICUDetector::addBytes(const char *bytes)
{
    //qDebug() << Q_FUNC_INFO << bytes;
    m_buffer += bytes;
}

void ICUDetector::addBytes(const QByteArray &bytes)
{
    //qDebug() << Q_FUNC_INFO << bytes;
    m_buffer += bytes;
}

QByteArray ICUDetector::detect(const QByteArray &bytes)
{
    //qDebug() << Q_FUNC_INFO << bytes;
    clear();
    addBytes(bytes);
    return detect();
}

QByteArray ICUDetector::detect()
{
    qDebug() << Q_FUNC_INFO;
    int32_t inputLength, match, matchCount = 0;
    UCharsetDetector* csd;
    const UCharsetMatch **csm;
    UErrorCode status = U_ZERO_ERROR;
    
    inputLength = (int32_t) m_buffer.length();

    csd = ucsdet_open(&status);
    ucsdet_setText(csd, m_buffer.constData(), inputLength, &status);
    csm = ucsdet_detectAll(csd, &matchCount, &status);

    QByteArray baName;
    int32_t max_confidence = 0;

    for(match = 0; match < matchCount; match += 1) {
        const char *name = ucsdet_getName(csm[match], &status);
        const char *lang = ucsdet_getLanguage(csm[match], &status);
        int32_t confidence = ucsdet_getConfidence(csm[match], &status);

        if (lang == NULL || strlen(lang) == 0) {
            lang = "**";
        }
        
        qDebug("%s (%s) %d", name, lang, confidence);
        
        if ((confidence > 10) && (confidence > max_confidence)) {
            max_confidence = confidence;
            baName = name;
        }
    }

    ucsdet_close(csd);
    return baName;
}

/*
 ********************************************************************************
 *   Copyright (C) 2005-2006, International Business Machines
 *   Corporation and others.  All Rights Reserved.
 ********************************************************************************

#include "unicode/utypes.h"
#include "unicode/ucsdet.h"

#include <string.h>
#include <stdio.h>

#define BUFFER_SIZE 8192

int main(int argc, char *argv[])
{
    static char buffer[BUFFER_SIZE];
    int32_t arg;

    if( argc <= 1 ) {
        printf("Usage: %s [filename]...\n", argv[0]);
        return -1;
    }

    for(arg = 1; arg < argc; arg += 1) {
        FILE *file;
        char *filename = argv[arg];
        int32_t inputLength, match, matchCount = 0;
        UCharsetDetector* csd;
        const UCharsetMatch **csm;
        UErrorCode status = U_ZERO_ERROR;

        if (arg > 1) {
            printf("\n");
        }

        file = fopen(filename, "rb");

        if (file == NULL) {
            printf("Cannot open file \"%s\"\n\n", filename);
            continue;
        }

        printf("%s:\n", filename);

        inputLength = (int32_t) fread(buffer, 1, BUFFER_SIZE, file);

        fclose(file);

        csd = ucsdet_open(&status);
        ucsdet_setText(csd, buffer, inputLength, &status);

        csm = ucsdet_detectAll(csd, &matchCount, &status);

        for(match = 0; match < matchCount; match += 1) {
            const char *name = ucsdet_getName(csm[match], &status);
            const char *lang = ucsdet_getLanguage(csm[match], &status);
            int32_t confidence = ucsdet_getConfidence(csm[match], &status);

            if (lang == NULL || strlen(lang) == 0) {
                lang = "**";
            }

            printf("%s (%s) %d\n", name, lang, confidence);
        }

        ucsdet_close(csd);
    }
    return 0;
}
*/
