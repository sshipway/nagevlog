Windows NT syslog service

Copyright © 1998-2002, SaberNet.net All rights reserved.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software Foundation,
Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307

Description: 

This program runs as a service under Windows NT 4.0 and Windows 2000.
It formats all System, Security, and Application events into a single
line and sends them to a syslog(3) host.

Example: 

Oct 18 21:37:34 test1.sabernet.net security[success] Successful Logon:  User
Name:Administrator  Domain:TEST1  Logon ID:(0x0,0x36D166)  Logon Type:7  Logon Process
:User32    Authentication Package: MICROSOFT_AUTHENTICATION_PACKAGE_V1_0  Work
station Name:TEST1 

The package is available for download at: 

http://sourceforge.net/projects/ntsyslog/ 

The source code is available via CVS at: 
http://sourceforge.net/cvs/?group_id=36242 

Installation:

Install the service by executing the following command: 

	NTsyslog -install

The service will be started automatically by the service control
manager during system startup. You can start and stop the service
manually from the Services Control Panel, or from the command line,
with the following command:

Start:	net start ntsyslog
Stop:	net stop ntsyslog

The service can be configured to run as a local user with the following rights:

	Log on as a service 
	Manage auditing and security log 

The user the service runs as can be configured in the NTsyslog Properties
page which can be accessed through the Services Control Panel. 

To remove the service run: ntsyslog -remove

Configure it by editing the registry keys properly, or by using the NtSyslogCtrl app.

The syslog host is stored in the following Registry entry: 

[HKEY_LOCAL_MACHINE\SOFTWARE\SaberNet]
"Syslog"="loghost.example.com"

An additional syslog host may be configured for redundancy: 
[HKEY_LOCAL_MACHINE\SOFTWARE\SaberNet]
"Syslog1"="logbackup.example.com"

The syslog host can be specified by domain name (loghost.example.com)
or by IP address (10.123.112.1).

The types of event log messages sent to the syslog host can be configured
by setting the dword value for each of the types of messages. All types with a
non-zero value will be processed. The included registry file enables all event
types for each event log.

The priority for each event log type controls the service and facility that
the syslog message is sent to.  Each log type has a seperate priority.
If the priority for a particular key does not exist, as if you were upgrading,
or using an old NTSyslogCtrl app, the default is 9, user.alert.

[HKEY_LOCAL_MACHINE\SOFTWARE\SaberNet\Syslog\System]
"Information"=dword:00000001
"Information Priority"==dword:00000009
"Warning"=dword:00000001
"Warning Priority"=dword:00000009
"Error"=dword:00000001
"Error Priority"=dword:00000009
"Audit Success"=dword:00000001
"Audit Success Priority"=dword:00000009
"Audit Failure"=dword:00000001
"Audit Failure Priority"=dword:00000009

[HKEY_LOCAL_MACHINE\SOFTWARE\SaberNet\Syslog\Security]
"Information"=dword:00000001
"Information Priority"==dword:00000009
"Warning"=dword:00000001
"Warning Priority"=dword:00000009
"Error"=dword:00000001
"Error Priority"=dword:00000009
"Audit Success"=dword:00000001
"Audit Success Priority"=dword:00000009
"Audit Failure"=dword:00000001
"Audit Failure Priority"=dword:00000009

[HKEY_LOCAL_MACHINE\SOFTWARE\SaberNet\Syslog\Application]
"Information"=dword:00000001
"Information Priority"==dword:00000009
"Warning"=dword:00000001
"Warning Priority"=dword:00000009
"Error"=dword:00000001
"Error Priority"=dword:00000009
"Audit Success"=dword:00000001
"Audit Success Priority"=dword:00000009
"Audit Failure"=dword:00000001
"Audit Failure Priority"=dword:00000009

The NTSyslog service must be stopped and restarted for the Registry settings to take effect. 

Synopsis:

    NTsyslog [ -install ] [ -remove ]

Options:

    -install       Installs the service

    -remove        Removes the service

Notes:

Version differences:

Older versions of this service did not allow the priority of message sent
to the syslog server to be changed, and always used 9, user.alert.

If an older version is run after having used this new version it will
continue to use user.alert, ignoring any priority set in the registry.

If an older version of the service is used with the newer version of
NTSyslogCtrl, the older service will ignore any priority set.

If an older version of NTSyslogCtrl is used with a newer service,
it will be unable to display the set priority or to change priority
of any messages sent to the server.  Use the registry or an updated
version of NTSyslogCtrl.

Calculating Priorities:

Usually, syslog refers to a "facility" and "severity".  These are
combined in to a single value called "priority".

To calculate the priorities from normal facility and severity codes:

	Take the numeric value for the facility, multiply by 8,
	and add the numeric value for the severity.

Standard facility and severity values are:

Facility:

(0) kernel
(1) user
(2) mail
(3) system
(4) security/auth 1
(5) syslog
(6) line printer
(7) news
(8) uucp
(9) clock 1
(10) security/auth 2
(11) ftp
(12) ntp
(13) log audit
(14) log alert
(15) clock 2
(16) local 0
(17) local 1
(18) local 2
(19) local 3
(20) local 4
(21) local 5
(22) local 6
(23) local 7

Severity:

(0) emergency
(1) alert
(2) critical
(3) error
(4) warning
(5) notice
(6) information
(7) debug

Note that facility 4, 9, 10, and 15 have different meaning on
various systems.  Please consult your system manual pages or
syslogd documentation.

Complete details are available in RFC 3164.  See:
http://www.ietf.org/rfc/rfc3164.txt