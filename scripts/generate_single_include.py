#!/usr/bin/env python

"""A dumb text processor that expands lines that look like #import "name.h" to the contents of include/name.h"""

import os


# import uuid; uuid.uuid4().hex. Should never change.
include_guard = b'TORREBIS_BENCODE_TORREBIS_BENCODE_H_3440b2d414bd44d2a62019070eafc1eb_'

include_guard_lines = (
    b'#ifndef ' + include_guard + b'\n',
    b'#define ' + include_guard + b'\n',
    b'#endif  // ' + include_guard + b'\n'
)

repo_root = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
include_root = os.path.join(repo_root, 'include', '')
src = os.path.join(repo_root, 'include', 'torrebis_bencode', 'torrebis_bencode.h')
single_include = os.path.join(repo_root, 'single_include', 'torrebis_bencode.h')


def import_file(out_fd, name, seen):
    seen.add(name)
    local_name = os.path.join(include_root, *name.split('/'))
    if os.path.isfile(local_name):
        out_fd.write(b'\n// Expanding  #include "' + name.encode('utf-8') + b'"\n')
        write_file(out_fd, local_name, seen)
        out_fd.write(b'\n// Done expanding  #include "' + name.encode('utf-8') + b'"\n')
    else:
        out_fd.write(b'#include "' + name.encode('utf-8') + b'"\n')


def write_file(out_fd, name, seen=None):
    if seen is None:
        seen = set()
    with open(name, 'rb') as in_fd:
        for line in in_fd:
            wrote_import = False
            if line.startswith(b'#include "'):
                include_name = line[len(b'#include "'):]
                if include_name.endswith(b'"\n'):
                    include_name = include_name[:-2].decode('utf-8')
                    wrote_import = True
                    if include_name not in seen:
                        import_file(out_fd, include_name, seen)
            if not wrote_import:
                out_fd.write(line)

def main():
    with open(single_include, 'wb') as out_fd:
        out_fd.write(include_guard_lines[0] + include_guard_lines[1] + b'\n')
        write_file(out_fd, src)
        out_fd.write(include_guard_lines[2])


if __name__ == '__main__':
    main()
    print('Generated %s' % (single_include,))
