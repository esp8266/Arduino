#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>



#define memcmp memcmp_P
#define memcpy memcpy_P
#define memmem memmem_P
#define memchr memchr_P
#define strcat strcat_P
#define strncat strncat_P
#define strcpy strcpy_P
#define strncpy strncpy_P
#define strlen strlen_P
#define strnlen strnlen_P
#define strcmp strcmp_P
#define strncmp strncmp_P


_CONST char *it = "<UNSET>";	/* Routine name for message routines. */
static int  errors = 0;

/* Complain if condition is not true.  */
#define check(thing) checkit(thing, __LINE__)

static void
_DEFUN(checkit,(ok,l),
       int ok _AND
       int l )

{
//  newfunc(it);
//  line(l);
  
  if (!ok)
  {
    printf("string.c:%d %s\n", l, it);
    ++errors;
  }
}



/* Complain if first two args don't strcmp as equal.  */
#define equal(a, b)  funcqual(a,b,__LINE__);

static void
_DEFUN(funcqual,(a,b,l),
       char *a _AND
       char *b _AND
       int l)
{
//  newfunc(it);
  
//  line(l);
  if (a == NULL && b == NULL) return;
  if (strcmp(a,b)) {
      printf("string.c:%d (%s)\n", l, it);  
    }
}



static char one[50];
static char two[50];


void libm_test_string()
{
  /* Test strcmp first because we use it to test other things.  */
  it = "strcmp";
  check(strcmp("", "") == 0); /* Trivial case. */
  check(strcmp("a", "a") == 0); /* Identity. */
  check(strcmp("abc", "abc") == 0); /* Multicharacter. */
  check(strcmp("abc", "abcd") < 0); /* Length mismatches. */
  check(strcmp("abcd", "abc") > 0);
  check(strcmp("abcd", "abce") < 0);	/* Honest miscompares. */
  check(strcmp("abce", "abcd") > 0);
  check(strcmp("a\103", "a") > 0); /* Tricky if char signed. */
  check(strcmp("a\103", "a\003") > 0);

  /* Test strcpy next because we need it to set up other tests.  */
  it = "strcpy";
  check(strcpy(one, "abcd") == one);	/* Returned value. */
  equal(one, "abcd");	/* Basic test. */

  (void) strcpy(one, "x");
  equal(one, "x");		/* Writeover. */
  equal(one+2, "cd");	/* Wrote too much? */

  (void) strcpy(two, "hi there");
  (void) strcpy(one, two);
  equal(one, "hi there");	/* Basic test encore. */
  equal(two, "hi there");	/* Stomped on source? */

  (void) strcpy(one, "");
  equal(one, "");		/* Boundary condition. */

  /* strcat.  */
  it = "strcat";
  (void) strcpy(one, "ijk");
  check(strcat(one, "lmn") == one); /* Returned value. */
  equal(one, "ijklmn");	/* Basic test. */

  (void) strcpy(one, "x");
  (void) strcat(one, "yz");
  equal(one, "xyz");		/* Writeover. */
  equal(one+4, "mn");	/* Wrote too much? */

  (void) strcpy(one, "gh");
  (void) strcpy(two, "ef");
  (void) strcat(one, two);
  equal(one, "ghef");	/* Basic test encore. */
  equal(two, "ef");		/* Stomped on source? */

  (void) strcpy(one, "");
  (void) strcat(one, "");
  equal(one, "");		/* Boundary conditions. */
  (void) strcpy(one, "ab");
  (void) strcat(one, "");
  equal(one, "ab");
  (void) strcpy(one, "");
  (void) strcat(one, "cd");
  equal(one, "cd");

  /* strncat - first test it as strcat, with big counts,
     then test the count mechanism.  */
  it = "strncat";
  (void) strcpy(one, "ijk");
  check(strncat(one, "lmn", 99) == one); /* Returned value. */
  equal(one, "ijklmn");	/* Basic test. */

  (void) strcpy(one, "x");
  (void) strncat(one, "yz", 99);
  equal(one, "xyz");		/* Writeover. */
  equal(one+4, "mn");	/* Wrote too much? */

  (void) strcpy(one, "gh");
  (void) strcpy(two, "ef");
  (void) strncat(one, two, 99);
  equal(one, "ghef");	/* Basic test encore. */
  equal(two, "ef");		/* Stomped on source? */

  (void) strcpy(one, "");
  (void) strncat(one, "", 99);
  equal(one, "");		/* Boundary conditions. */
  (void) strcpy(one, "ab");
  (void) strncat(one, "", 99);
  equal(one, "ab");
  (void) strcpy(one, "");
  (void) strncat(one, "cd", 99);
  equal(one, "cd");

  (void) strcpy(one, "ab");
  (void) strncat(one, "cdef", 2);
  equal(one, "abcd");	/* Count-limited. */

  (void) strncat(one, "gh", 0);
  equal(one, "abcd");	/* Zero count. */

  (void) strncat(one, "gh", 2);
  equal(one, "abcdgh");	/* Count _AND length equal. */
  it = "strncmp";
  /* strncmp - first test as strcmp with big counts";*/
  check(strncmp("", "", 99) == 0); /* Trivial case. */
  check(strncmp("a", "a", 99) == 0);	/* Identity. */
  check(strncmp("abc", "abc", 99) == 0); /* Multicharacter. */
  check(strncmp("abc", "abcd", 99) < 0); /* Length unequal. */
  check(strncmp("abcd", "abc",99) > 0);
  check(strncmp("abcd", "abce", 99) < 0); /* Honestly unequal. */
  check(strncmp("abce", "abcd",99)>0);
  check(strncmp("abce", "abcd", 3) == 0); /* Count limited. */
  check(strncmp("abce", "abc", 3) == 0); /* Count == length. */
  check(strncmp("abcd", "abce", 4) < 0); /* Nudging limit. */
  check(strncmp("abc", "def", 0) == 0); /* Zero count. */

  /* strncpy - testing is a bit different because of odd semantics.  */
  it = "strncpy";
  check(strncpy(one, "abc", 4) == one); /* Returned value. */
  equal(one, "abc");		/* Did the copy go right? */

  (void) strcpy(one, "abcdefgh");
  (void) strncpy(one, "xyz", 2);
  equal(one, "xycdefgh");	/* Copy cut by count. */

  (void) strcpy(one, "abcdefgh");
  (void) strncpy(one, "xyz", 3); /* Copy cut just before NUL. */
  equal(one, "xyzdefgh");

  (void) strcpy(one, "abcdefgh");
  (void) strncpy(one, "xyz", 4); /* Copy just includes NUL. */
  equal(one, "xyz");
  equal(one+4, "efgh");	/* Wrote too much? */

  (void) strcpy(one, "abcdefgh");
  (void) strncpy(one, "xyz", 5); /* Copy includes padding. */
  equal(one, "xyz");
  equal(one+4, "");
  equal(one+5, "fgh");

  (void) strcpy(one, "abc");
  (void) strncpy(one, "xyz", 0); /* Zero-length copy. */
  equal(one, "abc");	

  (void) strncpy(one, "", 2);	/* Zero-length source. */
  equal(one, "");
  equal(one+1, "");	
  equal(one+2, "c");

  (void) strcpy(one, "hi there");
  (void) strncpy(two, one, 9);
  equal(two, "hi there");	/* Just paranoia. */
  equal(one, "hi there");	/* Stomped on source? */

  /* strlen.  */
  it = "strlen";
  check(strlen("") == 0);	/* Empty. */
  check(strlen("a") == 1);	/* Single char. */
  check(strlen("abcd") == 4); /* Multiple chars. */

  /* strchr.  */
  it = "strchr";
  check(strchr("abcd", 'z') == NULL); /* Not found. */
  (void) strcpy(one, "abcd");
  check(strchr(one, 'c') == one+2); /* Basic test. */
  check(strchr(one, 'd') == one+3); /* End of string. */
  check(strchr(one, 'a') == one); /* Beginning. */
  check(strchr(one, '\0') == one+4);	/* Finding NUL. */
  (void) strcpy(one, "ababa");
  check(strchr(one, 'b') == one+1); /* Finding first. */
  (void) strcpy(one, "");
  check(strchr(one, 'b') == NULL); /* Empty string. */
  check(strchr(one, '\0') == one); /* NUL in empty string. */

  /* index - just like strchr.  */
  it = "index";
  check(index("abcd", 'z') == NULL);	/* Not found. */
  (void) strcpy(one, "abcd");
  check(index(one, 'c') == one+2); /* Basic test. */
  check(index(one, 'd') == one+3); /* End of string. */
  check(index(one, 'a') == one); /* Beginning. */
  check(index(one, '\0') == one+4); /* Finding NUL. */
  (void) strcpy(one, "ababa");
  check(index(one, 'b') == one+1); /* Finding first. */
  (void) strcpy(one, "");
  check(index(one, 'b') == NULL); /* Empty string. */
  check(index(one, '\0') == one); /* NUL in empty string. */

  /* strrchr.  */
  it = "strrchr";
  check(strrchr("abcd", 'z') == NULL); /* Not found. */
  (void) strcpy(one, "abcd");
  check(strrchr(one, 'c') == one+2);	/* Basic test. */
  check(strrchr(one, 'd') == one+3);	/* End of string. */
  check(strrchr(one, 'a') == one); /* Beginning. */
  check(strrchr(one, '\0') == one+4); /* Finding NUL. */
  (void) strcpy(one, "ababa");
  check(strrchr(one, 'b') == one+3);	/* Finding last. */
  (void) strcpy(one, "");
  check(strrchr(one, 'b') == NULL); /* Empty string. */
  check(strrchr(one, '\0') == one); /* NUL in empty string. */

  /* rindex - just like strrchr.  */
  it = "rindex";
  check(rindex("abcd", 'z') == NULL); /* Not found. */
  (void) strcpy(one, "abcd");
  check(rindex(one, 'c') == one+2); /* Basic test. */
  check(rindex(one, 'd') == one+3); /* End of string. */
  check(rindex(one, 'a') == one); /* Beginning. */
  check(rindex(one, '\0') == one+4);	/* Finding NUL. */
  (void) strcpy(one, "ababa");
  check(rindex(one, 'b') == one+3); /* Finding last. */
  (void) strcpy(one, "");
  check(rindex(one, 'b') == NULL); /* Empty string. */
  check(rindex(one, '\0') == one); /* NUL in empty string. */

  /* strpbrk - somewhat like strchr.  */
  it = "strpbrk";
  check(strpbrk("abcd", "z") == NULL); /* Not found. */
  (void) strcpy(one, "abcd");
  check(strpbrk(one, "c") == one+2);	/* Basic test. */
  check(strpbrk(one, "d") == one+3);	/* End of string. */
  check(strpbrk(one, "a") == one); /* Beginning. */
  check(strpbrk(one, "") == NULL); /* Empty search list. */
  check(strpbrk(one, "cb") == one+1); /* Multiple search. */
  (void) strcpy(one, "abcabdea");
  check(strpbrk(one, "b") == one+1);	/* Finding first. */
  check(strpbrk(one, "cb") == one+1); /* With multiple search. */
  check(strpbrk(one, "db") == one+1); /* Another variant. */
  (void) strcpy(one, "");
  check(strpbrk(one, "bc") == NULL); /* Empty string. */
  check(strpbrk(one, "") == NULL); /* Both strings empty. */

  /* strstr - somewhat like strchr.  */
  it = "strstr";
  check(strstr("z", "abcd") == NULL); /* Not found. */
  check(strstr("abx", "abcd") == NULL); /* Dead end. */
  (void) strcpy(one, "abcd");
  check(strstr(one,"c") == one+2); /* Basic test. */
  check(strstr(one, "bc") == one+1);	/* Multichar. */
  check(strstr(one,"d") == one+3); /* End of string. */
  check(strstr(one,"cd") == one+2);	/* Tail of string. */
  check(strstr(one,"abc") == one); /* Beginning. */
  check(strstr(one,"abcd") == one);	/* Exact match. */
  check(strstr(one,"de") == NULL);	/* Past end. */
  check(strstr(one,"") == one); /* Finding empty. */
  (void) strcpy(one, "ababa");
  check(strstr(one,"ba") == one+1); /* Finding first. */
  (void) strcpy(one, "");
  check(strstr(one, "b") == NULL); /* Empty string. */
  check(strstr(one,"") == one); /* Empty in empty string. */
  (void) strcpy(one, "bcbca");
  check(strstr(one,"bca") == one+2); /* False start. */
  (void) strcpy(one, "bbbcabbca");
  check(strstr(one,"bbca") == one+1); /* With overlap. */

  /* strspn.  */
  it = "strspn";
  check(strspn("abcba", "abc") == 5); /* Whole string. */
  check(strspn("abcba", "ab") == 2);	/* Partial. */
  check(strspn("abc", "qx") == 0); /* None. */
  check(strspn("", "ab") == 0); /* Null string. */
  check(strspn("abc", "") == 0); /* Null search list. */

  /* strcspn.  */
  it = "strcspn";
  check(strcspn("abcba", "qx") == 5); /* Whole string. */
  check(strcspn("abcba", "cx") == 2); /* Partial. */
  check(strcspn("abc", "abc") == 0);	/* None. */
  check(strcspn("", "ab") == 0); /* Null string. */
  check(strcspn("abc", "") == 3); /* Null search list. */

  /* strtok - the hard one.  */
  it = "strtok";
  (void) strcpy(one, "first, second, third");
  equal(strtok(one, ", "), "first");	/* Basic test. */
  equal(one, "first");
  equal(strtok((char *)NULL, ", "), "second");
  equal(strtok((char *)NULL, ", "), "third");
  check(strtok((char *)NULL, ", ") == NULL);
  (void) strcpy(one, ", first, ");
  equal(strtok(one, ", "), "first");	/* Extra delims, 1 tok. */
  check(strtok((char *)NULL, ", ") == NULL);
  (void) strcpy(one, "1a, 1b; 2a, 2b");
  equal(strtok(one, ", "), "1a"); /* Changing delim lists. */
  equal(strtok((char *)NULL, "; "), "1b");
  equal(strtok((char *)NULL, ", "), "2a");
  (void) strcpy(two, "x-y");
  equal(strtok(two, "-"), "x"); /* New string before done. */
  equal(strtok((char *)NULL, "-"), "y");
  check(strtok((char *)NULL, "-") == NULL);
  (void) strcpy(one, "a,b, c,, ,d");
  equal(strtok(one, ", "), "a"); /* Different separators. */
  equal(strtok((char *)NULL, ", "), "b");
  equal(strtok((char *)NULL, " ,"), "c"); /* Permute list too. */
  equal(strtok((char *)NULL, " ,"), "d");
  check(strtok((char *)NULL, ", ") == NULL);
  check(strtok((char *)NULL, ", ") == NULL); /* Persistence. */
  (void) strcpy(one, ", ");
  check(strtok(one, ", ") == NULL);	/* No tokens. */
  (void) strcpy(one, "");
  check(strtok(one, ", ") == NULL);	/* Empty string. */
  (void) strcpy(one, "abc");
  equal(strtok(one, ", "), "abc"); /* No delimiters. */
  check(strtok((char *)NULL, ", ") == NULL);
  (void) strcpy(one, "abc");
  equal(strtok(one, ""), "abc"); /* Empty delimiter list. */
  check(strtok((char *)NULL, "") == NULL);
  (void) strcpy(one, "abcdefgh");
  (void) strcpy(one, "a,b,c");
  equal(strtok(one, ","), "a"); /* Basics again... */
  equal(strtok((char *)NULL, ","), "b");
  equal(strtok((char *)NULL, ","), "c");
  check(strtok((char *)NULL, ",") == NULL);
  equal(one+6, "gh");	/* Stomped past end? */
  equal(one, "a");		/* Stomped old tokens? */
  equal(one+2, "b");
  equal(one+4, "c");

  /* memcmp.  */
  it = "memcmp";
  check(memcmp("a", "a", 1) == 0); /* Identity. */
  check(memcmp("abc", "abc", 3) == 0); /* Multicharacter. */
  check(memcmp("abcd", "abce", 4) < 0); /* Honestly unequal. */
  check(memcmp("abce", "abcd",4));
  check(memcmp("alph", "beta", 4) < 0);
  check(memcmp("abce", "abcd", 3) == 0); /* Count limited. */
  check(memcmp("abc", "def", 0) == 0); /* Zero count. */

  /* memcmp should test strings as unsigned */
  one[0] = 0xfe;
  two[0] = 0x03;
  check(memcmp(one, two,1) > 0);
  
  
  /* memchr.  */
  it = "memchr";
  check(memchr("abcd", 'z', 4) == NULL); /* Not found. */
  (void) strcpy(one, "abcd");
  check(memchr(one, 'c', 4) == one+2); /* Basic test. */
  check(memchr(one, 'd', 4) == one+3); /* End of string. */
  check(memchr(one, 'a', 4) == one);	/* Beginning. */
  check(memchr(one, '\0', 5) == one+4); /* Finding NUL. */
  (void) strcpy(one, "ababa");
  check(memchr(one, 'b', 5) == one+1); /* Finding first. */
  check(memchr(one, 'b', 0) == NULL); /* Zero count. */
  check(memchr(one, 'a', 1) == one);	/* Singleton case. */
  (void) strcpy(one, "a\203b");
  check(memchr(one, 0203, 3) == one+1); /* Unsignedness. */

  /* memcpy - need not work for overlap.  */
  it = "memcpy";
  check(memcpy(one, "abc", 4) == one); /* Returned value. */
  equal(one, "abc");		/* Did the copy go right? */

  (void) strcpy(one, "abcdefgh");
  (void) memcpy(one+1, "xyz", 2);
  equal(one, "axydefgh");	/* Basic test. */

  (void) strcpy(one, "abc");
  (void) memcpy(one, "xyz", 0);
  equal(one, "abc");		/* Zero-length copy. */

  (void) strcpy(one, "hi there");
  (void) strcpy(two, "foo");
  (void) memcpy(two, one, 9);
  equal(two, "hi there");	/* Just paranoia. */
  equal(one, "hi there");	/* Stomped on source? */
#if 0
  /* memmove - must work on overlap.  */
  it = "memmove";
  check(memmove(one, "abc", 4) == one); /* Returned value. */
  equal(one, "abc");		/* Did the copy go right? */

  (void) strcpy(one, "abcdefgh");
  (void) memmove(one+1, "xyz", 2);
  equal(one, "axydefgh");	/* Basic test. */

  (void) strcpy(one, "abc");
  (void) memmove(one, "xyz", 0);
  equal(one, "abc");		/* Zero-length copy. */

  (void) strcpy(one, "hi there");
  (void) strcpy(two, "foo");
  (void) memmove(two, one, 9);
  equal(two, "hi there");	/* Just paranoia. */
  equal(one, "hi there");	/* Stomped on source? */

  (void) strcpy(one, "abcdefgh");
  (void) memmove(one+1, one, 9);
  equal(one, "aabcdefgh");	/* Overlap, right-to-left. */

  (void) strcpy(one, "abcdefgh");
  (void) memmove(one+1, one+2, 7);
  equal(one, "acdefgh");	/* Overlap, left-to-right. */

  (void) strcpy(one, "abcdefgh");
  (void) memmove(one, one, 9);
  equal(one, "abcdefgh");	/* 100% overlap. */
#endif
#if 0
  /* memccpy - first test like memcpy, then the search part
     The SVID, the only place where memccpy is mentioned, says
     overlap might fail, so we don't try it.  Besides, it's hard
     to see the rationale for a non-left-to-right memccpy.  */
  it = "memccpy";
  check(memccpy(one, "abc", 'q', 4) == NULL); /* Returned value. */
  equal(one, "abc");		/* Did the copy go right? */

  (void) strcpy(one, "abcdefgh");
  (void) memccpy(one+1, "xyz", 'q', 2);
  equal(one, "axydefgh");	/* Basic test. */

  (void) strcpy(one, "abc");
  (void) memccpy(one, "xyz", 'q', 0);
  equal(one, "abc");		/* Zero-length copy. */

  (void) strcpy(one, "hi there");
  (void) strcpy(two, "foo");
  (void) memccpy(two, one, 'q', 9);
  equal(two, "hi there");	/* Just paranoia. */
  equal(one, "hi there");	/* Stomped on source? */

  (void) strcpy(one, "abcdefgh");
  (void) strcpy(two, "horsefeathers");
  check(memccpy(two, one, 'f', 9) == two+6);	/* Returned value. */
  equal(one, "abcdefgh");	/* Source intact? */
  equal(two, "abcdefeathers"); /* Copy correct? */

  (void) strcpy(one, "abcd");
  (void) strcpy(two, "bumblebee");
  check(memccpy(two, one, 'a', 4) == two+1); /* First char. */
  equal(two, "aumblebee");
  check(memccpy(two, one, 'd', 4) == two+4); /* Last char. */
  equal(two, "abcdlebee");
  (void) strcpy(one, "xyz");
  check(memccpy(two, one, 'x', 1) == two+1); /* Singleton. */
  equal(two, "xbcdlebee");
#endif
  /* memset.  */
  it = "memset";
  (void) strcpy(one, "abcdefgh");
  check(memset(one+1, 'x', 3) == one+1); /* Return value. */
  equal(one, "axxxefgh");	/* Basic test. */

  (void) memset(one+2, 'y', 0);
  equal(one, "axxxefgh");	/* Zero-length set. */

  (void) memset(one+5, 0, 1);
  equal(one, "axxxe");	/* Zero fill. */
  equal(one+6, "gh");	/* _AND the leftover. */

  (void) memset(one+2, 010045, 1);
  equal(one, "ax\045xe");	/* Unsigned char convert. */

  /* bcopy - much like memcpy.
     Berklix manual is silent about overlap, so don't test it.  */
  it = "bcopy";
  (void) bcopy("abc", one, 4);
  equal(one, "abc");		/* Simple copy. */

  (void) strcpy(one, "abcdefgh");
  (void) bcopy("xyz", one+1, 2);
  equal(one, "axydefgh");	/* Basic test. */

  (void) strcpy(one, "abc");
  (void) bcopy("xyz", one, 0);
  equal(one, "abc");		/* Zero-length copy. */

  (void) strcpy(one, "hi there");
  (void) strcpy(two, "foo");
  (void) bcopy(one, two, 9);
  equal(two, "hi there");	/* Just paranoia. */
  equal(one, "hi there");	/* Stomped on source? */

  /* bzero.  */
  it = "bzero";
  (void) strcpy(one, "abcdef");
  bzero(one+2, 2);
  equal(one, "ab");		/* Basic test. */
  equal(one+3, "");
  equal(one+4, "ef");

  (void) strcpy(one, "abcdef");
  bzero(one+2, 0);
  equal(one, "abcdef");	/* Zero-length copy. */

  /* bcmp - somewhat like memcmp.  */
  it = "bcmp";
  check(bcmp("a", "a", 1) == 0); /* Identity. */
  check(bcmp("abc", "abc", 3) == 0);	/* Multicharacter. */
  check(bcmp("abcd", "abce", 4) != 0); /* Honestly unequal. */
  check(bcmp("abce", "abcd",4));
  check(bcmp("alph", "beta", 4) != 0);
  check(bcmp("abce", "abcd", 3) == 0); /* Count limited. */
  check(bcmp("abc", "def", 0) == 0);	/* Zero count. */

  if (errors) abort();
  printf("ok\n");

#if 0  /* strerror - VERY system-dependent.  */
{
  extern CONST unsigned int _sys_nerr;
  extern CONST char *CONST _sys_errlist[];
  int f;
  it = "strerror";
  f = open("/", O_WRONLY);	/* Should always fail. */
  check(f < 0 && errno > 0 && errno < _sys_nerr);
  equal(strerror(errno), _sys_errlist[errno]);
}
#endif
}

