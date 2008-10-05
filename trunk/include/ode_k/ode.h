/*************************************************************************
 *                                                                       *
 * Open Dynamics Engine, Copyright (C) 2001,2002 Russell L. Smith.       *
 * All rights reserved.  Email: russ@q12.org   Web: www.q12.org          *
 *                                                                       *
 * This library is free software; you can redistribute it and/or         *
 * modify it under the terms of EITHER:                                  *
 *   (1) The GNU Lesser General Public License as published by the Free  *
 *       Software Foundation; either version 2.1 of the License, or (at  *
 *       your option) any later version. The text of the GNU Lesser      *
 *       General Public License is included with this library in the     *
 *       file LICENSE.TXT.                                               *
 *   (2) The BSD-style license that is included with this library in     *
 *       the file LICENSE-BSD.TXT.                                       *
 *                                                                       *
 * This library is distributed in the hope that it will be useful,       *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the files    *
 * LICENSE.TXT and LICENSE-BSD.TXT for more details.                     *
 *                                                                       *
 *************************************************************************/

#ifdef __WII__
#ifndef _ODE_ODE_H_
#define _ODE_ODE_H_

/* include *everything* here */

#include <ode_k/odeconfig.h>
#include <ode_k/compatibility.h>
#include <ode_k/common.h>
#include <ode_k/odeinit.h>
#include <ode_k/contact.h>
#include <ode_k/error.h>
#include <ode_k/memory.h>
#include <ode_k/odemath.h>
#include <ode_k/matrix.h>
#include <ode_k/timer.h>
#include <ode_k/rotation.h>
#include <ode_k/mass.h>
#include <ode_k/misc.h>
#include <ode_k/objects.h>
#include <ode_k/odecpp.h>
#include <ode_k/collision_space.h>
#include <ode_k/collision.h>
#include <ode_k/odecpp_collision.h>
#include <ode_k/export-dif.h>

#endif
#else
#include <ode_k/ode.h>
#endif

