#ifndef ROPE_UTIL_DEBUG
#define ROPE_UTIL_DEBUG

#ifdef DEBUG

#define __debug__(arg)  arg

#else

// These macros trick the compiler to replace all calls to the debug functions by 
// a "//" which is just a blank comment, removing these calls from the source code
// and thus saving

#define __debug__(arg) ;

#endif // DEBUG

#endif // ROPE_UTIL_DEBUG
