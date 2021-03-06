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

/* wb_c_pndevice.c -- work bench methods of the PnDevice class. */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <vector>
#include <map>

#include "pwr.h"
#include "pwr_baseclasses.h"
#include "pwr_profibusclasses.h"
#include "wb_env.h"
#include "flow.h"
#include "flow_ctx.h"
#include "flow_api.h"
#include "flow_browctx.h"
#include "flow_browapi.h"

#include "co_msg.h"
#include "co_cdh.h"
#include "co_dcli.h"
#include "cow_wow.h"
#include "cow_msgwindow.h"
#include "cow_xhelp.h"
#include "cow_pn_gsdml.h"
#include "cow_pn_gsdml_attr.h"
#include "cow_pn_gsdml_attrnav.h"
#include "wb_c_pndevice.h"

#include "wb_pwrs.h"
#include "wb_ldh_msg.h"
#include "wb_ldh.h"
#include "wb_pwrb_msg.h"
#include "rt_pb_msg.h"
#include "wb_wnav.h"

using namespace std;

static pwr_tStatus generate_viewer_data(device_sCtx *ctx);

class ChanItem {
 public:
  ChanItem() : subslot_number(0), representation(0), number(0), use_as_bit(0), cid(0) {}

  unsigned int subslot_number;
  unsigned int representation;
  unsigned int number;
  unsigned int use_as_bit;
  pwr_tCid cid;
  char description[80];
};

static int pndevice_add_channels( device_sCtx *ctx, gsdml_VirtualSubmoduleItem *vi, int subslot_number,
				  vector<ChanItem>& input_vect, vector<ChanItem>& output_vect);
static int pndevice_check_io( device_sCtx *ctx, gsdml_VirtualSubmoduleList *vsl, 
			      vector<ChanItem>& input_vect, vector<ChanItem>& output_vect);

/*----------------------------------------------------------------------------*\
  Configure the slave from gsd file.
\*----------------------------------------------------------------------------*/

static void get_subcid( ldh_tSession ldhses, pwr_tCid cid, vector<pwr_tCid>& v)
{
  pwr_tCid subcid;
  pwr_tStatus sts;

  for ( sts = ldh_GetSubClass( ldhses, cid, pwr_cNCid, &subcid);
	ODD(sts);
	sts = ldh_GetSubClass( ldhses, cid, subcid, &subcid)) {
    v.push_back( subcid);
    get_subcid( ldhses, subcid, v);
  }
}

int pndevice_help_cb( void *sctx, const char *text)
{
  pwr_tCmd cmd;
  device_sCtx *ctx = (device_sCtx *)sctx;

  strcpy( cmd, "help ");
  strcat( cmd, text);
  return ((WNav *)ctx->editor_ctx)->command( cmd);
}

void pndevice_close_cb( void *sctx)
{
  device_sCtx *ctx = (device_sCtx *)sctx;
  delete ctx->attr;
  delete ctx->gsdml;
  free( (char *)ctx);
}

int pndevice_save_cb( void *sctx)
{
  device_sCtx *ctx = (device_sCtx *)sctx;
  pwr_tStatus sts;
  pwr_tOName name;
  int size;
  pwr_tOid oid;
  pwr_tStatus rsts = PB__SUCCESS;
  
  // Syntax check
  if ( ctx->attr->attrnav->device_num == 0) {
    MsgWindow::message( 'E', "Device type not selected");
    return PB__SYNTAX;
  }

  for ( unsigned int i = 1; i < ctx->attr->attrnav->dev_data.slot_data.size(); i++) {
    if ( ctx->attr->attrnav->dev_data.slot_data[i]->module_enum_number == 0 &&
	 ctx->attr->attrnav->dev_data.slot_data[i]->module_class != 0) {
      // Module class selected but not module type
      char msg[20];

      sprintf( msg, "Slot %d", i);
      MsgWindow::message( 'E', "Module type not selected, ", msg);
      rsts = PB__MODULETYPE;
    }
    if ( ctx->attr->attrnav->dev_data.slot_data[i]->module_class == 0 &&
	 ctx->attr->attrnav->dev_data.slot_data[i]->module_enum_number != 0) {
      // Module type selected but not module class
      char msg[20];

      sprintf( msg, "Slot %d", i);
      MsgWindow::message( 'E', "Module class not selected, ", msg);
      rsts = PB__MODULECLASS;
    }
  }

  // Save configuration
  ((WNav *)ctx->editor_ctx)->set_nodraw();

  sts = ldh_ObjidToName(ctx->ldhses, ctx->aref.Objid, 
			ldh_eName_Hierarchy, name, sizeof(name), &size);
  if ( EVEN(sts)) goto return_now;

  // Check that Slot attribute corresponds to the and module_oid
  for ( unsigned int i = 1; i < ctx->attr->attrnav->dev_data.slot_data.size(); i++)
    ctx->attr->attrnav->dev_data.slot_data[i]->module_oid = pwr_cNOid;

  for ( sts = ldh_GetChild( ctx->ldhses, ctx->aref.Objid, &oid);
	ODD(sts);
	sts = ldh_GetNextSibling( ctx->ldhses, oid, &oid)) {
    unsigned int *slotnumberp, slotnumber;

    sts = ldh_GetObjectPar( ctx->ldhses, oid, "RtBody",
			    "Slot", (char **)&slotnumberp, &size);
    if ( EVEN(sts)) {
      MsgWindow::message( 'E', "Not a Profinet module object", msgw_ePop_Yes, oid);
      continue;
    }
    slotnumber = *slotnumberp;
    free( slotnumberp);

    if ( slotnumber >= ctx->attr->attrnav->dev_data.slot_data.size()) {
      MsgWindow::message( 'E', "Slot too large", msgw_ePop_Yes, oid);
      continue;
    }

    if ( cdh_ObjidIsNotNull( ctx->attr->attrnav->dev_data.slot_data[slotnumber]->module_oid)) {
      MsgWindow::message( 'E', "Slot already used", msgw_ePop_Yes, oid);
      continue;
    }

    if ( ctx->attr->attrnav->dev_data.slot_data[slotnumber]->module_class == pwr_cNCid)
      // Should be removed
      continue;


    ctx->attr->attrnav->dev_data.slot_data[slotnumber]->module_oid = oid;
  }

  // Remove modules that wasn't configured any more
  pwr_tOid moid[100];
  int mcnt;
  int found;
  mcnt = 0;
  for ( sts = ldh_GetChild( ctx->ldhses, ctx->aref.Objid, &oid);
	ODD(sts);
	sts = ldh_GetNextSibling( ctx->ldhses, oid, &oid)) {
    found = 0;
    for ( unsigned int i = 0; i < ctx->attr->attrnav->dev_data.slot_data.size(); i++) {
      if ( cdh_ObjidIsEqual( ctx->attr->attrnav->dev_data.slot_data[i]->module_oid, oid)) {
	found = 1;
	break;
      }
    }
    if ( !found) {
      moid[mcnt++] = oid;
      if ( mcnt > (int) (sizeof(moid)/sizeof(moid[0])))
	break;	 
    }
  }

  for ( int i = 0; i < mcnt; i++)
    sts = ldh_DeleteObjectTree( ctx->ldhses, moid[i], 0);

  // Create new module objects
  for ( unsigned int i = 0; i < ctx->attr->attrnav->dev_data.slot_data.size(); i++) {
    GsdmlSlotData *slot = ctx->attr->attrnav->dev_data.slot_data[i];

    if ( cdh_ObjidIsNull( slot->module_oid) && slot->module_class != pwr_cNCid) {
      char mname[20];
      sprintf( mname, "M%d", i);

      if ( i == 1)
	sts = ldh_CreateObject( ctx->ldhses, &slot->module_oid, mname, slot->module_class,
				ctx->aref.Objid, ldh_eDest_IntoFirst);
      else {	
	// Find sibling
	pwr_tOid dest_oid = pwr_cNOid;
	int dest_found = 0;
	for ( int j = i - 1; j > 0; j--) {
	  if ( cdh_ObjidIsNotNull( ctx->attr->attrnav->dev_data.slot_data[j]->module_oid)) {
	    dest_oid = ctx->attr->attrnav->dev_data.slot_data[j]->module_oid;
	    dest_found = 1;
	    break;
	  }	    
	}
	if ( !dest_found)
	  sts = ldh_CreateObject( ctx->ldhses, &slot->module_oid, mname, slot->module_class,
				  ctx->aref.Objid, ldh_eDest_IntoFirst);
	else
	  sts = ldh_CreateObject( ctx->ldhses, &slot->module_oid, mname, slot->module_class,
				  dest_oid, ldh_eDest_After);
      }
      if ( EVEN(sts)) {
	MsgWindow::message( 'E', "Error creating module object", mname);
	sts = 0;
	goto return_now;
      }

      pwr_tAttrRef aaref;
      pwr_tAttrRef modulearef = cdh_ObjidToAref( slot->module_oid);

      // Set Slot
      pwr_tUInt32 slotnumber = i;
      sts = ldh_ArefANameToAref( ctx->ldhses, &modulearef, "Slot", &aaref);
      if ( EVEN(sts)) goto return_now;
	    
      sts = ldh_WriteAttribute( ctx->ldhses, &aaref, &slotnumber, sizeof(slotnumber));
      if ( EVEN(sts)) goto return_now;
    }
  }

  for ( unsigned int i = 0; i < ctx->attr->attrnav->dev_data.slot_data.size(); i++) {
    GsdmlSlotData *slot = ctx->attr->attrnav->dev_data.slot_data[i];
    
    if ( i == 0) {
      vector<ChanItem> input_vect;
      vector<ChanItem> output_vect;

      sts = pndevice_check_io( ctx, ctx->attr->attrnav->device_item->VirtualSubmoduleList,
			       input_vect, output_vect);
      if ( sts == PB__CREATECHAN) {
	char msg[20];
	sprintf( msg, "Slot %d", i);
	MsgWindow::message( 'W', "Unexpected datatype, channel not created, ", msg);
      }
    }
    else {
      if ( slot->module_class == pwr_cClass_PnModule) {
	vector<ChanItem> input_vect;
	vector<ChanItem> output_vect;
	gsdml_UseableModules *um = ctx->gsdml->ApplicationProcess->DeviceAccessPointList->
	  DeviceAccessPointItem[ctx->attr->attrnav->device_num-1]->UseableModules;

	if ( !um)
	  continue;
	gsdml_ModuleItem *mi = (gsdml_ModuleItem *)um->
	  ModuleItemRef[slot->module_enum_number-1]->Body.ModuleItemTarget.p;

	sts = pndevice_check_io( ctx, mi->VirtualSubmoduleList, input_vect, output_vect);
	if ( sts == PB__CREATECHAN) {
	  char msg[20];
	  sprintf( msg, "Slot %d", i);
	  MsgWindow::message( 'W', "Unexpected datatype, channel not created, ", msg);
	}

	// Create the channels
	if ( EVEN(ldh_GetChild( ctx->ldhses, slot->module_oid, &oid))) {
	  unsigned int chan_cnt = 0;
	  for ( unsigned int j = 0; j < input_vect.size(); j++) {
	    char name[80];
	    sprintf( name, "Ch%02u", chan_cnt++);
	    sts = ldh_CreateObject( ctx->ldhses, &oid, name, input_vect[j].cid,
				    slot->module_oid, ldh_eDest_IntoLast);
	    if ( EVEN(sts)) goto return_now;

	    pwr_tAttrRef aaref;
	    pwr_tAttrRef chanaref = cdh_ObjidToAref( oid);

	    // Set Representation
	    pwr_tEnum representation = input_vect[j].representation;
	    sts = ldh_ArefANameToAref( ctx->ldhses, &chanaref, "Representation", &aaref);
	    if ( EVEN(sts)) goto return_now;
	    
	    sts = ldh_WriteAttribute( ctx->ldhses, &aaref, &representation, sizeof(representation));
	    if ( EVEN(sts)) goto return_now;

	    // Set Number
	    pwr_tUInt16 number = input_vect[j].number;
	    sts = ldh_ArefANameToAref( ctx->ldhses, &chanaref, "Number", &aaref);
	    if ( EVEN(sts)) goto return_now;
	    
	    sts = ldh_WriteAttribute( ctx->ldhses, &aaref, &number, sizeof(number));
	    if ( EVEN(sts)) goto return_now;

	    // Set Description
	    pwr_tString80 description;
	    strncpy( description, input_vect[j].description, sizeof(description));
	    sts = ldh_ArefANameToAref( ctx->ldhses, &chanaref, "Description", &aaref);
	    if ( EVEN(sts)) goto return_now;
	    
	    sts = ldh_WriteAttribute( ctx->ldhses, &aaref, description, sizeof(description));
	    if ( EVEN(sts)) goto return_now;
	  }
	  for ( unsigned int j = 0; j < output_vect.size(); j++) {
	    char name[80];
	    sprintf( name, "Ch%02u", chan_cnt++);
	    sts = ldh_CreateObject( ctx->ldhses, &oid, name, output_vect[j].cid,
				    slot->module_oid, ldh_eDest_IntoLast);
	    if ( EVEN(sts)) goto return_now;

	    pwr_tAttrRef aaref;
	    pwr_tAttrRef chanaref = cdh_ObjidToAref( oid);

	    // Set Representation
	    pwr_tEnum representation = output_vect[j].representation;
	    sts = ldh_ArefANameToAref( ctx->ldhses, &chanaref, "Representation", &aaref);
	    if ( EVEN(sts)) goto return_now;
	    
	    sts = ldh_WriteAttribute( ctx->ldhses, &aaref, &representation, sizeof(representation));
	    if ( EVEN(sts)) goto return_now;

	    // Set Number
	    pwr_tUInt16 number = output_vect[j].number;
	    sts = ldh_ArefANameToAref( ctx->ldhses, &chanaref, "Number", &aaref);
	    if ( EVEN(sts)) goto return_now;
	    
	    sts = ldh_WriteAttribute( ctx->ldhses, &aaref, &number, sizeof(number));
	    if ( EVEN(sts)) goto return_now;

	    // Set Description
	    pwr_tString80 description;
	    strncpy( description, output_vect[j].description, sizeof(description));
	    sts = ldh_ArefANameToAref( ctx->ldhses, &chanaref, "Description", &aaref);
	    if ( EVEN(sts)) goto return_now;
	    
	    sts = ldh_WriteAttribute( ctx->ldhses, &aaref, description, sizeof(description));
	    if ( EVEN(sts)) goto return_now;
	  }
	}
      }
      else {
	// Remove existing channels
	vector<pwr_tOid> chanvect;
	pwr_tCid cid;
      
	for ( sts = ldh_GetChild( ctx->ldhses, slot->module_oid, &oid);
	      ODD(sts);
	      sts = ldh_GetNextSibling( ctx->ldhses, oid, &oid)) {
	  sts = ldh_GetObjectClass( ctx->ldhses, oid, &cid);
	  if ( EVEN(sts)) goto return_now;
	
	  switch ( cid) {
	  case pwr_cClass_ChanDi:
	  case pwr_cClass_ChanDo:
	  case pwr_cClass_ChanAi:
	  case pwr_cClass_ChanAo:
	  case pwr_cClass_ChanIi:
	  case pwr_cClass_ChanIo:
	    chanvect.push_back( oid);
	    break;
	  default: ;
	  }
	}
	for ( unsigned int i = 0; i < chanvect.size(); i++) {
	  sts = ldh_DeleteObject( ctx->ldhses, chanvect[i]);
	  if ( EVEN(sts)) goto return_now;
	}
      }
    }
  }
  
  // Write data of all devices for profinet viewer
  // Data is device name, IP and MAC address
  sts = generate_viewer_data( ctx);

  sts = rsts;

 return_now:
  ((WNav *)ctx->editor_ctx)->reset_nodraw();
  return sts;
}


static pwr_tStatus generate_viewer_data(device_sCtx *ctx)
{ 
  pwr_tOid controller;
  pwr_tCid ccid;
  FILE *fp;
  FILE *ofp;
  pwr_tFileName fname;
  char line[500];
  char elemv[3][200];
  int nr;
  char device_text[200];
  char device_name[80];
  char ip_address[80];
  char mac_address[80];
  unsigned int vendor_id = 0;
  unsigned int device_id = 0;
  char *s;
  pwr_tStatus sts;
  pwr_tOid oid;
  char *ethernet_device;
  int size;

  sts = ldh_GetParent( ctx->ldhses, ctx->aref.Objid, &controller);
  if ( EVEN(sts)) return sts;

  sts = ldh_GetObjectClass( ctx->ldhses, controller, &ccid);
  if ( ODD(sts) && ccid == pwr_cClass_PnControllerSoftingPNAK) {

    sts = ldh_GetObjectPar( ctx->ldhses, controller, "RtBody",
			    "EthernetDevice", (char **)&ethernet_device, &size);
    if ( EVEN(sts)) return sts;

    dcli_trim( ethernet_device, ethernet_device);
    cdh_ToLower( ethernet_device, ethernet_device);
    sprintf( fname, "$pwrp_load/pwr_pnviewer_%s.dat", ethernet_device);
    free( ethernet_device);
    dcli_translate_filename( fname, fname);
    ofp = fopen( fname, "w");
    if ( !ofp)
      return 0;


    for ( sts = ldh_GetChild( ctx->ldhses, controller, &oid);
	  ODD(sts);
	  sts = ldh_GetNextSibling( ctx->ldhses, oid, &oid)) {

      sprintf( fname, "$pwrp_load/pwr_pn_%s.xml", cdh_ObjidToFnString( 0, oid));
      dcli_translate_filename( fname, fname);

      fp = fopen( fname, "r");
      if ( !fp)
	return 0;

      while ( dcli_read_line( line, sizeof( line), fp)) {
	dcli_trim( line, line);
	nr = dcli_parse( line, "=", "", (char *)elemv, sizeof( elemv) / sizeof( elemv[0]), 
			 sizeof( elemv[0]), 0);
	if ( nr != 2)
	  continue;
	
	if ( strcmp( elemv[0], "DeviceText") == 0) {
	  strncpy( device_text, elemv[1], sizeof(device_text));
	}
	else if ( strcmp( elemv[0], "VendorId") == 0) {
	  sscanf( elemv[1], "%d", &vendor_id);
	}
	else if ( strcmp( elemv[0], "DeviceId") == 0) {
	  sscanf( elemv[1], "%d", &device_id);
	}
	else if ( strcmp( elemv[0], "DeviceName") == 0) {
	  strncpy( device_name, elemv[1], sizeof(device_name));
	}
	else if ( strcmp( elemv[0], "IP_Address") == 0) {
	  strncpy( ip_address, elemv[1], sizeof(ip_address));
	}
	else if ( strcmp( elemv[0], "MAC_Address") == 0) {
	  strncpy( mac_address, elemv[1], sizeof(mac_address));
	  if ( (s = strchr( mac_address, '/')))
	    *s = 0;
	  dcli_trim( mac_address, mac_address);
	  break;
	}
      }
      fclose(fp);

      fprintf( ofp, "\"%s\" \"%s\" \"%s\" \"%s\" %d %d\n", device_text, device_name, ip_address, mac_address,
	       vendor_id, device_id);
      
    }
    fclose(ofp);
  }      
  return 1;
}
  
static int pndevice_check_io( device_sCtx *ctx, gsdml_VirtualSubmoduleList *vsl, 
			      vector<ChanItem>& input_vect, vector<ChanItem>& output_vect)
{
  int sts;

  if ( vsl) {
    unsigned int subslot_number = 0;
	
    for ( unsigned int i = 0; i < vsl->VirtualSubmoduleItem.size(); i++) {
      if ( strcmp( vsl->VirtualSubmoduleItem[i]->Body.FixedInSubslots.str, "") == 0) {
	// FixedInSubslots not supplied, default subslot number is 1 
	
	if ( vsl->VirtualSubmoduleItem.size() == 1)
	  subslot_number = 1;
	else
	  subslot_number++;
	
	sts = pndevice_add_channels( ctx, vsl->VirtualSubmoduleItem[i], subslot_number,
				     input_vect, output_vect);
	if ( EVEN(sts)) return sts;
      }
      else {
	// FixedInSubslots supplied, create channels for all fixed subslots
	
	gsdml_Valuelist *vl = new gsdml_Valuelist(  vsl->
			      VirtualSubmoduleItem[i]->Body.FixedInSubslots.str);
	gsdml_ValuelistIterator iter( vl);
	
	for ( unsigned int j = iter.begin(); j != iter.end(); j = iter.next()) {
	  subslot_number = j;
	  
	  sts = pndevice_add_channels( ctx, vsl->VirtualSubmoduleItem[i], subslot_number,
				       input_vect, output_vect);
	  if (EVEN(sts)) { 
	    delete vl; 
	    return sts;
	  }
	}
	delete vl;
      }
    }
  }
  return PB__SUCCESS;
}
    
static int pndevice_add_channels( device_sCtx *ctx, gsdml_VirtualSubmoduleItem *vi, int subslot_number,
				  vector<ChanItem>& input_vect, vector<ChanItem>& output_vect)
{

  // Find input data
  if ( vi->IOData && vi->IOData->Input) {
    for ( unsigned int i = 0; 
	  i < vi->IOData->Input->DataItem.size(); 
	  i++) {
      gsdml_DataItem *di = vi->IOData->Input->DataItem[i];
      gsdml_eValueDataType datatype;
      
      ctx->attr->attrnav->gsdml->string_to_value_datatype( di->Body.DataType, &datatype);
      
      if ( !di->Body.UseAsBits) {
	unsigned int representation;
	int invalid_type = 0;

	switch ( datatype) {
	case gsdml_eValueDataType_Integer8:	
	  representation = pwr_eDataRepEnum_Int8;
	  break;
	case gsdml_eValueDataType_Unsigned8:
	  representation = pwr_eDataRepEnum_UInt8;
	  break;
	case gsdml_eValueDataType_Integer16:
	  representation = pwr_eDataRepEnum_Int16;
	  break;
	case gsdml_eValueDataType_Unsigned16:
	  representation = pwr_eDataRepEnum_UInt16;
	  break;
	case gsdml_eValueDataType_Integer32:
	  representation = pwr_eDataRepEnum_Int32;
	  break;
	case gsdml_eValueDataType_Unsigned32:
	  representation = pwr_eDataRepEnum_UInt32;
	  break;
	case gsdml_eValueDataType_Integer64:
	  representation = pwr_eDataRepEnum_Int64;
	  break;
	case gsdml_eValueDataType_Unsigned64:
	  representation = pwr_eDataRepEnum_UInt64;
	  break;
	case gsdml_eValueDataType_Float32:
	  representation = pwr_eDataRepEnum_Float32;
	  break;
	case gsdml_eValueDataType_Float64:
	  representation = pwr_eDataRepEnum_Float64;
	  break;
	default:
	  invalid_type = 1;
	}

	if ( invalid_type)
	  return PB__CREATECHAN;

	ChanItem ci;
	ci.subslot_number = subslot_number;
	ci.number = 0;
	ci.representation = representation;
	ci.use_as_bit = 0;	
	ci.cid = pwr_cClass_ChanAi;
	strncpy( ci.description, (char *)di->Body.TextId.p, sizeof(ci.description));
	ci.description[sizeof(ci.description)-1] = 0;
	
	input_vect.push_back( ci);
      }
      else {
	// Use as bits
	unsigned int bits;
	unsigned int representation;

	switch ( datatype) {
	case gsdml_eValueDataType_Integer8:
	case gsdml_eValueDataType_Unsigned8:
	  representation = pwr_eDataRepEnum_Bit8;
	  bits = 8;
	  break;
	case gsdml_eValueDataType_Integer16:
	case gsdml_eValueDataType_Unsigned16:
	  representation = pwr_eDataRepEnum_Bit16;
	  bits = 16;
	  break;
	case gsdml_eValueDataType_Integer32:
	case gsdml_eValueDataType_Unsigned32:
	  representation = pwr_eDataRepEnum_Bit32;
	  bits = 32;
	  break;
	case gsdml_eValueDataType_Integer64:
	case gsdml_eValueDataType_Unsigned64:
	  representation = pwr_eDataRepEnum_Bit64;
	  bits = 64;
	  break;
	default:
	  bits = 0;
	}
	if ( di->BitDataItem.size() == 0) {
	  // Add all bits
	  for ( unsigned int j = 0; j < bits; j++) {
	    // Add Channel
	    ChanItem ci;
	    ci.subslot_number = subslot_number;
	    ci.number = j;
	    ci.representation = representation;
	    ci.use_as_bit = 1;
	    ci.cid = pwr_cClass_ChanDi;
	    strncpy( ci.description, (char *)di->Body.TextId.p, sizeof(ci.description));
	    ci.description[sizeof(ci.description)-2] = 0;			   
	    
	    input_vect.push_back( ci);
	  }
	}
	else {
	  for ( unsigned int j = 0; j < di->BitDataItem.size(); j++) {
	    // Add channel
	    ChanItem ci;
	    ci.subslot_number = subslot_number;
	    ci.number = di->BitDataItem[j]->Body.BitOffset;
	    ci.representation = representation;
	    ci.use_as_bit = 1;
	    ci.cid = pwr_cClass_ChanDi;
	    strncpy( ci.description, (char *)di->BitDataItem[j]->Body.TextId.p, sizeof(ci.description));
	    ci.description[sizeof(ci.description)-2] = 0;			   

	    input_vect.push_back( ci);
	  }
	}
      }
    }
  }

  // Find output data
  if ( vi->IOData && vi->IOData->Output) {
    for ( unsigned int i = 0; 
	  i < vi->IOData->Output->DataItem.size(); 
	  i++) {
      gsdml_DataItem *di = vi->IOData->Output->DataItem[i];
      gsdml_eValueDataType datatype;
      
      ctx->attr->attrnav->gsdml->string_to_value_datatype( di->Body.DataType, &datatype);
      
      if ( !di->Body.UseAsBits) {
	unsigned int representation;
	int invalid_type = 0;

	switch ( datatype) {
	case gsdml_eValueDataType_Integer8:	
	  representation = pwr_eDataRepEnum_Int8;
	  break;
	case gsdml_eValueDataType_Unsigned8:
	  representation = pwr_eDataRepEnum_UInt8;
	  break;
	case gsdml_eValueDataType_Integer16:
	  representation = pwr_eDataRepEnum_Int16;
	  break;
	case gsdml_eValueDataType_Unsigned16:
	  representation = pwr_eDataRepEnum_UInt16;
	  break;
	case gsdml_eValueDataType_Integer32:
	  representation = pwr_eDataRepEnum_Int32;
	  break;
	case gsdml_eValueDataType_Unsigned32:
	  representation = pwr_eDataRepEnum_UInt32;
	  break;
	case gsdml_eValueDataType_Integer64:
	  representation = pwr_eDataRepEnum_Int64;
	  break;
	case gsdml_eValueDataType_Unsigned64:
	  representation = pwr_eDataRepEnum_UInt64;
	  break;
	case gsdml_eValueDataType_Float32:
	  representation = pwr_eDataRepEnum_Float32;
	  break;
	case gsdml_eValueDataType_Float64:
	  representation = pwr_eDataRepEnum_Float64;
	  break;
	default:
	  invalid_type = 1;
	}

	if ( invalid_type) {
	  printf("GSDML-Error, Invalid type, unable to create channel\n");
	  return 0;
	}

	ChanItem ci;
	ci.subslot_number = subslot_number;
	ci.number = 0;
	ci.representation = representation;
	ci.use_as_bit = 0;	
	ci.cid = pwr_cClass_ChanAo;
	strncpy( ci.description, (char *)di->Body.TextId.p, sizeof(ci.description));
	ci.description[sizeof(ci.description)-2] = 0;			   

	output_vect.push_back( ci);
      }
      else {
	// Use as bits
	unsigned int bits;
	unsigned int representation;

	switch ( datatype) {
	case gsdml_eValueDataType_Integer8:
	case gsdml_eValueDataType_Unsigned8:
	  representation = pwr_eDataRepEnum_Bit8;
	  bits = 8;
	  break;
	case gsdml_eValueDataType_Integer16:
	case gsdml_eValueDataType_Unsigned16:
	  representation = pwr_eDataRepEnum_Bit16;
	  bits = 16;
	  break;
	case gsdml_eValueDataType_Integer32:
	case gsdml_eValueDataType_Unsigned32:
	  representation = pwr_eDataRepEnum_Bit32;
	  bits = 32;
	  break;
	case gsdml_eValueDataType_Integer64:
	case gsdml_eValueDataType_Unsigned64:
	  representation = pwr_eDataRepEnum_Bit64;
	  bits = 64;
	  break;
	default:
	  bits = 0;
	}
	if ( di->BitDataItem.size() == 0) {
	  // Add all bits
	  for ( unsigned int j = 0; j < bits; j++) {
	    // Add Channel
	    ChanItem ci;
	    ci.subslot_number = subslot_number;
	    ci.number = j;
	    ci.representation = representation;
	    ci.use_as_bit = 1;
	    ci.cid = pwr_cClass_ChanDo;
	    strncpy( ci.description, (char *)di->Body.TextId.p, sizeof(ci.description));
	    ci.description[sizeof(ci.description)-2] = 0;			   
	    
	    output_vect.push_back( ci);
	  }
	}
	else {
	  for ( unsigned int j = 0; j < di->BitDataItem.size(); j++) {
	    // Add channel
	    ChanItem ci;
	    ci.subslot_number = subslot_number;
	    ci.number = di->BitDataItem[j]->Body.BitOffset;
	    ci.representation = representation;
	    ci.use_as_bit = 1;
	    ci.cid = pwr_cClass_ChanDo;
	    strncpy( ci.description, (char *)di->BitDataItem[j]->Body.TextId.p, sizeof(ci.description));
	    ci.description[sizeof(ci.description)-2] = 0;			   

	    output_vect.push_back( ci);
	  }
	}
      }
    }
  }
  return PB__SUCCESS;
}

pwr_tStatus pndevice_create_ctx( ldh_tSession ldhses, pwr_tAttrRef aref, 
				    void *editor_ctx, device_sCtx **ctxp)
{
  pwr_tOName name;
  char *gsdmlfile;
  int size;
  int sts;
  pwr_tFileName fname;
  ldh_sSessInfo Info;
  vector<pwr_tCid> mcv;

  sts = ldh_GetSessionInfo( ldhses, &Info);


  sts = ldh_ObjidToName( ldhses, aref.Objid, 
			 ldh_eName_Hierarchy, name, sizeof(name), &size);
  if ( EVEN(sts)) return sts;

  sts = ldh_GetObjectPar( ldhses, aref.Objid, "RtBody",
			  "GSDMLfile", &gsdmlfile, &size);
  if ( EVEN(sts)) return sts;
  if ( strcmp( gsdmlfile, "") == 0) {
    free( gsdmlfile);  
    return PB__GSDATTR;
  }

  device_sCtx *ctx = (device_sCtx *) calloc( 1, sizeof(device_sCtx));
  ctx->ldhses = ldhses;
  ctx->aref = aref;
  ctx->editor_ctx = editor_ctx;
  ctx->edit_mode = (ODD(sts) && Info.Access == ldh_eAccess_ReadWrite) &&
    ldh_LocalObject( ldhses, aref.Objid);

  get_subcid( ctx->ldhses, pwr_cClass_PnModule, mcv);
  ctx->mc = (gsdml_sModuleClass *) calloc( mcv.size() + 2, sizeof(gsdml_sModuleClass));

  ctx->mc[0].cid = pwr_cClass_PnModule;
  sts = ldh_ObjidToName( ctx->ldhses, cdh_ClassIdToObjid(ctx->mc[0].cid), cdh_mName_object, 
			 ctx->mc[0].name, sizeof(ctx->mc[0].name), &size);
  if ( EVEN(sts)) return sts;

  for ( int i = 1; i <= (int) mcv.size(); i++) {
    ctx->mc[i].cid = mcv[i-1];
    sts = ldh_ObjidToName( ctx->ldhses, cdh_ClassIdToObjid(ctx->mc[i].cid), cdh_mName_object, 
			   ctx->mc[i].name, sizeof(ctx->mc[0].name), &size);
    if ( EVEN(sts)) return sts;
  }

  if ( strchr( gsdmlfile, '/') == 0) {
    strcpy( fname, "$pwrp_exe/");
    strcat( fname, gsdmlfile);
  }
  else
    strcpy( fname, gsdmlfile);
  free( gsdmlfile);
    
  ctx->gsdml = new pn_gsdml();
  sts = ctx->gsdml->read( fname);
  if ( EVEN(sts))
    return sts;
  ctx->gsdml->build();
  ctx->gsdml->set_classes( ctx->mc);

  *ctxp = ctx;
  return 1;
}

pwr_tStatus pndevice_init( device_sCtx *ctx)
{
  pwr_tOid module_oid;
  int corrupt = 0;
  unsigned int idx;
  pwr_tStatus sts;
  
  // Identify module objects

  int size;
  pwr_tObjName module_name;

  for ( sts = ldh_GetChild( ctx->ldhses, ctx->aref.Objid, &module_oid);
	ODD(sts);
	sts = ldh_GetNextSibling( ctx->ldhses, module_oid, &module_oid)) {
    sts = ldh_ObjidToName( ctx->ldhses,  module_oid, cdh_mName_object, module_name,
			   sizeof(module_name), &size);
    if ( EVEN(sts)) return sts;

    if ( !(sscanf(  module_name, "M%d", &idx) == 1)) {
      corrupt = 1;
      continue;
    }
    if ( idx >= ctx->attr->attrnav->dev_data.slot_data.size()) {
      corrupt = 1;
      continue;
    }
    ctx->attr->attrnav->dev_data.slot_data[idx]->module_oid = module_oid;
  }

  if ( corrupt) {
    corrupt = 0;

    // Not standard module names, get slot number from object order instead
    idx = 1;
    for ( sts = ldh_GetChild( ctx->ldhses, ctx->aref.Objid, &module_oid);
	  ODD(sts);
	  sts = ldh_GetNextSibling( ctx->ldhses, module_oid, &module_oid)) {
      if ( idx >= ctx->attr->attrnav->dev_data.slot_data.size()) {
	corrupt = 1;
	break;
      }
      ctx->attr->attrnav->dev_data.slot_data[idx]->module_oid = module_oid;
      idx++;
    }
    if ( corrupt)
      ctx->attr->wow->DisplayError( "Configuration corrupt", 
		      "Configuration of module objects doesn't match device configuration");    
  }
  return 1;
}