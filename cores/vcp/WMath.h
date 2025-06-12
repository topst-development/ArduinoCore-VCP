#ifndef _WIRING_MATH_
#define _WIRING_MATH_

// #ifdef __cplusplus
// extern "C" {
// #endif

// void randomSeed(unsigned long);
// long random(long);
// long random(long, long)

// long map(long, long, long, long, long);

// unsigned int makeWord(unsigned int);
// unsigned int makeWord(unsigned char, unsigned char);

// extern long random(long) ;
// extern long random(long, long) ;
extern void randomSeed(uint32_t dwSeed) ;
extern long map(long, long, long, long, long) ;

// extern uint16_t makeWord(uint16_t w) ;
extern uint16_t makeWord(uint8_t h, uint8_t l) ;

#define word(...) makeWord(__VA_ARGS__)


// #ifdef __cplusplus
// }
// #endif

#endif /* _WIRING_MATH_ */