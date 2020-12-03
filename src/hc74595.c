/*==========================================================================
  
    hc74595.c

    Implementation of the HC74595 "class" that is specified in 
    hc74595.h

    Copyright (c)2020 Kevin Boone, GPL v3.0

============================================================================*/
#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <assert.h>
#include <pthread.h>
#include <fcntl.h>
#include "defs.h" 
#include "gpiopin.h" 
#include "hc74595.h" 

struct _HC74595
  {
  int data_pin;
  int latch_pin;
  int shift_pin;
  GPIOPin *pin_data;
  GPIOPin *pin_latch;
  GPIOPin *pin_shift;
  BOOL ready;
  };

/*============================================================================
  hc74595_create
============================================================================*/
HC74595  *hc74595_create (int data_pin, int shift_pin, int latch_pin)
  {
  HC74595 *self = malloc (sizeof (HC74595));
  memset (self, 0, sizeof (HC74595));
  self->data_pin = data_pin;
  self->shift_pin = shift_pin;
  self->latch_pin = latch_pin;
  self->pin_data = gpiopin_create (self->data_pin);
  self->pin_latch = gpiopin_create (self->latch_pin);
  self->pin_shift = gpiopin_create (self->shift_pin);
  return self;
  }


/*============================================================================
  hc74595_destroy
============================================================================*/
void hc74595_destroy (HC74595 *self)
  {
  if (self)
    {
    gpiopin_destroy (self->pin_latch);
    gpiopin_destroy (self->pin_data);
    gpiopin_destroy (self->pin_shift);
    hc74595_uninit (self);
    free (self);
    }
  }

/*============================================================================
  hc74595_init
============================================================================*/
BOOL hc74595_init (HC74595 *self, char **error)
  {
  assert (self != NULL);
  BOOL ret = FALSE;
  if (gpiopin_init (self->pin_data, error))
    {
    // If initializing one pin works, we'll assume the others will, too
    gpiopin_init (self->pin_shift, NULL);
    gpiopin_init (self->pin_latch, NULL);
    // Start with all the pins in the high state -- the 74HC595 is
    //  signalled by low-going pulses.
    gpiopin_set (self->pin_data, HIGH);
    gpiopin_set (self->pin_shift, HIGH);
    gpiopin_set (self->pin_latch, HIGH);
    self->ready = TRUE;
    ret = TRUE;
    }
  return ret;
  }

/*============================================================================
  hc74595_uninit
============================================================================*/
void hc74595_uninit (HC74595 *self)
  {
  assert (self != NULL);
  gpiopin_uninit (self->pin_latch);
  gpiopin_uninit (self->pin_data);
  gpiopin_uninit (self->pin_shift);
  self->ready = FALSE;
  }

/*============================================================================

  hc74595_set

  This is where the real work happens (but it's not complicated).
  We shift out the bits of the supplied value, one by one, until all
  8 bits have been processed. As we do this, we set the bit value to
  the data pin, then take the shift pin low for 10 usec, then back
  high. Each rising edge of the shift pin causes the value of the
  data pin to be shifted into the register. 

  The 8-shift cycle is signalled by setting the latch pin low at the
  start of the cycle, and then back high at the end. 

  NB: the Pi can't time down to 10 usec. The smallest wait that can
  be provided by usleep() is about 100 usec (at least on the 3B+ -- maybe
  the Pi 4 is faster in this respec). My point, though, is that there's
  no point trying to reduce the usleep times in this function -- any value
  smaller than about 100 is the same. 

  This this method takes a little under 1 msec to complete.

============================================================================*/
void hc74595_set (HC74595 *self, BYTE val)
  {
  assert (self != NULL);
  assert (self->ready);

  // Start the write cycle by setting the latch pin low.
  // Writing the data can start after this.
  gpiopin_set (self->pin_latch, LOW);

  for (int i = 0; i < 8; i++)
      {
      // Set the bit we are currently proessing onto the data pin, then...
      gpiopin_set (self->pin_data, val & 0x01); 
      // Toggle the shift pin low for ten msec
      gpiopin_set (self->pin_shift, LOW);
      usleep (10);
      gpiopin_set (self->pin_shift, HIGH);
      // Allow a short time for everything to settle before shifting in
      //  the next bit
      usleep (10);
      // Make the next bit available in the LSB of the value
      val >>= 1;
      }
 
  // Set the latch pin high, indicating that all data has been 
  //  supplied.
  gpiopin_set (self->pin_latch, HIGH);
  }

