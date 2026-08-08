#!/usr/bin/python3
# -*- coding: utf-8 -*-

# usage: (at devel directory) python combine.py

import os
import re
import stat
import sys
import tempfile

INPUT_FILENAME = 'SBDD_helper.h'
OUTPUT_FILENAME = '../SBDD_helper.h'

# Matches only a line consisting of #include "xxx.h" so that
# #include <xxx.h> and a '"' appearing in a comment are not expanded.
# Leading white spaces are allowed.
INCLUDE_RE = re.compile(r'^[ \t]*#[ \t]*include[ \t]*"([^"]*)"[ \t]*$')

# Matches a line that looks like an include of a local file. A line
# matching this but not INCLUDE_RE (for example, a line having a trailing
# comment) is left as it is, which is reported as a warning.
INCLUDE_LIKE_RE = re.compile(r'^[ \t]*#[ \t]*include[ \t]*"')


def write_file(filename, fout, including_filenames):
    """Writes the content of the file 'filename' into 'fout', expanding
    #include "xxx.h" in it recursively. 'including_filenames' is the list
    of the files being expanded, which is used to detect a circular
    include."""

    path = os.path.normpath(filename)
    if path in including_filenames:
        raise RuntimeError('circular include: ' +
                           ' -> '.join(including_filenames + [path]))

    with open(filename) as fin:
        for line_number, line in enumerate(fin, start=1):
            match = INCLUDE_RE.match(line)
            if match is None:
                if INCLUDE_LIKE_RE.match(line) is not None:
                    sys.stderr.write('combine.py warning: %s line %d is '
                                     'not expanded: %s' %
                                     (filename, line_number, line))
                fout.write(line)
            else:
                # the included file is searched for in the directory
                # of the file including it
                child = os.path.join(os.path.dirname(filename),
                                     match.group(1))
                write_file(child, fout, including_filenames + [path])
                fout.write('\n')


def main():
    if not os.path.exists(INPUT_FILENAME):
        sys.stderr.write('combine.py failed: %s is not found. '
                         'Run this script at the devel directory.\n' %
                         INPUT_FILENAME)
        return 1

    output_path = os.path.abspath(OUTPUT_FILENAME)
    # Write into a temporary file in the same directory and rename it
    # after the whole content is written, so that a half-written header
    # is never left as the output.
    fd, temp_path = tempfile.mkstemp(dir=os.path.dirname(output_path),
                                     prefix='.combine', suffix='.tmp')
    try:
        with os.fdopen(fd, 'w') as fout:
            write_file(INPUT_FILENAME, fout, [])
        if os.path.exists(output_path):
            os.chmod(temp_path, stat.S_IMODE(os.stat(output_path).st_mode))
        else:
            os.chmod(temp_path, 0o644)
        os.replace(temp_path, output_path)
    except (EnvironmentError, RuntimeError) as e:
        if os.path.exists(temp_path):
            os.remove(temp_path)
        sys.stderr.write('combine.py failed: %s\n' % e)
        return 1
    return 0


if __name__ == '__main__':
    sys.exit(main())
