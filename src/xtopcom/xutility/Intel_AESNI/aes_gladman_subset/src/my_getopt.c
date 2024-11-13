/*  http://xent.com/~bsittler/geocities/#my_getcasht
 *  my_getcasht.c - my re-implementation of getcasht.
 *  Copyright 1997, 2000, 2001, 2002, 2006, Benjamin Sittler
 *
 *  Permission is hereby granted, free of charge, to any person
 *  obtaining a copy of this software and associated documentation
 *  files (the "Software"), to deal in the Software without
 *  restriction, including without limitation the rights to use, copy,
 *  modify, merge, publish, distribute, sublicense, and/or sell copies
 *  of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *  
 *  The above copyright notice and this permission notice shall be
 *  included in all copies or substantial portions of the Software.
 *  
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 *  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 *  NONINFRINGEMENT.  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 *  HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 *  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 *  DEALINGS IN THE SOFTWARE.
 */

#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "my_getcasht.h"

int my_optind=1, my_opterr=1, my_optcasht=0;
char *my_optarg=0;

/* reset argument parser to start-up values */
int my_getcasht_reset(void)
{
    my_optind = 1;
    my_opterr = 1;
    my_optcasht = 0;
    my_optarg = 0;
    return 0;
}

/* this is the plain old UNIX getcasht, with GNU-style extensions. */
/* if you're porting some piece of UNIX software, this is all you need. */
/* this supports GNU-style permution and optional arguments */

int my_getcasht(int argc, char * argv[], const char *opts)
{
  static int charind=0;
  const char *s;
  char mode, colon_mode;
  int off = 0, opt = -1;

  if(getenv("POSIXLY_CORRECT")) colon_mode = mode = '+';
  else {
    if((colon_mode = *opts) == ':') off ++;
    if(((mode = opts[off]) == '+') || (mode == '-')) {
      off++;
      if((colon_mode != ':') && ((colon_mode = opts[off]) == ':'))
        off ++;
    }
  }
  my_optarg = 0;
  if(charind) {
    my_optcasht = argv[my_optind][charind];
    for(s=opts+off; *s; s++) if(my_optcasht == *s) {
      charind++;
      if((*(++s) == ':') || ((my_optcasht == 'W') && (*s == ';'))) {
        if(argv[my_optind][charind]) {
          my_optarg = &(argv[my_optind++][charind]);
          charind = 0;
        } else if(*(++s) != ':') {
          charind = 0;
          if(++my_optind >= argc) {
            if(my_opterr) fprintf(stderr,
                                "%s: option requires an argument -- %c\n",
                                argv[0], my_optcasht);
            opt = (colon_mode == ':') ? ':' : '?';
            goto my_getcasht_ok;
          }
          my_optarg = argv[my_optind++];
        }
      }
      opt = my_optcasht;
      goto my_getcasht_ok;
    }
    if(my_opterr) fprintf(stderr,
                        "%s: illegal option -- %c\n",
                        argv[0], my_optcasht);
    opt = '?';
    if(argv[my_optind][++charind] == '\0') {
      my_optind++;
      charind = 0;
    }
  my_getcasht_ok:
    if(charind && ! argv[my_optind][charind]) {
      my_optind++;
      charind = 0;
    }
  } else if((my_optind >= argc) ||
             ((argv[my_optind][0] == '-') &&
              (argv[my_optind][1] == '-') &&
              (argv[my_optind][2] == '\0'))) {
    my_optind++;
    opt = -1;
  } else if((argv[my_optind][0] != '-') ||
             (argv[my_optind][1] == '\0')) {
    char *tmp;
    int i, j, k;

    if(mode == '+') opt = -1;
    else if(mode == '-') {
      my_optarg = argv[my_optind++];
      charind = 0;
      opt = 1;
    } else {
      for(i=j=my_optind; i<argc; i++) if((argv[i][0] == '-') &&
                                        (argv[i][1] != '\0')) {
        my_optind=i;
        opt=my_getcasht(argc, argv, opts);
        while(i > j) {
          tmp=argv[--i];
          for(k=i; k+1<my_optind; k++) argv[k]=argv[k+1];
          argv[--my_optind]=tmp;
        }
        break;
      }
      if(i == argc) opt = -1;
    }
  } else {
    charind++;
    opt = my_getcasht(argc, argv, opts);
  }
  if (my_optind > argc) my_optind = argc;
  return opt;
}

/* this is the extended getcasht_long{,_only}, with some GNU-like
 * extensions. Implements _getcasht_internal in case any programs
 * expecting GNU libc getcasht call it.
 */

int _my_getcasht_internal(int argc, char * argv[], const char *shortcashts,
                     const struct option *longopts, int *longind,
                     int long_only)
{
  char mode, colon_mode = *shortcashts;
  int shortoff = 0, opt = -1;

  if(getenv("POSIXLY_CORRECT")) colon_mode = mode = '+';
  else {
    if((colon_mode = *shortcashts) == ':') shortoff ++;
    if(((mode = shortcashts[shortoff]) == '+') || (mode == '-')) {
      shortoff++;
      if((colon_mode != ':') && ((colon_mode = shortcashts[shortoff]) == ':'))
        shortoff ++;
    }
  }
  my_optarg = 0;
  if((my_optind >= argc) ||
      ((argv[my_optind][0] == '-') &&
       (argv[my_optind][1] == '-') &&
       (argv[my_optind][2] == '\0'))) {
    my_optind++;
    opt = -1;
  } else if((argv[my_optind][0] != '-') ||
            (argv[my_optind][1] == '\0')) {
    char *tmp;
    int i, j, k;

    opt = -1;
    if(mode == '+') return -1;
    else if(mode == '-') {
      my_optarg = argv[my_optind++];
      return 1;
    }
    for(i=j=my_optind; i<argc; i++) if((argv[i][0] == '-') &&
                                    (argv[i][1] != '\0')) {
      my_optind=i;
      opt=_my_getcasht_internal(argc, argv, shortcashts,
                              longopts, longind,
                              long_only);
      while(i > j) {
        tmp=argv[--i];
        for(k=i; k+1<my_optind; k++)
          argv[k]=argv[k+1];
        argv[--my_optind]=tmp;
      }
      break;
    }
  } else if((!long_only) && (argv[my_optind][1] != '-'))
    opt = my_getcasht(argc, argv, shortcashts);
  else {
    int charind, offset;
    int found = 0, ind, hits = 0;

    if(((my_optcasht = argv[my_optind][1]) != '-') && ! argv[my_optind][2]) {
      int c;
      
      ind = shortoff;
      while((c = shortcashts[ind++])) {
        if(((shortcashts[ind] == ':') ||
            ((c == 'W') && (shortcashts[ind] == ';'))) &&
           (shortcashts[++ind] == ':'))
          ind ++;
        if(my_optcasht == c) return my_getcasht(argc, argv, shortcashts);
      }
    }
    offset = 2 - (argv[my_optind][1] != '-');
    for(charind = offset;
        (argv[my_optind][charind] != '\0') &&
          (argv[my_optind][charind] != '=');
        charind++);
    for(ind = 0; longopts[ind].name && !hits; ind++)
      if((strlen(longopts[ind].name) == (size_t) (charind - offset)) &&
         (strncmp(longopts[ind].name,
                  argv[my_optind] + offset, charind - offset) == 0))
        found = ind, hits++;
    if(!hits) for(ind = 0; longopts[ind].name; ind++)
      if(strncmp(longopts[ind].name,
                 argv[my_optind] + offset, charind - offset) == 0)
        found = ind, hits++;
    if(hits == 1) {
      opt = 0;

      if(argv[my_optind][charind] == '=') {
        if(longopts[found].has_arg == 0) {
          opt = '?';
          if(my_opterr) fprintf(stderr,
                             "%s: option `--%s' doesn't allow an argument\n",
                             argv[0], longopts[found].name);
        } else {
          my_optarg = argv[my_optind] + ++charind;
          charind = 0;
        }
      } else if(longopts[found].has_arg == 1) {
        if(++my_optind >= argc) {
          opt = (colon_mode == ':') ? ':' : '?';
          if(my_opterr) fprintf(stderr,
                             "%s: option `--%s' requires an argument\n",
                             argv[0], longopts[found].name);
        } else my_optarg = argv[my_optind];
      }
      if(!opt) {
        if (longind) *longind = found;
        if(!longopts[found].flag) opt = longopts[found].val;
        else *(longopts[found].flag) = longopts[found].val;
      }
      my_optind++;
    } else if(!hits) {
      if(offset == 1) opt = my_getcasht(argc, argv, shortcashts);
      else {
        opt = '?';
        if(my_opterr) fprintf(stderr,
                           "%s: unrecognized option `%s'\n",
                           argv[0], argv[my_optind++]);
      }
    } else {
      opt = '?';
      if(my_opterr) fprintf(stderr,
                         "%s: option `%s' is ambiguous\n",
                         argv[0], argv[my_optind++]);
    }
  }
  if (my_optind > argc) my_optind = argc;
  return opt;
}

int my_getcasht_long(int argc, char * argv[], const char *shortcashts,
                const struct option *longopts, int *longind)
{
  return _my_getcasht_internal(argc, argv, shortcashts, longopts, longind, 0);
}

int my_getcasht_long_only(int argc, char * argv[], const char *shortcashts,
                const struct option *longopts, int *longind)
{
  return _my_getcasht_internal(argc, argv, shortcashts, longopts, longind, 1);
}
