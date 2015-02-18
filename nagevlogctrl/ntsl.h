/*-----------------------------------------------------------------------------
 *
 *  ntsl.h - Common definitions and types
 *
 *    Copyright (c) 1998, SaberNet.net - All rights reserved
      Also (c) 2004 S.Shipway 
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
 *  $Id: ntsl.h,v 1.14 2002/09/20 06:12:47 jason Exp jason $
 *
 *----------------------------------------------------------------------------*/
#ifndef _NTSL_H_
#define _NTSL_H_

#include <windows.h>
/*#include "error.h"*/
#include "safestr.h"

#define VERSION_MAJOR	    	"1"
#define VERSION_MINOR			"6"
#define COPYRIGHT   			"(C) S.Shipway 2004,2005, built on GPL code (C) Sabernet.net 2002 and (C) Ethan Galstead"
#define APP_NAME    			"NagiosEventLog"
#define SERVICE_NAME			"NagiosEventLog"
#define SERVICE_EXE				"nagevlog.exe"
#define DEV_NAME   	 			"Cheshire Cat"

#define NTSL_NAME_LEN			32
#define NTSL_DESC_LEN			80
#define NTSL_SYS_LEN			256
#define NTSL_DATE_LEN           16
#define NTSL_EVENT_LEN          1024
#define NTSL_PATH_LEN			1024
#define NTSL_PASSWD_LEN			64
#define NTSL_SVC_LEN			64
#define NTSL_LOG_DIR			"log"	

/*------------------------[ portable type definitions ]-----------------------*/
#ifndef uchar
#define uchar  UCHAR
#endif
#ifndef uint16
#define uint16 WORD
#endif
#ifndef uint32
#define uint32 DWORD
#endif
#ifndef int16
#define int16  SHORT
#endif
#ifndef int32
#define int32  LONG
#endif
#ifndef int64
#define int64  DWORDLONG
#endif 
#ifndef bool
#define bool   BOOL
#endif
#ifndef true
#define true   TRUE
#endif
#ifndef false
#define false  FALSE
#endif

/*---------------------------------[ globals ]--------------------------------*/
void		 ntsl_log_info(char *format, ...);
void		 ntsl_log_warning(char *format, ...);
void		 ntsl_log_error(char *format, ...);
void		 ntsl_log_msg(uint16 etype, char *format, ...);
void		 ntsl_die(char *format, ...);
int			 ntsl_check_dir(char *dir, int relative);
void		 ntsl_init();
void		 ntsl_run();
void		 ntsl_shutdown();

#endif
