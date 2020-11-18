#ifndef _SYS__TZ_STRUCTS_H_
#define _SYS__TZ_STRUCTS_H_

typedef struct __tzrule_struct
{
  char ch;
  int m; /* Month of year if ch=M */
  int n; /* Week of month if ch=M */
  int d; /* Day of week if ch=M, day of year if ch=J or ch=D */
  int s; /* Time of day in seconds */
  time_t change;
  long offset; /* Match type of _timezone. */
} __tzrule_type;

typedef struct __tzinfo_struct
{
  int __tznorth;
  int __tzyear;
  __tzrule_type __tzrule[2];
} __tzinfo_type;

__tzinfo_type *__gettzinfo (void);

#endif /* _SYS__TZ_STRUCTS_H_ */
