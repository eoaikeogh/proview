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
 **/

#include "sev_db.h"
#include "sev_dbms.h"
#include "sev_dbsqlite.h"
#include "sev_dbhdf5.h"
#include "co_syi.h"
#include "pwr_names.h"
#include "rt_load.h"
#include "co_dcli.h"
#include "rt_sev_msg.h"


sev_db *sev_db::open_database( sev_eDbType type)
{

  if ( type == sev_eDbType_Mysql) {
#if defined PWRE_CONF_MYSQL
    return sev_dbms::open_database();
#else
    printf( "** Release is not built with mysql\n");
    return 0;
#endif
  }
  else if ( type == sev_eDbType_Sqlite) {
#if defined PWRE_CONF_SQLITE3
    return sev_dbsqlite::open_database();
#else
    printf( "** Release is not built with sqlite3\n");
    return 0;
#endif
  }
  else if ( type == sev_eDbType_HDF5) {
#if defined PWRE_CONF_HDF5
    return sev_dbhdf5::open_database();
#else
    printf( "** Release is not built with HDF5\n");
    return 0;
#endif
  }
  else
    return 0;
}

int sev_db::get_systemname( char *name)
{
  FILE 	*file;
  pwr_tFileName	fname;
  char  nodename[40];
  char	*bus_str;
  int bus;
  char line[200];
  pwr_tStatus sts;
  static char system_name[80] = "";

  if ( strcmp( system_name, "") == 0) {

    syi_NodeName( &sts, nodename, sizeof(nodename));
    if ( EVEN(sts)) return 0;
    
    bus_str = getenv( "PWR_BUS_ID");
    if ( !bus_str)
      return 0;
    if ( sscanf( bus_str, "%d", &bus) != 1)
      return 0;
    
    sprintf( fname, pwr_cNameBoot, load_cDirectory, cdh_Low(nodename), bus);
    dcli_translate_filename( fname, fname);
    file = fopen( fname, "r");
    if ( file == 0) {
      printf("In %s row %d:\n", __FILE__, __LINE__);
      printf("** Warning, systemname not found\n");
      return 0;
    }

    if ( !dcli_read_line( line, sizeof(line), file))
      return 0;
    if ( !dcli_read_line( line, sizeof(line), file))
      return 0;

    strcpy( system_name, line);
  }
  strcpy( name, system_name);
  return 1;
}

void sev_db::get_item_idx( pwr_tStatus *sts, unsigned int *item_idx, pwr_tOid oid, char *attributename)
{
  for ( unsigned int i = 0; i < m_items.size(); i++) {
    if ( m_items[i].oid.oix == oid.oix && m_items[i].oid.vid == oid.vid && 
	 (cdh_NoCaseStrcmp( attributename, m_items[i].attr[0].aname) == 0)) {
      *item_idx = i;
      *sts = SEV__SUCCESS;
      return;
    }      
  }
  *sts = SEV__NOSUCHITEM;
}

