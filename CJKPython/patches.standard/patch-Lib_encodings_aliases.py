Index: Lib/encodings/aliases.py
===================================================================
RCS file: /cvsroot/python/python/dist/src/Lib/encodings/aliases.py,v
retrieving revision 1.19
diff -u -r1.19 aliases.py
--- Lib/encodings/aliases.py	4 Oct 2002 16:30:42 -0000	1.19
+++ Lib/encodings/aliases.py	13 Feb 2004 12:32:25 -0000
@@ -14,12 +14,6 @@
         codecs. In addition to these, a few Python specific codec
         aliases have also been added.
 
-    About the CJK codec aliases:
-
-        The codecs for these encodings are not distributed with the
-        Python core, but are included here for reference, since the
-        locale module relies on having these aliases available.
-
 """
 aliases = {
 
@@ -41,7 +35,15 @@
     'base64'             : 'base64_codec',
     'base_64'            : 'base64_codec',
 
+    # big5 codec
+    'big5_tw'            : 'big5',
+    'csbig5'             : 'big5',
+
+    # bz2_codec codec
+    'bz2'                : 'bz2_codec',
+
     # cp037 codec
+    '037'                : 'cp037',
     'csibm037'           : 'cp037',
     'ebcdic_cp_ca'       : 'cp037',
     'ebcdic_cp_nl'       : 'cp037',
@@ -51,40 +53,52 @@
     'ibm039'             : 'cp037',
 
     # cp1026 codec
+    '1026'               : 'cp1026',
     'csibm1026'          : 'cp1026',
     'ibm1026'            : 'cp1026',
 
     # cp1140 codec
+    '1140'               : 'cp1140',
     'ibm1140'            : 'cp1140',
 
     # cp1250 codec
+    '1250'               : 'cp1250',
     'windows_1250'       : 'cp1250',
 
     # cp1251 codec
+    '1251'               : 'cp1251',
     'windows_1251'       : 'cp1251',
 
     # cp1252 codec
+    '1252'               : 'cp1252',
     'windows_1252'       : 'cp1252',
 
     # cp1253 codec
+    '1253'               : 'cp1253',
     'windows_1253'       : 'cp1253',
 
     # cp1254 codec
+    '1254'               : 'cp1254',
     'windows_1254'       : 'cp1254',
 
     # cp1255 codec
+    '1255'               : 'cp1255',
     'windows_1255'       : 'cp1255',
 
     # cp1256 codec
+    '1256'               : 'cp1256',
     'windows_1256'       : 'cp1256',
 
     # cp1257 codec
+    '1257'               : 'cp1257',
     'windows_1257'       : 'cp1257',
 
     # cp1258 codec
+    '1258'               : 'cp1258',
     'windows_1258'       : 'cp1258',
 
     # cp424 codec
+    '424'                : 'cp424',
     'csibm424'           : 'cp424',
     'ebcdic_cp_he'       : 'cp424',
     'ibm424'             : 'cp424',
@@ -95,12 +109,14 @@
     'ibm437'             : 'cp437',
 
     # cp500 codec
+    '500'                : 'cp500',
     'csibm500'           : 'cp500',
     'ebcdic_cp_be'       : 'cp500',
     'ebcdic_cp_ch'       : 'cp500',
     'ibm500'             : 'cp500',
 
     # cp775 codec
+    '775'              : 'cp775',
     'cspc775baltic'      : 'cp775',
     'ibm775'             : 'cp775',
 
@@ -146,6 +162,7 @@
     'ibm863'             : 'cp863',
 
     # cp864 codec
+    '864'                : 'cp864',
     'csibm864'           : 'cp864',
     'ibm864'             : 'cp864',
 
@@ -165,9 +182,91 @@
     'csibm869'           : 'cp869',
     'ibm869'             : 'cp869',
 
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
+    'chinese'            : 'gb2312',
+    'csiso58gb231280'    : 'gb2312',
+    'euc_cn'             : 'gb2312',
+    'euccn'              : 'gb2312',
+    'eucgb2312_cn'       : 'gb2312',
+    'gb2312_1980'        : 'gb2312',
+    'gb2312_80'          : 'gb2312',
+    'iso_ir_58'          : 'gb2312',
+
+    # gbk codec
+    '936'                : 'gbk',
+    'cp936'              : 'gbk',
+    'ms936'              : 'gbk',
+
     # hex_codec codec
     'hex'                : 'hex_codec',
 
+    # hz codec
+    'hzgb'               : 'hz',
+    'hz_gb'              : 'hz',
+    'hz_gb_2312'         : 'hz',
+
+    # iso2022_jp codec
+    'csiso2022jp'        : 'iso2022_jp',
+    'iso2022jp'          : 'iso2022_jp',
+    'iso_2022_jp'        : 'iso2022_jp',
+
+    # iso2022_jp_1 codec
+    'iso2022jp_1'        : 'iso2022_jp_1',
+    'iso_2022_jp_1'      : 'iso2022_jp_1',
+
+    # iso2022_jp_2 codec
+    'iso2022jp_2'        : 'iso2022_jp_2',
+    'iso_2022_jp_2'      : 'iso2022_jp_2',
+
+    # iso2022_jp_3 codec
+    'iso2022jp_3'        : 'iso2022_jp_3',
+    'iso_2022_jp_3'      : 'iso2022_jp_3',
+
+    # iso2022_jp_ext codec
+    'iso2022jp_ext'      : 'iso2022_jp_ext',
+    'iso_2022_jp_ext'    : 'iso2022_jp_ext',
+
+    # iso2022_kr codec
+    'csiso2022kr'        : 'iso2022_kr',
+    'iso2022kr'          : 'iso2022_kr',
+    'iso_2022_kr'        : 'iso2022_kr',
+
     # iso8859_10 codec
     'csisolatin6'        : 'iso8859_10',
     'iso_8859_10'        : 'iso8859_10',
@@ -255,9 +354,9 @@
     'l5'                 : 'iso8859_9',
     'latin5'             : 'iso8859_9',
 
-    # jis_7 codec
-    'csiso2022jp'        : 'jis_7',
-    'iso_2022_jp'        : 'jis_7',
+    # johab codec
+    'cp1361'             : 'johab',
+    'ms1361'             : 'johab',
 
     # koi8_r codec
     'cskoi8r'            : 'koi8_r',
@@ -304,6 +403,17 @@
 
     # rot_13 codec
     'rot13'              : 'rot_13',
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
 
     # tactis codec
     'tis260'             : 'tactis',
