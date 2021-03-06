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

#ifndef cnv_xtthelpto_h
#define cnv_xtthelpto_h

#if defined __cplusplus
extern "C" {
#endif

#include "pwr.h"
#include "co_nav_help.h"
#include "cnv_readxtthelp.h"

using namespace std;

class CnvReadXtthelp;

typedef enum {
  Cnv_eXtthelpToType_Html,
  Cnv_eXtthelpToType_Xml,
  Cnv_eXtthelpToType_Ps,
  Cnv_eXtthelpToType_Pdf,
  Cnv_eXtthelpToType_Text
} Cnv_eXtthelpToType;
  
class CnvXtthelpTo {
 public:
  // CnvXtthelpTo() {}

  virtual Cnv_eXtthelpToType type() { return Cnv_eXtthelpToType_Html;}
  virtual void *insert( navh_eItemType item_type, const char *text1,
			const char *text2, const char *text3, const char *link, 
			const char *link_bookmark, const char *file_name,
			navh_eHelpFile file_type, int help_index, 
			const char *bookmark, int coding) 
    { return 0;}
  virtual bool confpass() { return false;}
  virtual void set_confpass( bool conf) {};

  virtual ~CnvXtthelpTo() {}
};

#if defined __cplusplus
}
#endif
#endif