--- Lib/encodings/aliases.py.orig	2003-07-27 05:53:24.000000000 +0900
+++ Lib/encodings/aliases.py	2003-07-27 05:56:36.000000000 +0900
@@ -338,4 +338,96 @@
     'zip'                : 'zlib_codec',
     'zlib'               : 'zlib_codec',
 
+    # big5 codec
+    'csbig5'             : 'big5',
+
+    # cp932 codec
+    '932'                : 'cp932',
+    'ms932'              : 'cp932',
+    'mskanji'            : 'cp932',
+    'ms_kanji'           : 'cp932',
+
+    # cp949 codec
+    '949'                : 'cp949',
+    'ms949'              : 'cp949',
+    'uhc'                : 'cp949',
+
+    # cp950 codec
+    '950'                : 'cp950',
+    'ms950'              : 'cp950',
+
+    # euc_jisx0213 codec
+    'jisx0213'           : 'euc_jisx0213',
+    'eucjisx0213'        : 'euc_jisx0213',
+
+    # euc_jp codec
+    'eucjp'              : 'euc_jp',
+    'ujis'               : 'euc_jp',
+    'u_jis'              : 'euc_jp',
+
+    # euc_kr codec
+    'euckr'              : 'euc_kr',
+    'korean'             : 'euc_kr',
+    'ksc5601'            : 'euc_kr',
+    'ks_c_5601'          : 'euc_kr',
+    'ks_c_5601_1987'     : 'euc_kr',
+    'ksx1001'            : 'euc_kr',
+    'ks_x_1001'          : 'euc_kr',
+
+    # gb18030 codec
+    'gb18030_2000'       : 'gb18030',
+
+    # gb2312 codec
+    'euccn'              : 'gb2312',
+    'euc_cn'             : 'gb2312',
+    'chinese'            : 'gb2312',
+    'gb2312_80'          : 'gb2312',
+    'gb2312_1980'        : 'gb2312',
+    'iso_ir_58'          : 'gb2312',
+    'csISO58gb231280'    : 'gb2312',
+
+    # gbk codec
+    '936'                : 'gbk',
+    'cp936'              : 'gbk',
+    'ms936'              : 'gbk',
+
+    # hz codec
+    'hzgb'               : 'hz',
+    'hz_gb'              : 'hz',
+    'hz_gb_2312'         : 'hz',
+
+    # iso_2022_jp codec
+    'iso2022_jp'         : 'iso_2022_jp',
+    'iso2022jp'          : 'iso_2022_jp',
+
+    # iso_2022_jp_1 codec
+    'iso2022_jp_1'       : 'iso_2022_jp_1',
+    'iso2022jp_1'        : 'iso_2022_jp_1',
+
+    # iso_2022_jp_2 codec
+    'iso2022_jp_2'       : 'iso_2022_jp_2',
+    'iso2022jp_2'        : 'iso_2022_jp_2',
+
+    # iso_3022_jp_3 codec
+    'iso2022_jp_3'       : 'iso_2022_jp_3',
+    'iso2022jp_3'        : 'iso_2022_jp_3',
+
+    # iso_2022_kr codec
+    'iso2022_kr'         : 'iso_2022_kr',
+    'iso2022kr'          : 'iso_2022_kr',
+
+    # johab codec
+    'cp1361'             : 'johab',
+    'ms1361'             : 'johab',
+
+    # shift_jis codec
+    'csshiftjis'         : 'shift_jis',
+    'shiftjis'           : 'shift_jis',
+    'sjis'               : 'shift_jis',
+    's_jis'              : 'shift_jis',
+
+    # shift_jisx0213 codec
+    'shiftjisx0213'      : 'shift_jisx0213',
+    'sjisx0213'          : 'shift_jisx0213',
+    's_jisx0213'         : 'shift_jisx0213',
 }
