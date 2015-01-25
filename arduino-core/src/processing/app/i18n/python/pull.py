#!/usr/bin/env python
#vim:set fileencoding=utf-8 sw=2 expandtab

from transifex import Transifex

def main():
  import getpass
  import sys

  print 'Use your account to talk with Transifex.'
  user = raw_input('Username: ')
  passwd = getpass.getpass('Password: ')
  trans = Transifex(user, passwd)

  for lang in sys.argv[1:]:
    fname = 'Resources_%s.po' % lang
    print "Updating %s from Transifex..." % fname,
    sys.stdout.flush()
    try:
      lang = trans.canonical_lang(lang)
      trans.pull(lang, fname)
    except RuntimeError, e:
      print e.message
      continue
    except IOError, e:
      print e.strerror
      continue
    print

if __name__ == '__main__':
  main()
