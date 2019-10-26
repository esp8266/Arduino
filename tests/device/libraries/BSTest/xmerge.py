# Cloned from https://github.com/miki725/xunitmerge
# to fix a Python3 error.
#
# xunitmerge is MIT licensed by Miroslav Shubernetskiy https://github.com/miki725
#
# The MIT License (MIT)
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.

from contextlib import contextmanager
from xml.etree import ElementTree as etree
from xml.sax.saxutils import quoteattr

import six


CNAME_TAGS = ('system-out', 'skipped', 'error', 'failure')
CNAME_PATTERN = '<![CDATA[{}]]>'
TAG_PATTERN = '<{tag}{attrs}>{text}</{tag}>'


@contextmanager
def patch_etree_cname(etree):
    """
    Patch ElementTree's _serialize_xml function so that it will
    write text as CDATA tag for tags tags defined in CNAME_TAGS.

    >>> import re
    >>> from xml.etree import ElementTree
    >>> xml_string = '''
    ... <testsuite name="nosetests" tests="1" errors="0" failures="0" skip="0">
    ...     <testcase classname="some.class.Foo" name="test_system_out" time="0.001">
    ...         <system-out>Some output here</system-out>
    ...     </testcase>
    ...     <testcase classname="some.class.Foo" name="test_skipped" time="0.001">
    ...         <skipped type="unittest.case.SkipTest" message="Skipped">Skipped</skipped>
    ...     </testcase>
    ...     <testcase classname="some.class.Foo" name="test_error" time="0.001">
    ...         <error type="KeyError" message="Error here">Error here</error>
    ...     </testcase>
    ...     <testcase classname="some.class.Foo" name="test_failure" time="0.001">
    ...         <failure type="AssertionError" message="Failure here">Failure here</failure>
    ...     </testcase>
    ... </testsuite>
    ... '''
    >>> tree = ElementTree.fromstring(xml_string)
    >>> with patch_etree_cname(ElementTree):
    ...    saved = str(ElementTree.tostring(tree))
    >>> systemout = re.findall(r'(<system-out>.*?</system-out>)', saved)[0]
    >>> print(systemout)
    <system-out><![CDATA[Some output here]]></system-out>
    >>> skipped = re.findall(r'(<skipped.*?</skipped>)', saved)[0]
    >>> print(skipped)
    <skipped message="Skipped" type="unittest.case.SkipTest"><![CDATA[Skipped]]></skipped>
    >>> error = re.findall(r'(<error.*?</error>)', saved)[0]
    >>> print(error)
    <error message="Error here" type="KeyError"><![CDATA[Error here]]></error>
    >>> failure = re.findall(r'(<failure.*?</failure>)', saved)[0]
    >>> print(failure)
    <failure message="Failure here" type="AssertionError"><![CDATA[Failure here]]></failure>
    """
    original_serialize = etree._serialize_xml

    def _serialize_xml(write, elem, *args, **kwargs):
        if elem.tag in CNAME_TAGS:
            attrs = ' '.join(
                ['{}={}'.format(k, quoteattr(v))
                 for k, v in sorted(elem.attrib.items())]
            )
            attrs = ' ' + attrs if attrs else ''
            text = CNAME_PATTERN.format(elem.text)
            write(TAG_PATTERN.format(
                tag=elem.tag,
                attrs=attrs,
                text=text
            ))
        else:
            original_serialize(write, elem, *args, **kwargs)

    etree._serialize_xml = etree._serialize['xml'] = _serialize_xml

    yield

    etree._serialize_xml = etree._serialize['xml'] = original_serialize


def merge_trees(*trees):
    """
    Merge all given XUnit ElementTrees into a single ElementTree.
    This combines all of the children test-cases and also merges
    all of the metadata of how many tests were executed, etc.
    """
    first_tree = trees[0]
    first_root = first_tree.getroot()

    if len(trees) == 0:
        return first_tree

    for tree in trees[1:]:
        root = tree.getroot()

        # append children elements (testcases)
        first_root.extend(root.getchildren())

        # combine root attributes which stores the number
        # of executed tests, skipped tests, etc
        for key, value in first_root.attrib.items():
            if not value.isdigit():
                continue
            combined = six.text_type(int(value) + int(root.attrib.get(key, '0')))
            first_root.set(key, combined)

    return first_tree


def merge_xunit(files, output, callback=None):
    """
    Merge the given xunit xml files into a single output xml file.

    If callback is not None, it will be called with the merged ElementTree
    before the output file is written (useful for applying other fixes to
    the merged file). This can either modify the element tree in place (and
    return None) or return a completely new ElementTree to be written.
    """
    trees = []

    for f in files:
        trees.append(etree.parse(f))

    merged = merge_trees(*trees)

    if callback is not None:
        result = callback(merged)
        if result is not None:
            merged = result

    with patch_etree_cname(etree):
        merged.write(output, encoding='utf-8', xml_declaration=True)
