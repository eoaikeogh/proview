/** 
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


#include <stdlib.h>
#include "glow_utils.h"

static void  qsort_swp(int, char *, char *);

void glow_qsort(
  void *basearg, 
  size_t nel, 
  size_t width, 
  int (* compar)(const void*, const void*)
)
{
  int     gap,ngap,i,j;
  int     t1,t2;
  char    *jd;
  char    *base = basearg;

  t1 = nel * width;
  for(ngap=nel/2; ngap > 0; ngap = ngap / 2)
  {
    gap = ngap * width;
    t2 = gap + width;
    jd = base + gap;
    for(i=t2; i <= t1; i += width)
      for(j = i - t2 ; j >= 0 ; j -= gap)
      {
        if ((*compar)(base + j, jd + j) <= 0)
          break;
        qsort_swp(width, base + j, jd + j);
      }
  }
}

static void qsort_swp(int w, char *a, char *b)
{
  char    tmp;

  while(w--)
  {
    tmp = *a;
    *a++ = *b;
    *b++ = tmp;
  }
}