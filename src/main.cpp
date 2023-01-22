#include "enet/enet.h"
#include <iostream>
#include <stdio.h>

using std::cout;
using std::endl;


int main()
{
  if (enet_initialize () != 0)
  {
    fprintf (stderr, "An error occurred while initializing ENet.\n");
    return EXIT_FAILURE;
  }
  atexit(enet_deinitialize);

  return 0;
}
