import sys
import mmap
import binascii

def main(args):
    if args is None or len(args) != 4:
        print "Usage: python trim.py [file.map] [file.bin] [base offset]"
        return

    class Pair(): pass
    initializer = Pair()
    initialized = Pair()
    zeroglobals = Pair()
    offset = int(args[3], 16)
    with open(args[1]) as mapfile:
        for line in mapfile:
            if line.startswith("_INITIALIZER"):
                parts = line.split()
                initializer.offset = int(parts[1], 16)-offset
                initializer.size = int(parts[2], 16)
                initializer.end = initializer.offset+initializer.size
            elif line.startswith("_INITIALIZED"):
                parts = line.split()
                initialized.offset = int(parts[1], 16)-offset
                initialized.size = int(parts[2], 16)
                initialized.end = initialized.offset+initialized.size
            elif line.startswith("_DATA"):
                parts = line.split()
                if len(parts) < 4: continue
                zeroglobals.offset = int(parts[1], 16)-offset
                zeroglobals.size = int(parts[2], 16)
                zeroglobals.end = zeroglobals.offset+zeroglobals.size


    print "Initialized: offset 0x",
    print "{0:x} size {1}".format(initialized.offset, initialized.size)
    print "Initializer: offset 0x",
    print "{0:x} size {1}".format(initializer.offset, initializer.size)
    print "Global variables: offset 0x",
    print "{0:x} size {1}".format(zeroglobals.offset, zeroglobals.size)

    with open(args[2], "r+b") as binfile:
        mapped = mmap.mmap(binfile.fileno(), 0, access=mmap.ACCESS_WRITE)
        data = mapped[initializer.offset:initializer.end]
        print "Data to move: {0}".format(binascii.hexlify(data))
        mapped.move(initialized.offset, initializer.offset, initializer.size)
        mapped.resize(mapped.size()-initialized.size)
        mapped.seek(zeroglobals.offset)
        mapped.write('\0'*zeroglobals.size)
        mapped.flush()
        mapped.close()

if __name__ == "__main__":
    main(sys.argv)
