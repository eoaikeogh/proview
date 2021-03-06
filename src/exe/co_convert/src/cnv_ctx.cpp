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


#include <stdio.h>
#include <float.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>

extern "C" {
#include "pwr.h"
#include "co_dcli.h"
#include "co_cdh.h"
}

#include "cnv_ctx.h"

using namespace std;

char *CnvCtx::low( const char *in)
{
  static char str[400];

  cdh_ToLower( str, in);
  return str;
}

int CnvCtx::remove_spaces(
			const char *in,
			char	*out)
{
  char    *s, *s1;

  for ( s = (char *)in; !((*s == 0) || ((*s != ' ') && (*s != 9))); s++);

  // strcpy( out, s);
  for ( s1 = out; *s; s++,s1++)
    *s1 = *s;
  *s1 = 0;
        
  s = out;
  if ( strlen(s) != 0) {
    for ( s += strlen(s) - 1; 
          !((s == out) || ((*s != ' ') && (*s != 9))); s--) ;
    s++;
    *s = 0;
  }

  return 1;
}

int CnvCtx::read_line(
			char	*line,
			int	maxsize,
			FILE	*file)
{ 
  char	*s;

  if (fgets( line, maxsize, file) == NULL)
    return 0;
  line[maxsize-1] = 0;
  s = strchr( line, 10);
  if ( s != 0)
    *s = 0;
  s = strchr( line, 13);
  if ( s != 0)
    *s = 0;

  return 1;
}

void CnvCtx::add_depend( char *fname)
{
  if ( strcmp( depend_filename, "") == 0)
    return;

  string fnamestr( fname);
  depend.push_back(fnamestr);
}

void CnvCtx::print_depend()
{
  if ( strcmp( depend_filename, "") == 0)
    return;

  dcli_translate_filename( dependfile, dependfile);
  dcli_translate_filename( depend_filename, depend_filename);

  ofstream fp( depend_filename);
  
  fp << dependfile << " : \\" << endl;
  for ( unsigned int i = 0; i < depend.size(); i++)
    fp << depend[i] << " \\" << endl;

  fp << endl;
  fp.close();
}