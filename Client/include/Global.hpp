#ifndef XAL_GLOBAL_HPP
#define XAL_GLOBAL_HPP

#define DEBUG

#define LOG 0
/* LOG LEVELS
 * 0 - All output
 */

#ifdef DEBUG
    #include <iostream>
#endif

#if LOG == 0
    inline void Debug(const std::string& message) { std::cout << message << "\n"; }
#else
    inline void Debug(const std::string& message) {}
#endif

#endif
