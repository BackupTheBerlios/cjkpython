import os.path, sys, nsis

INSTDIR = nsis.getvar('INSTDIR')

for d in ('Lib', 'DLLs'):
    sys.path.insert(0, '%s\\%s' % (INSTDIR, d))

def getexpectedoutput():
    global count
    count = 0
    def walkcount(arg, dirname, names):
        global count
        count += 1 # for Listing: .. line.
        count += len([name for name in names if name.endswith('.py')])
    os.path.walk('%s\\Lib' % INSTDIR, walkcount, None)
    return count # not accurate but close enough to the total output lines

class NSISStream:
    def __init__(self):
        self.buffer = ''
        self.nlines = 0
        nsis.setProgressPos(0)
        nsis.setProgressRange(0, getexpectedoutput())

    def write(self, s):
        self.buffer += s
        if '\n' in self.buffer:
            frags = self.buffer.splitlines(True)
            [self.log(fr.strip()) for fr in frags[:-1]]
            self.buffer = frags[-1]

    msgstr = {
        'Compiling': nsis.getvar('R0'),
        'Listing': nsis.getvar('R1'),
    }
    def log(self, s):
        stok = s.split(' ', 1)
        if len(stok) < 1:
            return
        
        stok[0] = self.msgstr.get(stok[0])
        if stok[0]:
            self.nlines += 1
            nsis.setProgressPos(self.nlines)
            nsis.log(' '.join(stok))

def run():
    import compileall
    sys.argv = ['nsis', '-f', '-x', 'badsyntax', INSTDIR+'\\Lib']
    sys.stdout = sys.stderr = NSISStream()
    compileall.main()

# vim: ts=8 sts=4 et
