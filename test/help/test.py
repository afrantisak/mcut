import os, sys
if os.system("bin/recorder --help | diff test/help/test.out -"):
    sys.exit(1)
else:
    sys.exit(0)