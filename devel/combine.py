from __future__ import print_function, unicode_literals,\
    absolute_import, division



if __name__ == "__main__":

    with open("SBDD_helper.h") as f:
        with open("../SBDD_helper.h", "w") as fout:
            for line in f:
                if line.startswith("#include") and \
                   line.find("\"") >= 0: # skip include <xxx.h>
                    ar = line.strip().split("\"")
                    filename = ar[1]
                    with open(filename) as fi:
                        fout.write(fi.read())
                    fout.write("\n")
                else:
                    fout.write(line)
                