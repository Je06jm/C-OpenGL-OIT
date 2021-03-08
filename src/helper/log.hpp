#pragma once

#include <string>

/* Prints a formatted string into the console if it's a
 * debug build. It's used just like printf
 * @param format The string to be formatted
 * @param ... Additional arguments
 */
void info(const char *format, ...);

/* Prints a formatted string into the console on unix
 * OS types. On Windows, a message box pops up with the
 * formatted string. The program then exits with a code
 * of 1
 * @param format The string to be formatted
 * @param ... Additional arguments
 */
void critical(const char *format, ...);
