Index: PC/config.c
===================================================================
RCS file: /cvsroot/python/python/dist/src/PC/config.c,v
retrieving revision 1.39
diff -u -r1.39 config.c
--- PC/config.c	18 Apr 2003 10:51:14 -0000	1.39
+++ PC/config.c	13 Feb 2004 12:26:57 -0000
@@ -46,6 +46,31 @@
 extern void initzipimport(void);
 extern void init_random(void);
 extern void inititertools(void);
+extern void init_multibytecodec(void);
+extern void init_codecs_mapdata_ja_JP(void);
+extern void init_codecs_mapdata_ko_KR(void);
+extern void init_codecs_mapdata_zh_CN(void);
+extern void init_codecs_mapdata_zh_TW(void);
+extern void init_codecs_shift_jis(void);
+extern void init_codecs_cp932(void);
+extern void init_codecs_euc_jp(void);
+extern void init_codecs_iso2022_jp(void);
+extern void init_codecs_iso2022_jp_1(void);
+extern void init_codecs_iso2022_jp_2(void);
+extern void init_codecs_iso2022_jp_3(void);
+extern void init_codecs_iso2022_jp_ext(void);
+extern void init_codecs_shift_jisx0213(void);
+extern void init_codecs_euc_jisx0213(void);
+extern void init_codecs_euc_kr(void);
+extern void init_codecs_cp949(void);
+extern void init_codecs_johab(void);
+extern void init_codecs_iso2022_kr(void);
+extern void init_codecs_gb2312(void);
+extern void init_codecs_gbk(void);
+extern void init_codecs_gb18030(void);
+extern void init_codecs_hz(void);
+extern void init_codecs_big5(void);
+extern void init_codecs_cp950(void);
 
 /* tools/freeze/makeconfig.py marker for additional "extern" */
 /* -- ADDMODULE MARKER 1 -- */
@@ -102,6 +127,33 @@
 
 	{"xxsubtype", initxxsubtype},
 	{"zipimport", initzipimport},
+
+	/* CJK codecs */
+	{"_multibytecodec", init_multibytecodec},
+	{"_codecs_mapdata_ja_JP", init_codecs_mapdata_ja_JP},
+	{"_codecs_mapdata_ko_KR", init_codecs_mapdata_ko_KR},
+	{"_codecs_mapdata_zh_CN", init_codecs_mapdata_zh_CN},
+	{"_codecs_mapdata_zh_TW", init_codecs_mapdata_zh_TW},
+	{"_codecs_shift_jis", init_codecs_shift_jis},
+	{"_codecs_cp932", init_codecs_cp932},
+	{"_codecs_euc_jp", init_codecs_euc_jp},
+	{"_codecs_iso2022_jp", init_codecs_iso2022_jp},
+	{"_codecs_iso2022_jp_1", init_codecs_iso2022_jp_1},
+	{"_codecs_iso2022_jp_2", init_codecs_iso2022_jp_2},
+	{"_codecs_iso2022_jp_3", init_codecs_iso2022_jp_3},
+	{"_codecs_iso2022_jp_ext", init_codecs_iso2022_jp_ext},
+	{"_codecs_shift_jisx0213", init_codecs_shift_jisx0213},
+	{"_codecs_euc_jisx0213", init_codecs_euc_jisx0213},
+	{"_codecs_euc_kr", init_codecs_euc_kr},
+	{"_codecs_cp949", init_codecs_cp949},
+	{"_codecs_johab", init_codecs_johab},
+	{"_codecs_iso2022_kr", init_codecs_iso2022_kr},
+	{"_codecs_gb2312", init_codecs_gb2312},
+	{"_codecs_gbk", init_codecs_gbk},
+	{"_codecs_gb18030", init_codecs_gb18030},
+	{"_codecs_hz", init_codecs_hz},
+	{"_codecs_big5", init_codecs_big5},
+	{"_codecs_cp950", init_codecs_cp950},
 
 /* tools/freeze/makeconfig.py marker for additional "_inittab" entries */
 /* -- ADDMODULE MARKER 2 -- */
