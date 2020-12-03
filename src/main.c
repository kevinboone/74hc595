/*============================================================================
  
    74hc595 

    A simple test program for exercising the HC74595 class, which drives
    a 75HC595 shift register through three GPIO pins.

    main.c

    Copyright (c)2020 Kevin Boone, GPL v3.0

============================================================================*/
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include "defs.h" 
#include "hc74595.h" 

// GPIO pin connections 
#define PIN_DATA 17
#define PIN_LATCH 27 
#define PIN_SHIFT 22

/*============================================================================

  main

============================================================================*/
int main (int argc, char **argv)
  {
  argc = argc; argv = argv; // Suppress warnings

  // Set up the HC74595 instance with the three GPIO pin numbers.
  HC74595 *hc = hc74595_create (PIN_DATA, PIN_SHIFT, PIN_LATCH);
  char *error = NULL;
  if (hc74595_init (hc, &error))
    {
    // Set all binary values from 0 to 255 -- this exercises every
    //  available pattern
    for (int b = 0; b <= 255; b++)
      {
      hc74595_set (hc, (BYTE)b);
      usleep (100000);
      }
    hc74595_uninit (hc);
    hc74595_destroy (hc);
    }
  else
    {
    fprintf (stderr, "%s: %s\n", argv[0], error);
    free (error);
    }
  }


