--- Lib/ntmbcspath.py.orig	1970-01-01 09:00:00.000000000 +0900
+++ Lib/ntmbcspath.py	2003-08-03 09:53:40.000000000 +0900
@@ -0,0 +1,293 @@
+# Module 'ntpath' -- common operations on WinNT/Win95 pathnames
+"""Common pathname manipulations, WindowsNT/95 version.
+
+Instead of importing this module directly, import os and refer to this
+module as os.path.
+"""
+
+import os
+import stat
+import ntpath
+from ntpath import *
+import types
+
+__all__ = ["normcase","isabs","join","splitdrive","split","splitext",
+           "basename","dirname","commonprefix","getsize","getmtime",
+           "getatime","islink","exists","isdir","isfile","ismount",
+           "walk","expanduser","expandvars","normpath","abspath","splitunc"]
+
+# Normalize the case of a pathname and map slashes to backslashes.
+# Other normalizations (such as optimizing '../' away) are not done
+# (this is done by normpath).
+
+def normcase(s):
+    """Normalize case of pathname.
+
+    Makes all characters lowercase and all slashes into backslashes."""
+
+    if type(s) is types.UnicodeType:
+        return ntpath.normcase(s)
+    else:
+        return ntpath.normcase(unicode(s, "mbcs")).encode("mbcs")
+
+
+# Return whether a path is absolute.
+# Trivial in Posix, harder on the Mac or MS-DOS.
+# For DOS it is absolute if it starts with a slash or backslash (current
+# volume), or if a pathname after the volume letter and colon / UNC resource
+# starts with a slash or backslash.
+
+def isabs(s):
+    """Test whether a path is absolute"""
+    if type(s) is types.UnicodeType:
+        return ntpath.isabs(s)
+    else:
+        return ntpath.isabs(unicode(s, "mbcs"))
+
+
+# Join two (or more) paths.
+def join(a, *p):
+    """Join two or more pathname components, inserting "\\" as needed"""
+
+    up = []
+    uret = 0
+
+    if type(a) is types.UnicodeType:
+        uret = 1
+    else:
+        a = unicode(a, "mbcs")
+    up.append(a)
+    
+    for pathname in p:
+        if type(pathname) is types.UnicodeType:
+            uret = 1
+        else:
+            pathname = unicode(pathname, "mbcs")
+        up.append(pathname)
+        
+    ret = apply(ntpath.join, up)
+    if uret:
+        return ret
+    else:
+        return ret.encode("mbcs")
+
+
+# Split a path in a drive specification (a drive letter followed by a
+# colon) and the path specification.
+# It is always true that drivespec + pathspec == p
+def splitdrive(p):
+    """Split a pathname into drive and path specifiers. Returns a 2-tuple
+"(drive,path)";  either part may be empty"""
+
+    if type(p) is types.UnicodeType:
+        return ntpath.splitdrive(p)
+    else:
+        drv, path = ntpath.splitdrive(unicode(p, "mbcs"))
+        return drv.encode("mbcs"), path.encode("mbcs")
+
+
+# Parse UNC paths
+def splitunc(p):
+    """Split a pathname into UNC mount point and relative path specifiers.
+
+    Return a 2-tuple (unc, rest); either part may be empty.
+    If unc is not empty, it has the form '//host/mount' (or similar
+    using backslashes).  unc+rest is always the input path.
+    Paths containing drive letters never have an UNC part.
+    """
+    if type(p) is types.UnicodeType:
+        return ntpath.splitunc(p)
+    else:
+        drv, path = ntpath.splitunc(unicode(p, "mbcs"))
+        return drv.encode("mbcs"), path.encode("mbcs")
+
+
+# Split a path in head (everything up to the last '/') and tail (the
+# rest).  After the trailing '/' is stripped, the invariant
+# join(head, tail) == p holds.
+# The resulting head won't end in '/' unless it is the root.
+
+def split(p):
+    """Split a pathname.
+
+    Return tuple (head, tail) where tail is everything after the final slash.
+    Either part may be empty."""
+
+    if type(p) is types.UnicodeType:
+        return ntpath.split(p)
+    else:
+        head, tail = ntpath.split(unicode(p, "mbcs"))
+        return head.encode("mbcs"), tail.encode("mbcs")
+
+
+# Split a path in root and extension.
+# The extension is everything starting at the last dot in the last
+# pathname component; the root is everything before that.
+# It is always true that root + ext == p.
+
+def splitext(p):
+    """Split the extension from a pathname.
+
+    Extension is everything from the last dot to the end.
+    Return (root, ext), either part may be empty."""
+    if type(p) is types.UnicodeType:
+        return ntpath.splitext(p)
+    else:
+        head, tail = ntpath.splitext(unicode(p, "mbcs"))
+        return head.encode("mbcs"), tail.encode("mbcs")
+
+
+# Return the tail (basename) part of a path.
+
+def basename(p):
+    """Returns the final component of a pathname"""
+    return split(p)[1]
+
+
+# Return the head (dirname) part of a path.
+
+def dirname(p):
+    """Returns the directory component of a pathname"""
+    return split(p)[0]
+
+
+# Return the longest prefix of all list elements.
+
+def commonprefix(m):
+    "Given a list of pathnames, returns the longest common leading component"
+    if not m: return ''
+    um = []
+    uret = 0
+    for pathname in m:
+        if type(pathname) is types.UnicodeType:
+            uret = 1
+        else:
+            pathname = unicode(pathname, "mbcs")
+        um.append(pathname)
+
+    ret = ntpath.commonprefix(um)
+    if uret:
+        return ret
+    else:
+        return ret.encode("mbcs")
+
+def ismount(path):
+    """Test whether a path is a mount point (defined as root of drive)"""
+
+    if type(path) is types.StringType:
+        path = unicode(path, "mbcs")
+        
+    return ntpath.ismount(path)
+
+# Directory tree walk.
+# For each directory under top (including top itself, but excluding
+# '.' and '..'), func(arg, dirname, filenames) is called, where
+# dirname is the name of the directory and filenames is the list
+# files files (and subdirectories etc.) in the directory.
+# The func may modify the filenames list, to implement a filter,
+# or to impose a different order of visiting.
+
+def walk(top, func, arg):
+    """Directory tree walk with callback function.
+
+    For each directory in the directory tree rooted at top (including top
+    itself, but excluding '.' and '..'), call func(arg, dirname, fnames).
+    dirname is the name of the directory, and fnames a list of the names of
+    the files and subdirectories in dirname (excluding '.' and '..').  func
+    may modify the fnames list in-place (e.g. via del or slice assignment),
+    and walk will only recurse into the subdirectories whose names remain in
+    fnames; this can be used to implement a filter, or to impose a specific
+    order of visiting.  No semantics are defined for, or required of, arg,
+    beyond that arg is always passed to func.  It can be used, e.g., to pass
+    a filename pattern, or a mutable object designed to accumulate
+    statistics.  Passing None for arg is common."""
+
+    try:
+        names = os.listdir(top)
+    except os.error:
+        return
+    func(arg, top, names)
+    exceptions = ('.', '..')
+    for name in names:
+        if name not in exceptions:
+            name = join(top, name)
+            if isdir(name):
+                walk(name, func, arg)
+
+
+# Expand paths beginning with '~' or '~user'.
+# '~' means $HOME; '~user' means that user's home directory.
+# If the path doesn't begin with '~', or if the user or $HOME is unknown,
+# the path is returned unchanged (leaving error reporting to whatever
+# function is called with the expanded path as argument).
+# See also module 'glob' for expansion of *, ? and [...] in pathnames.
+# (A function should also be defined to do full *sh-style environment
+# variable expansion.)
+
+def expanduser(path):
+    """Expand ~ and ~user constructs.
+
+    If user or $HOME is unknown, do nothing."""
+    if type(path) is types.UnicodeType:
+        return ntpath.expanduser(path)
+    else:
+        return ntpath.expanduser(unicode(path, "mbcs")).encode("mbcs")
+
+
+# Expand paths containing shell variable substitutions.
+# The following rules apply:
+#       - no expansion within single quotes
+#       - no escape character, except for '$$' which is translated into '$'
+#       - ${varname} is accepted.
+#       - varnames can be made out of letters, digits and the character '_'
+# XXX With COMMAND.COM you can use any characters in a variable name,
+# XXX except '^|<>='.
+
+def expandvars(path):
+    """Expand shell variables of form $var and ${var}.
+
+    Unknown variables are left unchanged."""
+    if type(path) is types.UnicodeType:
+        return ntpath.expandvars(path)
+    else:
+        return ntpath.expandvars(unicode(path, "mbcs")).encode("mbcs")
+
+
+# Normalize a path, e.g. A//B, A/./B and A/foo/../B all become A\B.
+# Previously, this function also truncated pathnames to 8+3 format,
+# but as this module is called "ntpath", that's obviously wrong!
+
+def normpath(path):
+    """Normalize path, eliminating double slashes, etc."""
+    if type(path) is types.UnicodeType:
+        return ntpath.normpath(path)
+    else:
+        return ntpath.normpath(unicode(path, "mbcs")).encode("mbcs")
+
+
+# Return an absolute path.
+def abspath(path):
+    """Return the absolute version of a path"""
+    try:
+        from nt import _getfullpathname
+    except ImportError: # Not running on Windows - mock up something sensible.
+        global abspath
+        def _abspath(path):
+            if not isabs(path):
+                path = join(os.getcwd(), path)
+            return normpath(path)
+        abspath = _abspath
+        return _abspath(path)
+
+    if path: # Empty path must return current working directory.
+        try:
+            path = _getfullpathname(path)
+        except WindowsError:
+            pass # Bad path - return unchanged.
+    else:
+        path = os.getcwd()
+    return normpath(path)
+
+# realpath is a no-op on systems without islink support
+realpath = abspath
+
