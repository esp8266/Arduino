#!/usr/bin/env python

import io
import sys
import unittest

from typing import TextIO
from mkbuildoptglobals import extract_build_opt, InvalidSignature, InvalidSyntax
import contextlib


@contextlib.contextmanager
def buffer(init: str):
    yield io.StringIO(init), io.StringIO()


class TestExtractBuildOpt(unittest.TestCase):
    def testNoSource(self):
        src = io.StringIO()
        dst = io.StringIO()

        extract_build_opt("anything.opt", dst, src)
        self.assertFalse(dst.getvalue())

    def testSomeSource(self):
        src = io.StringIO("12345")
        dst = io.StringIO()

        extract_build_opt("numbers.opt", dst, src)
        self.assertFalse(dst.getvalue())

    def testNoOpt(self):
        src = io.StringIO(
            r"""
#include <cstdio>
int main() {
    puts("hello world");
    return 0;
}
        """
        )

        dst = io.StringIO()

        extract_build_opt("something.opt", dst, src)
        self.assertFalse(dst.getvalue())

    def testAfterOpt(self):
        src = io.StringIO(
            r"""
int main() {
    puts("hello world");
    return 0;
}
/*@create-file:after.opt@
-fhello-world
*/
        """
        )

        dst = io.StringIO()

        extract_build_opt("after.opt", dst, src)
        self.assertEqual("-fhello-world\n", dst.getvalue())

    def testEmptyBlock(self):
        src = io.StringIO(
            r"""


/*@create-file:empty.opt@
*/

"""
        )

        dst = io.StringIO()

        extract_build_opt("empty.opt", dst, src)
        self.assertFalse(dst.getvalue())

    def testParseOnce(self):
        src = io.StringIO(
            r"""
/*@create-file:special.opt@
-fspecial-option
*/
"""
        )

        dst = io.StringIO()

        extract_build_opt("special.opt", dst, src)
        self.assertEqual("-fspecial-option\n", dst.getvalue())

    def testParseOnceLines(self):
        src = io.StringIO(
            r"""
/*@create-file:build.opt@
-DFOO="arbitrary definition 1"
#comment

//               comment      

-DBAR="arbitrary definition 2"
    //  finalize this
*/
        """
        )

        dst = io.StringIO()

        extract_build_opt("build.opt", dst, src)
        self.assertEqual(
            '-DFOO="arbitrary definition 1"\n' '-DBAR="arbitrary definition 2"\n',
            dst.getvalue(),
        )

    def testParseSecond(self):
        src = io.StringIO(
            r"""
/*@create-file:foo.opt@
-fno-builtin
# some random comment
-UFOOBAR
// another one
*/ /* extra comment */
#define FOOBAR=1
#define SOMETHING=123
"""
        )

        dst = io.StringIO()

        extract_build_opt("bar.opt", dst, src)
        self.assertEqual(dst.getvalue(), "")

        src.seek(0)

        extract_build_opt("foo.opt", dst, src)
        self.assertEqual("-fno-builtin\n-UFOOBAR\n", dst.getvalue())

    def testMultiple(self):
        src = io.StringIO(
            r"""
#include <cstdio>
int foo() { return 111; }

/*@ create-file:foo.opt @
-ffoo
*/
#define INTERMIXED_DATA
/*@create-file:foo.opt:debug@
-fbaz
*/

/*@create-file:bar.opt:debug@
-DUNUSED
*/
/*@create-file:foo.opt@
-mbar
*/

/*@create-file:bar.opt@
-DALSO_UNUSED
*/


"""
        )

        dst = io.StringIO()
        extract_build_opt("foo.opt", dst, src)

        self.assertEqual("-ffoo\n-mbar\n", dst.getvalue())

    def testInvalidSignature(self):
        src = io.StringIO(
            r"""
#pragma once

/*@create-file:foo.opt@
-fanalyzer
*/
// ordinary c++ code

const char GlobalVariable[] = /*@ hello world @*/
const char WriteMkbuildopts[] = /*@create-file:foo.opt@*/

/*@make-file:bar.opt@
-mforce-l32
*/

/* nothing to look at here */
"""
        )

        dst = io.StringIO()
        with self.assertRaises(InvalidSignature) as raises:
            extract_build_opt("bar.opt", dst, src)

        self.assertFalse(dst.getvalue())

        e = raises.exception
        self.assertFalse(e.file)
        self.assertEqual(12, e.lineno)
        self.assertEqual("/*@make-file:bar.opt@\n", e.line)

    def testPartialInvalidSyntax(self):
        src = io.StringIO(
            r"""
/*@create-file:syntax.opt@
-DIMPORTANT_FLAG
-DANOTHER_FLAG=123
*/
/*@ create-file:syntax.opt @
/*@oops
-mthis-fails
*/
"""
        )

        dst = io.StringIO()
        with self.assertRaises(InvalidSyntax) as raises:
            extract_build_opt("syntax.opt", dst, src)

        self.assertFalse(dst.getvalue())

        e = raises.exception
        self.assertFalse(e.file)
        self.assertEqual(7, e.lineno)
        self.assertEqual("/*@oops\n", e.line)

    def testPartialUnclosed(self):
        src = io.StringIO(
            r"""
/*@create-file:unclosed.opt@
line 1
line 2
"""
        )
        dst = io.StringIO()
        with self.assertRaises(InvalidSyntax) as raises:
            extract_build_opt("unclosed.opt", dst, src)

        self.assertFalse(dst.getvalue())

    def testParseSignatureSpace(self):
        with buffer(
            r"""
/*@  create-file:test.opt    @
-ftest-test-test
*/
"""
        ) as (src, dst):
            extract_build_opt("test.opt", dst, src)
            self.assertEqual("-ftest-test-test\n", dst.getvalue())

        with buffer(
            r"""
/*@create-file:test.opt
@
-ftest-test-test
*/
"""
        ) as (src, dst):
            with self.assertRaises(InvalidSyntax) as raises:
                extract_build_opt("test.opt", dst, src)

            self.assertFalse(dst.getvalue())

        with buffer(
            r"""
/*@create-file:test.opt
-ftest-test-test
*/
"""
        ) as (src, dst):
            with self.assertRaises(InvalidSyntax) as raises:
                extract_build_opt("test.opt", dst, src)

            self.assertFalse(dst.getvalue())


if __name__ == "__main__":
    unittest.main()
