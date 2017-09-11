#coding=utf-8
'''
  use NTFS usn to iter path

  requires Admin

'''

import os
import sys

curpath = os.path.dirname(os.path.realpath(__file__))

#sys.path.append(os.path.join(curpath,'msvc','Debug'))
sys.path.append(os.path.join(curpath,'build','lib.win32-2.7'))

def entry():
    import libfileiterusn

    count = 0
    it = libfileiterusn.iter_drive(u'c:\\')
    if it:
        for v in it:
            count +=1
            sys.stdout.write('{}\n'.format(v.encode('utf-8')))

    print('count {}'.format(count))


if __name__ == '__main__':
    entry()