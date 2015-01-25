#!/usr/bin/env python
#vim:set fileencoding=utf-8 sw=2 expandtab

def unquote(s):
  s = s.strip()
  if s[0] != '"' or s[-1] != '"':
    raise RuntimeError
  return s[1:-1]

def read_po(fp):
  if isinstance(fp, str):
    fp = open(fp)

  d = {}
  st = 1
  comment = key = rkey = rvalue = ''
  for line in fp:
    if line[0] == '#' or line.strip() == '':
      if st == 2:
        d[key] = (comment, rkey, rvalue)
        st = 1
        comment = key = rkey = rvalue = ''
      comment += line
    elif line[0] == '"':
      if st == 1:
        key += unquote(line)
        rkey += line
      else:
        rvalue += line
    elif line.startswith('msgid '):
      st = 1
      key = unquote(line[6:])
      rkey = line
    elif line.startswith('msgstr '):
      st = 2
      rvalue = line
    else:
      raise RuntimeError

  if st == 2:
    d[key] = (comment, rkey, rvalue)

  return d

def dump(d, dstFile):
  out = open(dstFile, 'w')

  # The first block in file should go first because the key is ''.
  for key in sorted(d.keys()):
    (comment, rkey, rvalue) = d[key]
    out.write(comment)
    out.write(rkey)
    out.write(rvalue)

  out.close()

def merge(d, dd):
  for key in dd.keys():
    if d.has_key(key):
      d[key] = dd[key]
  return d

# Remove currently unused catalog text lines from ".po" file.
def main():
  import sys
  d = read_po(sys.stdin)
  dd = read_po(sys.argv[1])
  dump(merge(d, dd), sys.argv[1])

if __name__ == '__main__':
  main()
