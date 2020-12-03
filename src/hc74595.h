/*============================================================================
  
  hc74595.h

  Functions to control a 74HC595 shift register using three GPIO pins. 

  Copyright (c)1990-2020 Kevin Boone. Distributed under the terms of the
  GNU Public Licence, v3.0

  ==========================================================================*/
#pragma once

#include "defs.h"

struct HC74595;
typedef struct _HC74595 HC74595;

BEGIN_DECLS

/** Initialize the HC74595 object with the numbers of the three GPIO
    pins that will be used. Note that this method only stores values, 
    and will always succeed. */
HC74595  *hc74595_create (int data_pin, int shift_pin, int latch_pin);

/** Clean up the object. This method implicitly calls _uninit(). */
void      hc74595_destroy (HC74595 *self);

/** Initialize the object. This opens a bunch of file handles for the
    sysfs files for the various GPIO pins. Consequently, the method
    can fail. If it does, and *error is not NULL, then it is written with
    and error message that the caller should free. If this method 
    succeeds, _uninit() should be called in due course to clean up. */ 
BOOL      hc74595_init (HC74595 *self, char **error);

/** Clean up. In principle, this operation can fail, as it involves sysfs
    operations. But what can we do if this happens? Probably nothing, so no
    errors are reported. */
void      hc74595_uninit (HC74595 *self);

/** Set a byte value into the shift register. This method has no error
    return, because if we've successfully called _init(), it's highly 
    unlikely it can fail. */
void      hc74595_set (HC74595 *aself, BYTE val);

END_DECLS
