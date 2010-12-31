//#include <check.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "regex.h"
#include "utils.h"
#include "output.h"

#ifdef MEMWATCH
  #include "memwatch.h"
#endif

int8_t verbose = P_NONE;


#define VERBOSE 1

static int test = 0;

#ifdef VERBOSE
  #define check( A ) \
    { \
        ++test; \
        if( A ){ \
            fprintf( stderr, "PASS test #%d (%s, %d)\n", test, __FILE__, __LINE__ ); \
        } else { \
            fprintf( stderr, "FAIL test #%d (%s, %d)\n", test, __FILE__, __LINE__ ); \
            return test; \
        } \
    }
#else
  #define check( A ) \
    { \
        ++test; \
        if( !( A ) ){ \
            fprintf( stderr, "FAIL test #%d (%s, %d)\n", test, __FILE__, __LINE__ ); \
            return test; \
        } \
    }
#endif

void testIsRegexMatch(void) {
  check(isRegExMatch(NULL, NULL) == 0);
  check(isRegExMatch(NULL, "test") == 0);
  check(isRegExMatch("test", NULL) == 0);
  check(isRegExMatch("test", "adc") == 0);
  check(isRegExMatch("test", "test") == 1);
  check(isRegExMatch("(a|b)c", "test") == 0);
  check(isRegExMatch("(a|b)c", "abc") == 1);
  check(isRegExMatch("(a|b)c", "acd") == 1);
  check(isRegExMatch("(a|b)c", "adc") == 0);
  check(isRegExMatch("(?!.*abc)def.*", "def abc") == 0);
  check(isRegExMatch("(?!.*abc)def.*", "def ghi") == 1);
  check(isRegExMatch("def.*ghi", "def xyz (ghi - rst)") == 1);
  check(isRegExMatch("def.*ghi", "def xyz (abc - ghi - rst)") == 1);
  check(isRegExMatch("def.*abc", "def xyz (ghi - rst)") == 0);
  check(isRegExMatch("def.*abc", "def xyz (abc - ghi - rst)") == 1);
  check(isRegExMatch("�dy", "�dy") == 1);
}


void testGetMatch(void) {
  const char* pattern1 = "Content-Disposition:\\s(inline|attachment);\\s*filename=\"?(.+?)\"?;?\\r?\\n?$";
  const char* pattern2 = "\"result\":\\s\"(.+)\"\n";
  const char* string1 = "Content-Disposition: inline; filename=\"this.is.a.test-file.torrent\"";
  const char* string2 = "Content-Disposition: inline; filename=this.is.a.test-file.torrent";
  const char* string3 = "\"result\": \"success\"";
  char *res_str = NULL;

  res_str = getRegExMatch(NULL, NULL, 0);
  check(res_str == NULL);

  res_str = getRegExMatch(pattern1, NULL, 0);
  check(res_str == NULL);

  res_str = getRegExMatch(NULL, string1, 0);
  check(res_str == NULL);

  res_str = getRegExMatch(pattern1, string1, 7);
  check(res_str == NULL);

  res_str = getRegExMatch(pattern1, string1, 2);
  check(strcmp(res_str, "this.is.a.test-file.torrent") == 0);
  am_free(res_str);

  res_str = getRegExMatch(pattern1, string2, 2);
  check(strcmp(res_str, "this.is.a.test-file.torrent") == 0);
  am_free(res_str);

  res_str = getRegExMatch(pattern1, string2, 2);
  check(res_str == NULL);
  am_free(res_str);

  res_str = getRegExMatch(pattern2, string3, 2);
  check(res_str == NULL);
  am_free(res_str);

  res_str = getRegExMatch(pattern2, string3, 1);
  check(strcmp(res_str, "success") == 0);
  am_free(res_str);

  res_str = getRegExMatch(pattern2, "\"result\": \"failure\"", 1);
  check(strcmp(res_str, "failure") == 0);
  am_free(res_str);

  res_str = getRegExMatch(pattern2, "\"result\": \"duplicate torrent\"", 1);
  check(strcmp(res_str, "duplicate torrent") == 0);
  am_free(res_str);
}

int main(void) {
  testIsRegexMatch();
  testGetMatch();
  return 0;
}
