;
; CJKPython Text Translations - Traditional Chinese
;
; created by Hye-Shik Chang <perky@i18n.org>
;
; $Id: text-tradchinese.nsh,v 1.1 2003/09/23 11:13:47 perky Exp $
;

LicenseData /LANG=${LANG_TRADCHINESE} "LICENSE.txt"

LangString TypeFull ${LANG_TRADCHINESE} "Full Installation"
LangString TypeFullJapanese ${LANG_TRADCHINESE} "Full Installation (for Japanese Users)"
LangString TypeBasic ${LANG_TRADCHINESE} "Basic Installation"
LangString TypeBasicJapanese ${LANG_TRADCHINESE} "Basic Installation (for Japanese Users)"
LangString TypeMinimum ${LANG_TRADCHINESE} "Minimum Installation"

LangString SecPythonCore ${LANG_TRADCHINESE} "Python Interpreter and Libraries"
LangString SecPythonCAPI ${LANG_TRADCHINESE} "Python-C API files"
LangString SecPythonDocumentation ${LANG_TRADCHINESE} "Python Documentation"
LangString SecPythonTclTkGUI ${LANG_TRADCHINESE} "Python Tcl/Tk GUI (tkinter, IDLE)"
LangString SecPythonUtilityScripts ${LANG_TRADCHINESE} "Python utility scripts"
LangString SecPythonTestSuite ${LANG_TRADCHINESE} "Python test suite"
LangString SecSimplifiedChineseCodecs ${LANG_TRADCHINESE} "Simplified Chinese Support"
LangString SecTraditionalChineseCodecs ${LANG_TRADCHINESE} "Traditional Chinese Support"
LangString SecJapaneseCodecs ${LANG_TRADCHINESE} "Japanese Support"
LangString SecKoreanCodecs ${LANG_TRADCHINESE} "Korean Support"
LangString SecUTFCodecs ${LANG_TRADCHINESE} "UTF-7 and UTF-8 Support"
LangString SecLegacyLocaleSupportByDefault ${LANG_TRADCHINESE} "Legacy Locale Support by default"
LangString SecPythonSJISPatch ${LANG_TRADCHINESE} "SJIS patches for Python core libraries"
LangString SecCtypesModule ${LANG_TRADCHINESE} "ctypes module"
LangString SecCompileByteCodes ${LANG_TRADCHINESE} "Compile byte codes"
LangString SecHangul ${LANG_TRADCHINESE} "hangul module"
LangString SecPyKf ${LANG_TRADCHINESE} "pykf module"

LangString SubSectionPython ${LANG_TRADCHINESE} "Python"
LangString SubSectionCJKCodecs ${LANG_TRADCHINESE} "CJKCodecs"
LangString SubSectionLocalizationPatches ${LANG_TRADCHINESE} "CJK Localization Patches"
LangString SubSectionThirdPartyModules ${LANG_TRADCHINESE} "Third Party Modules"

LangString DESC_Python ${LANG_TRADCHINESE} "Install the Standard Python files."
LangString DESC_CJKCodecs ${LANG_TRADCHINESE} "Install Unicode codes for Chinese, Japanese and Korean encodings."
LangString DESC_LocalizationPatches ${LANG_TRADCHINESE} "Install non-standard CJK Localization patches."
LangString DESC_ThirdPartyModules ${LANG_TRADCHINESE} "Useful modules that aren't part of standard Python."

LangString DESC_PythonCore ${LANG_TRADCHINESE} "Install the Python interpreter and libraries."
LangString DESC_PythonSJISPatch ${LANG_TRADCHINESE} "Install Shift-JIS patches for Python libraries. (by Atsuo Ishimoto)"
LangString DESC_PythonCAPI ${LANG_TRADCHINESE} "Install Python headers and libraries for C-API Developers."
LangString DESC_PythonDocumentation ${LANG_TRADCHINESE} "Install Python HTML Documentation."
LangString DESC_PythonTclTkGUI ${LANG_TRADCHINESE} "Install Tcl/Tk, tkinter and IDLE."
LangString DESC_PythonUtilityScripts ${LANG_TRADCHINESE} "Install Python utility scripts."
LangString DESC_PythonTestSuite ${LANG_TRADCHINESE} "Install Python unittest Suite."
LangString DESC_CJKCodecs ${LANG_TRADCHINESE} "Install CJKCodecs to enable unicode support for Chinese, Japanese and Korean encodings."
LangString DESC_CtypesModule ${LANG_TRADCHINESE} "Install ctypes module that offers easy function call interfaces to native DLLs."
LangString DESC_CompileByteCodes ${LANG_TRADCHINESE} "Compile python library files into byte codes"
LangString DESC_SimplifiedChineseCodecs ${LANG_TRADCHINESE} "Install unicode codecs for simplified chinese encodings."
LangString DESC_TraditionalChineseCodecs ${LANG_TRADCHINESE} "Install unicode codecs for traditional chinese encodings."
LangString DESC_JapaneseCodecs ${LANG_TRADCHINESE} "Install unicode codecs for japanese encodings."
LangString DESC_KoreanCodecs ${LANG_TRADCHINESE} "Install unicode codecs for korean encodings."
LangString DESC_UTFCodecs ${LANG_TRADCHINESE} "Install unicode codecs for UTF-7 and UTF-8."
LangString DESC_LegacyLocaleSupportByDefault ${LANG_TRADCHINESE} "Turn legacy locale support by default."
LangString DESC_HangulModule ${LANG_TRADCHINESE} "Install hangul, a module that manipulates Korean alphabets."
LangString DESC_PyKfModule ${LANG_TRADCHINESE} "Install pykf, a module that converts JIS codes."

LangString MSG_Compiling ${LANG_TRADCHINESE} "Generating Byte Codes..."
LangString MSG_OptimizedCompiling ${LANG_TRADCHINESE} "Generating Optimized Byte Codes..."

LangString LOG_Compiling ${LANG_TRADCHINESE} "Compiling"
LangString LOG_Listing ${LANG_TRADCHINESE} "Listing"
