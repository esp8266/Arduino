import unittest

from restyle import changed_files_for_diff

# small git-diff samples from https://queirozf.com/entries/git-diff-reference-and-examples


class BaseTest(unittest.TestCase):
    def testNewLine(self):
        diff = """
diff --git a/file.txt b/file.txt
index 257cc56..3bd1f0e 100644
--- a/file.txt
+++ b/file.txt
@@ -1 +1,2 @@
 foo
+bar
"""
        changed = changed_files_for_diff(diff)
        self.assertEqual(1, len(changed))
        self.assertEqual("file.txt", changed[0].file)
        self.assertEqual(1, len(changed[0].lines))
        self.assertEqual(1, changed[0].lines[0])

        expected = """
--- a/file.txt
+++ b/file.txt
@@ -1 +1,2 @@
 foo
+bar
""".strip()
        self.assertEqual(expected, changed[0].hunk.strip())

    def testNewLines(self):
        diff = """
diff --git a/file.txt b/file.txt
index 257cc56..3bd1f0e 100644
--- a/file2.txt
+++ b/file2.txt
@@ -1 +1,2 @@
 foo
+bar
 baz
@@ -1 +10,2 @@
 222
-222
 333
@@ -1 +100,3 @@
 aaa
+bbb
+ccc
 ddd
"""
        changed = changed_files_for_diff(diff)
        self.assertEqual(1, len(changed))
        self.assertEqual("file2.txt", changed[0].file)

        lines = changed[0].lines
        self.assertEqual(3, len(lines))

        first, second, third = lines
        self.assertEqual(1, first)
        self.assertEqual(10, second)
        self.assertEqual(100, third)

        expected = """
--- a/file2.txt
+++ b/file2.txt
@@ -1 +1,2 @@
 foo
+bar
 baz
@@ -1 +10,2 @@
 222
-222
 333
@@ -1 +100,3 @@
 aaa
+bbb
+ccc
 ddd
""".strip()
        self.assertEqual(expected, changed[0].hunk.strip())

    def testRemovedLineAndDeletedFile(self):
        diff = """
diff --git a/file.txt b/file.txt
index 3bd1f0e..257cc56 100644
--- a/file.txt
+++ b/file.txt
@@ -1,2 +1 @@
 foo
-bar
diff --git a/file2.txt b/file2.txt
deleted file mode 100644
index 85553e8..0000000
--- a/file2.txt
+++ /dev/null
@@ -1,2 +0,0 @@
-aaaaaa
-bbbbbb
"""
        changed = changed_files_for_diff(diff)
        self.assertEqual(1, len(changed))
        self.assertEqual("file.txt", changed[0].file)
        self.assertEqual(1, len(changed[0].lines))
        self.assertEqual(1, changed[0].lines[0])

        expected = """
--- a/file.txt
+++ b/file.txt
@@ -1,2 +1 @@
 foo
-bar
""".strip()
        self.assertEqual(expected, changed[0].hunk.strip())

    def testNewLineAndDeletedFile(self):
        diff = """
diff --git a/file.txt b/file.txt
index 3bd1f0e..86e041d 100644
--- a/file.txt
+++ b/file.txt
@@ -1,2 +1,3 @@
 foo
 bar
+baz
diff --git a/file2.txt b/file2.txt
deleted file mode 100644
index 85553e8..0000000
--- a/file2.txt
+++ /dev/null
@@ -1,2 +0,0 @@
-aaaaaa
-bbbbbb
"""
        changed = changed_files_for_diff(diff)
        self.assertEqual(1, len(changed))
        self.assertEqual("file.txt", changed[0].file)
        self.assertEqual(1, len(changed[0].lines))
        self.assertEqual(1, changed[0].lines[0])

        expected = """
--- a/file.txt
+++ b/file.txt
@@ -1,2 +1,3 @@
 foo
 bar
+baz
""".strip()
        self.assertEqual(expected, changed[0].hunk.strip())

    def testDeletedFile(self):
        diff = """
diff --git a/file2.txt b/file2.txt
deleted file mode 100644
index 85553e8..0000000
--- a/file2.txt
+++ /dev/null
@@ -1,2 +0,0 @@
-aaaaaa
-bbbbbb
"""
        changed = changed_files_for_diff(diff)
        self.assertEqual(0, len(changed))

    def testNewFile(self):
        diff = """
diff --git a/file3.txt b/file3.txt
new file mode 100644
index 0000000..a309e46
--- /dev/null
+++ b/file3.txt
@@ -0,0 +1 @@
+this is file3
"""
        changed = changed_files_for_diff(diff)
        self.assertEqual(0, len(changed))


if __name__ == '__main__':
    unittest.main()
