;
; CJKPython Text Translations - Japanese
;
; created by Hye-Shik Chang <perky@i18n.org>
;
; $Id: text-japanese.nsh,v 1.1 2003/09/23 11:13:47 perky Exp $
;

LicenseData /LANG=${LANG_JAPANESE} "LICENSE.txt"

LangString TypeFull ${LANG_JAPANESE} "Full Installation"
LangString TypeFullJapanese ${LANG_JAPANESE} "Full Installation (for Japanese Users)"
LangString TypeBasic ${LANG_JAPANESE} "Basic Installation"
LangString TypeBasicJapanese ${LANG_JAPANESE} "Basic Installation (for Japanese Users)"
LangString TypeMinimum ${LANG_JAPANESE} "Minimum Installation"

LangString SecPythonCore ${LANG_JAPANESE} "Python Interpreter and Libraries"
LangString SecPythonCAPI ${LANG_JAPANESE} "Python-C API files"
LangString SecPythonDocumentation ${LANG_JAPANESE} "Python Documentation"
LangString SecPythonTclTkGUI ${LANG_JAPANESE} "Python Tcl/Tk GUI (tkinter, IDLE)"
LangString SecPythonUtilityScripts ${LANG_JAPANESE} "Python utility scripts"
LangString SecPythonTestSuite ${LANG_JAPANESE} "Python test suite"
LangString SecSimplifiedChineseCodecs ${LANG_JAPANESE} "Simplified Chinese Support"
LangString SecTraditionalChineseCodecs ${LANG_JAPANESE} "Traditional Chinese Support"
LangString SecJapaneseCodecs ${LANG_JAPANESE} "Japanese Support"
LangString SecKoreanCodecs ${LANG_JAPANESE} "Korean Support"
LangString SecUTFCodecs ${LANG_JAPANESE} "UTF-7 and UTF-8 Support"
LangString SecLegacyLocaleSupportByDefault ${LANG_JAPANESE} "Legacy Locale Support by default"
LangString SecPythonSJISPatch ${LANG_JAPANESE} "SJIS patches for Python core libraries"
LangString SecCtypesModule ${LANG_JAPANESE} "ctypes module"
LangString SecCompileByteCodes ${LANG_JAPANESE} "Compile byte codes"
LangString SecHangul ${LANG_JAPANESE} "hangul module"
LangString SecPyKf ${LANG_JAPANESE} "pykf module"

LangString SubSectionPython ${LANG_JAPANESE} "Python"
LangString SubSectionCJKCodecs ${LANG_JAPANESE} "CJKCodecs"
LangString SubSectionLocalizationPatches ${LANG_JAPANESE} "CJK Localization Patches"
LangString SubSectionThirdPartyModules ${LANG_JAPANESE} "Third Party Modules"

LangString DESC_Python ${LANG_JAPANESE} "Install the Standard Python files."
LangString DESC_CJKCodecs ${LANG_JAPANESE} "Install Unicode codes for Chinese, Japanese and Korean encodings."
LangString DESC_LocalizationPatches ${LANG_JAPANESE} "Install non-standard CJK Localization patches."
LangString DESC_ThirdPartyModules ${LANG_JAPANESE} "Useful modules that aren't part of standard Python."

LangString DESC_PythonCore ${LANG_JAPANESE} "Install the Python interpreter and libraries."
LangString DESC_PythonSJISPatch ${LANG_JAPANESE} "Install Shift-JIS patches for Python libraries. (by Atsuo Ishimoto)"
LangString DESC_PythonCAPI ${LANG_JAPANESE} "Install Python headers and libraries for C-API Developers."
LangString DESC_PythonDocumentation ${LANG_JAPANESE} "Install Python HTML Documentation."
LangString DESC_PythonTclTkGUI ${LANG_JAPANESE} "Install Tcl/Tk, tkinter and IDLE."
LangString DESC_PythonUtilityScripts ${LANG_JAPANESE} "Install Python utility scripts."
LangString DESC_PythonTestSuite ${LANG_JAPANESE} "Install Python unittest Suite."
LangString DESC_CJKCodecs ${LANG_JAPANESE} "Install CJKCodecs to enable unicode support for Chinese, Japanese and Korean encodings."
LangString DESC_CtypesModule ${LANG_JAPANESE} "Install ctypes module that offers easy function call interfaces to native DLLs."
LangString DESC_CompileByteCodes ${LANG_JAPANESE} "Compile python library files into byte codes"
LangString DESC_SimplifiedChineseCodecs ${LANG_JAPANESE} "Install unicode codecs for simplified chinese encodings."
LangString DESC_TraditionalChineseCodecs ${LANG_JAPANESE} "Install unicode codecs for traditional chinese encodings."
LangString DESC_JapaneseCodecs ${LANG_JAPANESE} "Install unicode codecs for japanese encodings."
LangString DESC_KoreanCodecs ${LANG_JAPANESE} "Install unicode codecs for korean encodings."
LangString DESC_UTFCodecs ${LANG_JAPANESE} "Install unicode codecs for UTF-7 and UTF-8."
LangString DESC_LegacyLocaleSupportByDefault ${LANG_JAPANESE} "Turn legacy locale support by default."
LangString DESC_HangulModule ${LANG_JAPANESE} "Install hangul, a module that manipulates Korean alphabets."
LangString DESC_PyKfModule ${LANG_JAPANESE} "Install pykf, a module that converts JIS codes."

LangString MSG_Compiling ${LANG_JAPANESE} "Generating Byte Codes..."
LangString MSG_OptimizedCompiling ${LANG_JAPANESE} "Generating Optimized Byte Codes..."

LangString LOG_Compiling ${LANG_JAPANESE} "Compiling"
LangString LOG_Listing ${LANG_JAPANESE} "Listing"
