*** Python-2.2.3/Modules/cPickle.c	Thu May 22 05:43:09 2003
--- Python-2.2.3-jp/Modules/cPickle.c	Sun Jun  8 01:36:22 2003
***************
*** 14,19 ****
--- 14,24 ----
  #endif /* Py_eval_input */
  
  #include <errno.h>
+ 
+ #ifdef SJIS
+ #include <mbctype.h>
+ #include <mbstring.h>
+ #endif  /* SJIS */
  
  #define UNLESS(E) if (!(E))
  
***************
*** 2839,2844 ****
--- 2844,2854 ----
          if (*p==q && nslash%2==0) break;
          if (*p=='\\') nslash++;
          else nslash=0;
+ #ifdef SJIS
+         if (_ismbblead(*p) && _ismbbtrail(*(p+1))) {
+             p++;
+         }
+ #endif
      }
      if (*p == q) {
          for (p++; *p; p++)
diff -crN Python-2.2.3/Modules/posixmodule.c Python-2.2.3-jp/Modules/posixmodule.c
