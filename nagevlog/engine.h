/*-----------------------------------------------------------------------------
 *
 *  engine.h - Event processing engine
 *
 *    Copyright (c) 1998, SaberNet.net - All rights reserved
 *
 *    This program is free software; you can redistribute it and/or
 *    modify it under the terms of the GNU General Public License
 *    as published by the Free Software Foundation; either version 2
 *    of the License, or (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307
 *
 *  $Id: engine.h,v 1.9 2002/06/27 21:05:44 jason Exp $
 *
 *----------------------------------------------------------------------------*/
#ifndef _ENGINE_H_
#define _ENGINE_H_

#include "event.h"

void engine_init();
int  engine_process_event(ntsl_event *event);
void engine_crontab();
void engine_shutdown();

#endif
