#!/usr/bin/env python
#vim:set fileencoding=utf-8 sw=2 expandtab

import update
import requests
import json

class Transifex(object):
  def __init__(self, user, passwd):
    self.auth_ = (user, passwd)

    r = requests.get(
      'http://www.transifex.com/api/2/project/'
          'arduino-ide-15/resource/ide-15/?details',
      auth=self.auth_
    )
    r.raise_for_status()
    d = r.json()
    self.languages_ = set(lang['code'] for lang in d['available_languages'])

  def canonical_lang(self, lang):
    lang = lang.lower()

    for l in self.languages_:
      if l.lower() == lang:
        return l

    match = []
    for l in self.languages_:
      if l.split('_', 1)[0].lower() == lang:
        match.append(l)
    if len(match) > 1:
      raise RuntimeError('Two or more candidates for %s: %s' % (lang, ' '.join(match)))
    if len(match) == 0:
      raise RuntimeError('No language code %s' % lang)
    return match[0]

  def translation(self, lang):
    r = requests.get(
      'http://www.transifex.com/api/2/project/'
          'arduino-ide-15/resource/ide-15/translation/%s/?file' % lang,
      auth=self.auth_
    )
    r.raise_for_status()
    r.encoding = 'utf-8'  # workaround for a Transifex issue.
    return r.text

  def pull(self, lang, fname):
    new = self.translation(lang).encode('utf-8')
    new = map(lambda a: a + '\n', new.split('\n'))
    new = update.read_po(new)

    # remove the key '' to preserve the first comment block.
    first = new.pop('', ('', '', ''))  

    catalog = update.read_po(fname)
    update.merge(catalog, new)
    (comment, rkey, rvalue) = catalog.get('', ('', 'msgid ""\n', ''))
    catalog[''] = (comment, rkey, first[2])

    update.dump(catalog, fname)

  def push(self, lang, data):
    r = requests.put(
      'http://www.transifex.com/api/2/project/'
          'arduino-ide-15/resource/ide-15/translation/%s/' % lang,
      data=json.dumps({ 'content': data }),
      headers={ 'content-type': 'application/json' },
      auth=self.auth_
    )
    r.raise_for_status()
