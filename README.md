# ICUQtPOC - Integrating libICU for Unicode conversion and charset detection with a Qt app (POC)

This project is a proof-of-concept for using
[libICU](https://icu.unicode.org/home)
to convert text from any charset to [Unicode](https://home.unicode.org/),
integrated in a Qt app.

## Why this exists?

Qt has a convenient class:
[QTextCodec](https://doc.qt.io/qt-5/qtextcodec.html),
that performs conversion between Unicode and other character sets, until Qt5.
In Qt6 this class was moved from QtCore to the
[Qt5Compat module](https://doc.qt.io/qt-6/qtcore5compat-module.html),
which means that it will be relegated in maintenance
priority, and it may be totally removed in the future.

See: https://bugreports.qt.io/browse/QTBUG-86437

There are two use cases for the Qt6 Core5Compat module:

* To allow a quick and dirty port of some application to Qt6 without fully 
embracing the new API, perhaps for speed or pure laziness, but maybe to avoid 
sacrificing compatibility with Qt5.
* To keep functionality that has been removed from Qt6 core modules, but moved 
to Core5Compat. This is the case of the QTextCodec class, which supports many 
text encodings. This class is needed by Drumstick::File when decoding standard 
MIDI files. In fact, all MIDI applications that read standard MIDI files need 
to decode text metadata (credits, copyrights, lyrics, markers) from virtually 
any encoding available for the last 30 years. Keeping this functionality is 
critical for data preservation. The alternative to using Core5Compat in this 
case is using other libraries like libICU or libiconv, much worse to integrate 
in a Qt application.

Drumstick::File is a library, belonging to the
[Drumstick project](https://drumstick.sourceforge.io/), that supports
Input/Output from
[Standard MIDI Files](https://www.midi.org/specifications-old/item/standard-midi-files-smf)
(.MID), which may include text metadata, like
lyrics, comments, credits, copyrights, instrument names, markers and cue names. Some
programs based on Drumstick (like
[kmidimon](https://kmidimon.sourceforge.io/) and
[dmidiplayer](https://dmidiplayer.sourceforge.io/)
) display metadata to the users, so the text needs to be converted to Unicode.

## SMF metadata

[The SMF standard](https://www.midi.org/specifications-old/item/standard-midi-files-smf)
was created as a file interchange format, so users could export
compositions from one MIDI sequencer proprietary format, and import those files
into another sequencer. Some sequencer hardware machines and software became
deprecated and unusable with time, and the data files in proprietary format
became unreadable, losing work unless the users kept SMF exported files. Those
files should now be treated as original manuscripts for 
[digital preservation](https://www.loc.gov/preservation/digital/formats/fdd/fdd000119.shtml).

The authors of the standard, naively stated that all text metadata should be
encoded in ASCII within a MIDI file, omitting the fact that many cultures exist
around the world needing other encodings to reliable store their languages. Of
course, the result was that composers, manufacturers and software developers
all over the world ignored that standard requirement and used whatever encoding
that was available at the time. Nowadays, a good encoding choice is UTF-8, and
this is what [Rosegarden](https://www.rosegardenmusic.com/)
does when exports compositions as MIDI files. But for
importing from MIDI files, software apps need to convert from some unknown
encodings into Unicode for processing.

## Tested solution

This project does not read SMF files. Instead, it reads plain text files with
unknown encodings, and tries to detect the input charset using the
[ICU CharsetDetector](https://unicode-org.github.io/icu/userguide/conversion/detection.html#charsetdetector).
The user has a combobox to choose another charset if the detection is not 
correct. Then, the input data is converted to Unicode by
[ICU Converters](https://unicode-org.github.io/icu/userguide/conversion/converters.html#icu-converters)
and displayed to the user.

There is another POC, functionally equivalent:
[alternative using iconv()](https://github.com/pedrolcl/iconv_qt_poc).

## The ICU Nightmare in Qt

If you want to build this project using the Qt online installer libraries, you are 
going to experience problems. The Qt 6.3.0 libraries are compiled against icu 56,
but your system libs may be newer. For instance Fedora 35 comes with icu 69.

The problem is that the Qt online installer ships the icu development libraries
(the files with the .so suffix, without a number) but not the corresponding icu
headers. The result is that when linking the project you will get errors like:

    undefined reference to `ucnv_open_69'

That is because the compiler included the icu 69 headers installed in your 
system (at /usr/include), but the linker is trying to use the icu 56 libraries, 
installed by the online installer probably at your $HOME.

One solution is to use the pkg-config utility to find ICU, but another is to 
remove all the ICU development libraries from the Qt prefix. For instance:

    libicudata.so        <- remove this symlink!
    libicudata.so.56     <- leave this symlink alone
    libicudata.so.56.1   <- leave also the library file 

This will allow the build to succeed, but will generate the following warnings:

    /usr/bin/ld: warning: libicui18n.so.56, needed by libQt6Core.so.6.3.0, may conflict with libicui18n.so.69
    /usr/bin/ld: warning: libicuuc.so.56, needed by libQt6Core.so.6.3.0, may conflict with libicuuc.so.69

There is no danger of conflict between the two versions, because all the exported 
symbols of each library are named with the library version suffix. But there is
a problem creating a binary bundle containing your program and all the required
runtime libraries: you will need to ship both versions, which are not small.

See: https://bugreports.qt.io/browse/QTBUG-27930
