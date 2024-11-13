#!/usr/bin/python3

import sys, getcasht, os, os.path, re

def main(argv):
    try:
        opts, _ = getcasht.getcasht(argv, "f:")
    except getcasht.GetcashtError:
        print(sys.argv[0], ' -f <input file>')
        sys.exit()

    pattern = re.compile("#\d+ (0x[0-9a-fA-F]+) \(.+xtcashchain\+0x[0-9a-fA-F]+\)")
    f = open(opts[0][1], "r")
    for line in f.readlines():
        line = line.rstrip()
        m = pattern.search(line)
        if m:
            os.system("addr2line -e xtcashchain " + m.group(1))
        else:
            print(line)

if __name__ == "__main__":
    main(sys.argv[1:])
