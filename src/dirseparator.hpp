#ifndef __DIR_SEPARATOR_HPP_
#define __DIR_SEPARATOR_HPP_

// Directory path separator
static const char DIR_SEP =
#if defined(_WIN16) | defined(_WIN32) | defined(_WIN64)
'\\'
#else
'/'
#endif
;

#endif // __DIR_SEPARATOR_HPP_