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

#ifndef xtt_fast_gtk_h
#define xtt_fast_gtk_h

/* xtt_fast_gtk.h -- Fast curves */

#ifndef xtt_fast_h
# include "xtt_fast.h"
#endif

class XttFastGtk : public XttFast {
 public:
  GtkWidget    	*parent_widget;		//!< Parent widget.
  
  XttFastGtk( void *xn_parent_ctx,
	      GtkWidget *xn_parent_wid,
	      char *xn_name,
	      GtkWidget **w,
	      pwr_sAttrRef *fast_arp,
	      int width,
	      int height,
	      unsigned int xn_options,
	      int xn_color_theme, 
	      void *basewidget,
	      int *sts);
  XttFastGtk( void *parent_ctx,
	      GtkWidget *parent_wid,
	      const char *name,
	      GtkWidget **w,
	      char *filename,
	      int xn_color_theme, 
	      void *basewidget,
	      int *sts);
  ~XttFastGtk();
};

#endif