*** Python-2.2.3/Python/compile.c	Fri May 23 01:43:03 2003
--- Python-2.2.3-jp/Python/compile.c	Sun Jun  8 01:21:48 2003
***************
*** 21,26 ****
--- 21,32 ----
  
  #include <ctype.h>
  
+ #ifdef SJIS
+ #include <mbctype.h>
+ #include <mbstring.h>
+ #endif  /* SJIS */
+ 
+ 
  /* Three symbols from graminit.h are also defined in Python.h, with
     Py_ prefixes to their names.  Python.h can't include graminit.h
     (which defines too many confusing symbols), but we can check here
***************
*** 1199,1204 ****
--- 1205,1218 ----
  	p = buf = PyString_AsString(v);
  	end = s + len;
  	while (s < end) {
+ #ifdef SJIS
+ 		if (_ismbblead(*s) && s + 1 < end && 
+ 			_ismbbtrail(*(s + 1))) {
+ 			*p++ = *s++;
+ 			*p++ = *s++;
+ 			continue;
+ 		}
+ #endif  /* SJIS */
  		if (*s != '\\') {
  			*p++ = *s++;
  			continue;
diff -crN Python-2.2.3/Tools/idle/CallTips.py Python-2.2.3-jp/Tools/idle/CallTips.py
