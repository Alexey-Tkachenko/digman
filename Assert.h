#ifndef _ASSERT_
#define _ASSERT_

#include "WString.h"

void prog_abort() __attribute__((noreturn));

#ifdef USE_ASSERT
extern const __FlashStringHelper* AssertMessage;
#define STR1(x) #x
#define STR(x) STR1(x)
#define ASSERT(c) c ? (void)0 : (AssertMessage = F("Assertion failed: " STR(c) " in " __FILE__ ":" STR(__LINE__)), prog_abort());
#define ASSERT_A(c) ASSERT(c)
#define FAILURE(msg) (AssertMessage = F("Failure: '" msg "' in " __FILE__ ":" STR(__LINE__)), prog_abort());
#else
#define ASSERT(c)
#define ASSERT_A(c) (void)(c)
#define FAILURE(msg) prog_abort();
#endif


#endif
