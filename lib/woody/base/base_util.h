#include <stdlib.h>     /* srand, rand */
#include <time.h>  
inline unsigned char GetRandomByte()
{
  srand (time(NULL));
  return rand() % 256;
}
