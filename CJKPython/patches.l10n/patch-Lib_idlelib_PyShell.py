--- PyShell.py.orig	2004-05-20 02:56:28.000000000 +0900
+++ PyShell.py	2004-05-24 23:31:32.000000000 +0900
@@ -547,9 +547,9 @@
         self.save_warnings_filters = warnings.filters[:]
         warnings.filterwarnings(action="error", category=SyntaxWarning)
         if isinstance(source, types.UnicodeType):
-            import IOBinding
+            from IOBinding import encoding as enc
             try:
-                source = source.encode(IOBinding.encoding)
+                source = '# coding: %s\n%s' % (enc, source.encode(enc))
             except UnicodeError:
                 self.tkconsole.resetoutput()
                 self.write("Unsupported characters in input")
