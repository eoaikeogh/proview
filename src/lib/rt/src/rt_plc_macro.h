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

#include <math.h>
#include <float.h>

#include "pwr.h"
#include "pwr_systemclasses.h"
#include "pwr_baseclasses.h"
#include "co_time.h"
#include "rt_plc.h"
#include "rt_plc_rt.h"
#include "rt_plc_dirlink.h"
#include "rt_plc_ioarea.h"
#include "rt_plc_timer.h"
#include "rt_plc_arithm.h"
#include "rt_plc_io.h"
#include "rt_plc_pid.h"
#include "rt_plc_data.h"
#include "rt_plc_drive.h"
#include "rt_plc_macro_logic.h"
#include "rt_plc_macro_io.h"
#include "rt_plc_macro_grafcet.h"
#include "rt_plc_macro_sup.h"
#include "rt_plc_macro_div.h"
#include "rt_plc_macro_mps.h"
#include "rt_plc_macro_string.h"
#include "rt_plc_macro_time.h"
#include "pwr_nmpsclasses.h"
#include "nmps_plc_macro.h"
#include "nmps.h"
#include "pwr_profibusclasses.h"
#include "pwr_remoteclasses.h"
#include "remote_plc_macro.h"
#include "pwr_basecomponentclasses.h"
#include "rt_plc_bcomp.h"
#include "ra_plc_user.h"