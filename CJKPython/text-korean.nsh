;
; CJKPython Text Translations - Korean
;
; created by Hye-Shik Chang <perky@i18n.org>
;
; $Id: text-korean.nsh,v 1.1 2003/09/23 11:13:47 perky Exp $
;

LicenseData /LANG=${LANG_KOREAN} "LICENSE.txt"

LangString TypeFull ${LANG_KOREAN} "전체 설치"
LangString TypeFullJapanese ${LANG_KOREAN} "전체 설치 (일본어 사용자)"
LangString TypeBasic ${LANG_KOREAN} "기본 설치"
LangString TypeBasicJapanese ${LANG_KOREAN} "기본 설치 (일본어 사용자)"
LangString TypeMinimum ${LANG_KOREAN} "최소 설치"

LangString SecPythonCore ${LANG_KOREAN} "파이썬 인터프리터와 라이브러리"
LangString SecPythonCAPI ${LANG_KOREAN} "파이썬-C API 개발자용 헤더와 라이브러리"
LangString SecPythonDocumentation ${LANG_KOREAN} "파이썬 HTML 문서"
LangString SecPythonTclTkGUI ${LANG_KOREAN} "파이썬 Tcl/Tk, tkinter와 IDLE"
LangString SecPythonUtilityScripts ${LANG_KOREAN} "유용한 파이썬 스크립트"
LangString SecPythonTestSuite ${LANG_KOREAN} "파이썬 테스트 슈트"
LangString SecSimplifiedChineseCodecs ${LANG_KOREAN} "중국어(간체) 지원"
LangString SecTraditionalChineseCodecs ${LANG_KOREAN} "중국어(번체) 지원" 
LangString SecJapaneseCodecs ${LANG_KOREAN} "일본어 지원"
LangString SecKoreanCodecs ${LANG_KOREAN} "한국어 지원"
LangString SecUTFCodecs ${LANG_KOREAN} "UTF-7, UTF-8 지원"
LangString SecLegacyLocaleSupportByDefault ${LANG_KOREAN} "기본으로 확장 완성형 로켈 지원"
LangString SecPythonSJISPatch ${LANG_KOREAN} "파이썬 SJIS 패치 (일본어 전용)"
LangString SecCtypesModule ${LANG_KOREAN} "ctypes 모듈"
LangString SecCompileByteCodes ${LANG_KOREAN} "바이트 코드로 컴파일"
LangString SecHangul ${LANG_KOREAN} "hangul 모듈"
LangString SecPyKf ${LANG_KOREAN} "pykf 모듈"

LangString SubSectionPython ${LANG_KOREAN} "파이썬"
LangString SubSectionCJKCodecs ${LANG_KOREAN} "한중일 유니코드 코덱"
LangString SubSectionLocalizationPatches ${LANG_KOREAN} "한중일 지역화 패치"
LangString SubSectionThirdPartyModules ${LANG_KOREAN} "비표준 확장 모듈"

LangString DESC_Python ${LANG_KOREAN} "표준 파이썬 배포 파일들을 설치합니다."
LangString DESC_CJKCodecs ${LANG_KOREAN} "한,중,일 인코딩들을 위한 유니코드 코덱을 설치합니다." 
LangString DESC_LocalizationPatches ${LANG_KOREAN} "한,중,일 로켈을 위한 비표준 지역화 패치들을 설치합니다."
LangString DESC_ThirdPartyModules ${LANG_KOREAN} "표준 파이썬에 포함되어 있지 않은 유용한 모듈들을 설치합니다."

LangString DESC_PythonCore ${LANG_KOREAN} "파이썬 인터프리터와 라이브러리를 설치합니다."
LangString DESC_PythonSJISPatch ${LANG_KOREAN} "아쯔오 이시모토의 Shift-JIS를 위한 파이썬 라이브러리 패치를 설치합니다."
LangString DESC_PythonCAPI ${LANG_KOREAN} "파이썬-C API 개발자들을 위한 헤더 파일과 라이브러리를 설치합니다."
LangString DESC_PythonDocumentation ${LANG_KOREAN} "파이썬 HTML 문서를 설치합니다."
LangString DESC_PythonTclTkGUI ${LANG_KOREAN} "파이썬을 위한 Tcl/Tk GUI 툴킷 tkinter와 IDLE을 설치합니다."
LangString DESC_PythonUtilityScripts ${LANG_KOREAN} "유용한 파이썬 스크립트들을 설치합니다."
LangString DESC_PythonTestSuite ${LANG_KOREAN} "파이썬 유닛 테스트 슈트를 설치합니다."
LangString DESC_CJKCodecs ${LANG_KOREAN} "파이썬 유니코드 지원에서 한,중,일 인코딩을 사용할 수 있도록 CJKCodecs를 설치합니다."
LangString DESC_CtypesModule ${LANG_KOREAN} "윈도우 DLL의 함수를 쉽게 호출할 수 있도록 해 주는 ctypes 모듈을 설치합니다."
LangString DESC_CompileByteCodes ${LANG_KOREAN} "빠른 실행을 위해 미리 파이썬 라이브러리 파일들을 컴파일 합니다."
LangString DESC_SimplifiedChineseCodecs ${LANG_KOREAN} "중국어(간체) 인코딩을 위한 코덱을 설치합니다."
LangString DESC_TraditionalChineseCodecs ${LANG_KOREAN} "중국어(번체) 인코딩을 위한 코덱을 설치합니다."
LangString DESC_JapaneseCodecs ${LANG_KOREAN} "일본어 인코딩을 위한 코덱을 설치합니다."
LangString DESC_KoreanCodecs ${LANG_KOREAN} "한국어 인코딩을 위한 코덱을 설치합니다." 
LangString DESC_UTFCodecs ${LANG_KOREAN} "UTF-7과 UTF-8을 위한 코덱을 설치합니다."
LangString DESC_LegacyLocaleSupportByDefault ${LANG_KOREAN} "확장 완성형 로켈 지원을 기본적으로 지원하도록 패치합니다."
LangString DESC_HangulModule ${LANG_KOREAN} "한글 분석을 위한 hangul 모듈을 설치합니다."
LangString DESC_PyKfModule ${LANG_KOREAN} "JIS 코드 변환을 위한 pykf 모듈을 설치합니다."

LangString MSG_Compiling ${LANG_KOREAN} "바이트 코드 컴파일 중..."
LangString MSG_OptimizedCompiling ${LANG_KOREAN} "최적화된 바이트 코드 컴파일 중..."

LangString LOG_Compiling ${LANG_KOREAN} "컴파일:"
LangString LOG_Listing ${LANG_KOREAN} "디렉토리:"
