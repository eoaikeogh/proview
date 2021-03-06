/* 
 * Proview   Open Source Process Control.
 * Copyright (C) 2005-2017 SSAB EMEA AB.
 *
 * This file is part of Proview.
 *
 * This program is free software; you can redistribute it and/or 
 * modify it under the terms of the GNU General Public License as 
 * published by the Free Software Foundation, either version 2 of 
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful 
 * but WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the 
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License 
 * along with Proview. If not, see <http://www.gnu.org/licenses/>
 *
 * Linking Proview statically or dynamically with other modules is
 * making a combined work based on Proview. Thus, the terms and 
 * conditions of the GNU General Public License cover the whole 
 * combination.
 *
 * In addition, as a special exception, the copyright holders of
 * Proview give you permission to, from the build function in the
 * Proview Configurator, combine Proview with modules generated by the
 * Proview PLC Editor to a PLC program, regardless of the license
 * terms of these modules. You may copy and distribute the resulting
 * combined work under the terms of your choice, provided that every 
 * copy of the combined work is accompanied by a complete copy of 
 * the source code of Proview (the version used to produce the 
 * combined work), being distributed under the terms of the GNU 
 * General Public License plus this exception.
 */

/* rt_bck.c
   This module contains the methods that can be called by
   a user program, to control the backup.  */

#if defined OS_POSIX
#include <stdio.h>
#include "pwr.h"

void bck_ForceBackup (void **context)
{
    printf("bck_ForceBackup is not implemented\n");
}

pwr_tUInt32 bck_WaitBackup (
		void *context,
		pwr_tBoolean timeout)
{
    printf("bck_WaitBackup is not implemented\n");
    return 2;
}
#else

#include "pwr.h"
#include "pwr_class.h"
#include "pwr_baseclasses.h"
#include "co_time.h"
#include "rt_gdh.h"
#include "rt_gdh_msg.h"
#include "rt_bckdef.h"


/************************************************************************
*
* Name: bck_ForceBackup
*
* Type:	void
*
* Type		Parameter	IOGF	Description
* void *	context		IO	a pointer to a context block
*                                       that gets allocated by this routine
*					If NULL is supplied, no such
*					allocation takes place
* Description:	
*	The routine forces both backup cycles to be activated. This
*	activation is done in a way so the backup lists get rebuilt.
*	
*************************************************************************/

void bck_ForceBackup (
		void **context)
{
  pwr_tInt32 sts;
  pwr_tTime *t;

/*
 * Build the context block (i.e. current time)
 */

  if (context != NULL) {
    t = malloc (sizeof *t);
    time_GetTime(t);
    *context = t;
  }

/*
 * Kick the backup processes
 */

  sts = sys$ascefc (BCK_EFC, &efcname, 0, 0);
  if (EVEN (sts)) lib$signal (sts);
  sts = sys$setef (BCK_ACTIVATE);
  if (EVEN (sts)) lib$signal (sts);

} /* bck_ForceBackup */

/************************************************************************
*
* Name: bck_WaitBackup
*
* Type:	pwr_tUInt32 - status
*
* Type		Parameter	IOGF	Description
* void *	context		I	a pointer to a context block
*                                       that gets deallocated by this routine
*					This argument can optionally be
*					passed as NULL.
* pwr_tBoolean	timeout		I	Timeout wanted. This implies that
*					the WaitBackup function returns
*					a timeout error code, after 2 times
*					the slow backupcycle, if no
*					backup was done.
* Description:	
*	The routine waits for both backup cycles to be done, WITH every
*	backup object included.
*	
*************************************************************************/

pwr_tUInt32 bck_WaitBackup (
		void *context,
		pwr_tBoolean timeout)
{
  pwr_tUInt32 sts;
  pwr_tObjid objid;
  pwr_tInt32 c;
  pwr_tTime t;
  pwr_tVaxTime tmo;
  pwr_tVaxTime tmptime;
  pwr_sClass_Backup_Conf *backup_confp;	/* Backup_Conf object pointer */
  $DESCRIPTOR (timeunitdsc, "0 0:0:0.1");	/* 0.1 second units */
  int cycletime;

/*
 * Find the local Backup_Conf object
 */

  sts = gdh_GetClassList (pwr_cClass_Backup_Conf, &objid);
  while (ODD (sts)) {
    sts = gdh_ObjidToPointer (objid, (pwr_tAddress *)&backup_confp);
    if (ODD (sts)) break;
    sts = gdh_GetNextObject (objid, &objid);
  }
  if (EVEN (sts)) return sts;		/* Something wrong, quit */

/*
 * Pick up argument information
 */

  if (context == NULL) time_GetTime(&t);
  else {
    t = *(pwr_tTime *)context;
    free (context);
  }

  timed_out = FALSE;  
  sts = sys$ascefc (BCK_EFC, &efcname, 0, 0);
  if (EVEN (sts)) lib$signal (sts);			/* BUG */

  if (timeout) {
    cycletime = backup_confp->CycleSlow * 2;
    if (cycletime == 0) cycletime = BCK_DEFAULT_SLOW * 2;

    sts = sys$bintim (&timeunitdsc, &tmo);
    if (EVEN (sts)) lib$signal (sts);		/* BUG, should not happen */

    lib$mult_delta_time (
		&cycletime,		/* multiplier */
		&tmo);			/* delta_time (modified) */
    sys$gettim (&tmptime);
    lib$add_times (&tmo, &tmptime, &tmo); /* Make absolute time */

    sts = sys$setimr (BCK_WRITE_DONE, &tmo, &astrtn, 4711, 0);
    if (EVEN (sts)) lib$signal (sts);			/* BUG */
  }

/*
 * Loop, and wait for things to happen
 */

  while (TRUE) {
    sts = sys$clref (BCK_WRITE_DONE);
    if (EVEN (sts)) lib$signal (sts);			/* BUG */
    sts = sys$waitfr (BCK_WRITE_DONE);
    if (EVEN (sts)) lib$signal (sts);			/* BUG */

    /* Check for timeout */

    if (timed_out) return SS$_TIMEOUT;

    /* Check if both cycles done */

    if (time_Acomp(&backup_confp->ObjTimeSlow, &t) < 0) 
        continue;
    if (time_Acomp(&backup_confp->ObjTimeFast, &t) < 0) 
        continue;

    break;

  } /* Loop */

  return 1;	/* Done. */

} /* bck_WaitBackup */

#endif