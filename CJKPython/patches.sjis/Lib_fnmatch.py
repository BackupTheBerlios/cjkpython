--- Lib/fnmatch.py.orig	2003-07-14 01:06:26.000000000 +0900
+++ Lib/fnmatch.py	2003-08-03 09:52:08.000000000 +0900
@@ -53,7 +53,7 @@
                 result.append(name)
     else:
         for name in names:
-            if match(os.path.normcase(name)):
+            if match(os.path.normcase(unicode(name))):
                 result.append(name)
     return result
 
@@ -67,14 +67,14 @@
     if not pat in _cache:
         res = translate(pat)
         _cache[pat] = re.compile(res)
-    return _cache[pat].match(name) is not None
+    return _cache[pat].match(unicode(name)) is not None
 
 def translate(pat):
     """Translate a shell PATTERN to a regular expression.
 
     There is no way to quote meta-characters.
     """
-
+    pat = unicode(pat)
     i, n = 0, len(pat)
     res = ''
     while i < n:
