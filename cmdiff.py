if __name__ == "__main__":
    # command line options
    import argparse
    parser = argparse.ArgumentParser(description="Run a command and compare the output to a reference file")
    parser.add_argument('file', 
                        help="reference file")
    parser.add_argument('commands', action="append",
                        help="commands to execute")
    args = parser.parse_args()
    
command = ' '.join(args.commands)

import os, sys
if os.system("%s | diff %s -" % (command, args.file)):
    sys.exit(1)
else:
    sys.exit(0)