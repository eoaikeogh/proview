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

#ifndef co_status_h
#define co_status_h

#include <string>
#include "pwr.h"
extern "C" {
#include "co_msg.h"
}
using namespace std;

class co_status
{
 protected:
  pwr_tStatus m_sts;

 public:
  co_status() : m_sts(0) {}
  co_status( pwr_tStatus sts) : m_sts(sts) {}
  virtual ~co_status() {};
  pwr_tStatus sts() const { return m_sts;}
  bool evenSts() const { return EVEN(m_sts);}
  bool oddSts() const { return ODD(m_sts);}
  virtual string what() const
  { 
    char msg[200];

    msg_GetMsg( m_sts, msg, sizeof(msg));
    string s(msg);
    return s;
  }
};

#endif