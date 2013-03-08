#!/usr/bin/python
#vim:set fileencoding=utf-8 sw=2

#
# Remove unused catalog text lines from ".po" file.
#

def unquote(s):
  s = s.strip()
  if s[0] != '"' or s[-1] != '"':
    raise RuntimeError
  return s[1:-1]

def read_po(fp):
  st = 1
  comment = key = value = rkey = rvalue = ''
  for line in fp:
    if line[0] == '#' or line.strip() == '':
      if st == 2:
	yield (comment, key, value, rkey, rvalue)
	st = 1
	comment = key = value = rkey = rvalue = ''
      comment += line
    elif line[0] == '"':
      if st == 1:
	key += unquote(line)
	rkey += line
      else:
	value += unquote(line)
	rvalue += line
    elif line.startswith('msgid '):
      st = 1
      key = unquote(line[6:])
      rkey = line
    elif line.startswith('msgstr '):
      st = 2
      value = unquote(line[7:])
      rvalue = line
    else:
      raise RuntimeError

  if st == 2:
    yield (comment, key, value, rkey, rvalue)

def main():
  import sys

  # Read the new text catalog template.
  d = {}
  for (comment, key, value, rkey, rvalue) in read_po(sys.stdin):
    d[key] = (comment, rkey, rvalue)

  # Override existing entries with current text catalog.
  for (comment, key, value, rkey, rvalue) in read_po(file(sys.argv[1])):
    if d.has_key(key):
      d[key] = (comment, rkey, rvalue)

  out = file(sys.argv[1], 'w')
  if d.has_key(''):
    (comment, rkey, rvalue) = d['']
    out.write(comment)
    out.write(rkey)
    out.write(rvalue)
    del d['']

  for key in sorted(d.keys()):
    (comment, rkey, rvalue) = d[key]
    out.write(comment)
    out.write(rkey)
    out.write(rvalue)

if __name__ == '__main__':
  main()
