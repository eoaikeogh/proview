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

#include "flow_std.h"

#include <stdio.h>
#include <stdlib.h>
#include <Xm/Xm.h>
#include <Xm/XmP.h>
#include <Xm/ScrollBar.h>
#include <Xm/Form.h>
#include <Mrm/MrmPublic.h>
#ifndef _XtIntrinsic_h
#include <X11/Intrinsic.h>
#endif
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include "flow.h"
#include "flow_ctx.h"
#include "flow_draw_xlib.h"
#include "flow_widget_motif.h"


typedef struct {
	Widget  flow;
	Widget	form;
	Widget	scroll_h;
	Widget	scroll_v;
	int	scroll_h_managed;
	int	scroll_v_managed;
	} flowwidget_sScroll;

static void Notify( Widget w, XEvent *event);
static void Destroy( Widget w);
static void Realize( Widget w, unsigned long *dum, XSetWindowAttributes *swa);
static void Initialize( Widget rec, Widget new_widget, ArgList arg, int *args);
static void Redisplay( Widget w, XEvent *event, Region region);
static Boolean SetValues( Widget old, Widget request, Widget new_widget);

static char defaultTranslations[] = "#replace \n\
<Btn1Up>: notify()\n\
<Btn2Up>: notify()\n\
<Btn3Up>: notify()\n\
<Btn1Down>: notify()\n\
<Btn2Down>: notify()\n\
<Btn3Down>: notify()\n\
<BtnMotion>: notify()\n\
<EnterWindow>: notify()\n\
<LeaveWindow>: notify()\n\
<VisibilityNotify>: notify()\n\
<MotionNotify>: notify()\n\
<Key>Up: notify()\n\
<Key>Down: notify()\n\
<Key>Return: notify()";

static XtActionsRec actionsList[] = { {(char*) "notify", (XtActionProc) Notify}};

FlowClassRec flowClassRec = {
  { /* Core class part */
    (WidgetClass) &compositeClassRec,	/* superclass */
    (char*) "Flow",		       	/* class name */
    sizeof(FlowRec),			/* widget size */
    NULL,				/* class initialize */
    NULL, 				/* class part initialize */
    FALSE, 				/* class inited */
    (XtInitProc) Initialize,		/* initialize */
    NULL, 				/* initialize hook */
    Realize,				/* realize */
    actionsList, 			/* actions */
    XtNumber( actionsList),		/* num actions */
    NULL, 				/* resourses */
    0,					/* num resourses */
    NULLQUARK,				/* xrm class */
    TRUE,				/* compress motion */
    TRUE,				/* compress expsure */
    TRUE,				/* compress enterleave */
    FALSE,				/* visible interest */
    Destroy,				/* destroy */
    XtInheritResize,			/* resize */
    Redisplay,				/* expose */
    (XtSetValuesFunc)SetValues,		/* set values */
    NULL,				/* set values hook */
    XtInheritSetValuesAlmost,		/* set values almost */
    NULL,				/* get values hook */
    NULL, 				/* accept focus */
    XtVersionDontCheck,			/* version */
    NULL, 				/* callback offsets */
    defaultTranslations,		/* tm_table */
    NULL,				/* geometry */
    NULL,				/* disp accelerators */
    NULL				/* extension */
  },
  { /* composite class record */
    NULL,				/* geometry manager */
    NULL,				/* change managed */
    NULL,				/* insert child */
    NULL,				/* delete child */
    NULL				/* extension */
  },
  { /* flow class record */
    NULL,
    0
  }
};

WidgetClass flowWidgetClass = (WidgetClass) &flowClassRec;

static void scroll_h_action( 	Widget 		w,
				XtPointer 	client_data,
				XtPointer 	call_data);
static void scroll_v_action( 	Widget 		w,
				XtPointer 	client_data,
				XtPointer 	call_data);
static void scroll_callback( flow_sScroll *data);

int flow_init_proc( Widget w, FlowCtx *fctx, void *client_data)
{
  flowwidget_sScroll *scroll_data;
  FlowCtx	*ctx;

  if ( ((FlowWidget) w)->flow.scrollbars)
  {
    scroll_data = (flowwidget_sScroll *) malloc( sizeof( flowwidget_sScroll));
    scroll_data->flow = w;
    scroll_data->scroll_h = ((FlowWidget) w)->flow.scroll_h;
    scroll_data->scroll_v = ((FlowWidget) w)->flow.scroll_v;
    scroll_data->form = ((FlowWidget) w)->flow.form;
    scroll_data->scroll_h_managed = 1;
    scroll_data->scroll_v_managed = 1;

    ctx = (FlowCtx *) ((FlowWidget) w)->flow.flow_ctx;
    ctx->register_scroll_callback( (void *) scroll_data, scroll_callback);

    XtAddCallback( scroll_data->scroll_h, XmNvalueChangedCallback, scroll_h_action, w);
    XtAddCallback( scroll_data->scroll_h, XmNdragCallback, scroll_h_action, w);
    XtAddCallback( scroll_data->scroll_h, XmNincrementCallback, scroll_h_action, w);
    XtAddCallback( scroll_data->scroll_h, XmNdecrementCallback, scroll_h_action, w);
    XtAddCallback( scroll_data->scroll_h, XmNpageIncrementCallback, scroll_h_action, w);
    XtAddCallback( scroll_data->scroll_h, XmNpageDecrementCallback, scroll_h_action, w);
    XtAddCallback( scroll_data->scroll_h, XmNtoTopCallback, scroll_h_action, w);
    XtAddCallback( scroll_data->scroll_h, XmNtoBottomCallback, scroll_h_action, w);

    XtAddCallback( scroll_data->scroll_v, XmNvalueChangedCallback, scroll_v_action, w);
    XtAddCallback( scroll_data->scroll_v, XmNdragCallback, scroll_v_action, w);
    XtAddCallback( scroll_data->scroll_v, XmNincrementCallback, scroll_v_action, w);
    XtAddCallback( scroll_data->scroll_v, XmNdecrementCallback, scroll_v_action, w);
    XtAddCallback( scroll_data->scroll_v, XmNpageIncrementCallback, scroll_v_action, w);
    XtAddCallback( scroll_data->scroll_v, XmNpageDecrementCallback, scroll_v_action, w);
    XtAddCallback( scroll_data->scroll_v, XmNtoTopCallback, scroll_v_action, w);
    XtAddCallback( scroll_data->scroll_v, XmNtoBottomCallback, scroll_v_action, w);
  }
  else
    ctx = (FlowCtx *) ((FlowWidget) w)->flow.flow_ctx;
  return (((FlowWidget) w)->flow.init_proc)( ctx, client_data);
}

static void scroll_h_action( 	Widget 		w,
				XtPointer 	client_data,
				XtPointer 	call_data)
{
  XmScrollBarCallbackStruct *cbs = (XmScrollBarCallbackStruct *) call_data;
  FlowCtx *ctx = (FlowCtx *) ((FlowWidget) client_data)->flow.flow_ctx;

  switch( cbs->reason)
  {
    case XmCR_DRAG:
    case XmCR_VALUE_CHANGED:
    case XmCR_INCREMENT:
    case XmCR_DECREMENT:
    case XmCR_PAGE_INCREMENT:
    case XmCR_PAGE_DECREMENT:
    case XmCR_TO_TOP:
    case XmCR_TO_BOTTOM:
      flow_scroll_horizontal( ctx, cbs->value, 0);
      break;
  }
}

static void scroll_v_action( 	Widget 		w,
				XtPointer 	client_data,
				XtPointer 	call_data)
{

  XmScrollBarCallbackStruct *cbs = (XmScrollBarCallbackStruct *) call_data;
  FlowCtx *ctx = (FlowCtx *) ((FlowWidget) client_data)->flow.flow_ctx;
  Arg 		arg[20];
  int		i;
  int		maximum, slider, value, bottom;


  // Calculate if position is bottom
  i = 0;
  XtSetArg( arg[i], XmNmaximum, &maximum);i++;
  XtSetArg( arg[i], XmNsliderSize, &slider);i++;
  XtSetArg( arg[i], XmNvalue, &value);i++;
  XtGetValues( w, arg, i);
  if ( slider + value == maximum)
    bottom = 1;
  else
    bottom = 0;

  switch( cbs->reason)
  {
    case XmCR_DRAG:
    case XmCR_VALUE_CHANGED:
    case XmCR_INCREMENT:
    case XmCR_DECREMENT:
    case XmCR_PAGE_INCREMENT:
    case XmCR_PAGE_DECREMENT:
    case XmCR_TO_TOP:
    case XmCR_TO_BOTTOM:
      flow_scroll_vertical( ctx, cbs->value, bottom);
      break;
  }
}

static void scroll_callback( flow_sScroll *data)
{
  flowwidget_sScroll *scroll_data;
  Arg 		arg[20];
  int		i;

  scroll_data = (flowwidget_sScroll *) data->scroll_data;

  if ( data->total_width <= data->window_width)
  {
    if ( data->offset_x == 0)
      data->total_width = data->window_width;
    if ( scroll_data->scroll_h_managed)
    {
      // Remove horizontal scrollbar
    }
  }
  else
  {
    if ( !scroll_data->scroll_h_managed)
    {
      // Insert horizontal scrollbar
    }
  }

  if ( data->total_height <= data->window_height)
  {
    if ( data->offset_y == 0)
      data->total_height = data->window_height;
    if ( scroll_data->scroll_v_managed)
    {
      // Remove vertical scrollbar
    }
  }
  else
  {
    if ( !scroll_data->scroll_v_managed)
    {
      // Insert vertical scrollbar
    }
  }
  if ( data->offset_x < 0)
    data->offset_x = 0;
  if ( data->offset_y < 0)
    data->offset_y = 0;
  if ( data->total_height < data->window_height + data->offset_y)
    data->total_height = data->window_height + data->offset_y;
  if ( data->total_width < data->window_width + data->offset_x)
    data->total_width = data->window_width + data->offset_x;
  if ( data->window_width < 1)
    data->window_width = 1;
  if ( data->window_height < 1)
    data->window_height = 1;

  if ( scroll_data->scroll_h_managed)
  {
    i = 0;
    XtSetArg( arg[i], XmNmaximum, data->total_width);i++;
    XtSetArg( arg[i], XmNsliderSize, data->window_width);i++;
    XtSetArg( arg[i], XmNvalue, data->offset_x);i++;
    XtSetValues( scroll_data->scroll_h, arg, i);
  }

  if ( scroll_data->scroll_v_managed)
  {
    i = 0;
    XtSetArg( arg[i], XmNmaximum, data->total_height);i++;
    XtSetArg( arg[i], XmNsliderSize, data->window_height);i++;
    XtSetArg( arg[i], XmNvalue, data->offset_y);i++;
    XtSetValues( scroll_data->scroll_v, arg, i);
  }
}

static void Initialize( Widget rec, Widget new_widget, ArgList arg, int *args)
{
/*
  FlowWidget w;

  XtManageChild( new_widget);
  w = (FlowWidget) new_widget;
*/
}  

static void Redisplay( Widget w, XEvent *event, Region region)
{

  ((FlowDrawXLib *)((FlowCtx *)((FlowWidget)w)->flow.flow_ctx)->fdraw)->event_handler( (FlowCtx *)((FlowWidget)w)->flow.flow_ctx, *event);
}

static void Notify( Widget w, XEvent *event)
{

  ((FlowDrawXLib *)((FlowCtx *)((FlowWidget)w)->flow.flow_ctx)->fdraw)->event_handler( (FlowCtx *)((FlowWidget)w)->flow.flow_ctx, *event);
}

static Boolean SetValues( Widget old, Widget request, Widget new_widget)
{
  return 0;
}

static void Destroy( Widget w)
{
  if ( ((FlowWidget) w)->flow.is_navigator)
    return;
  delete ((FlowCtx *)((FlowWidget)w)->flow.flow_ctx)->fdraw;
}

static void Realize( Widget w, unsigned long *mask, XSetWindowAttributes *swa)
{

  (* flowWidgetClass->core_class.superclass->core_class.realize)
	(w, mask, swa);

  if ( ((FlowWidget) w)->flow.is_navigator)
  {
    if ( !((FlowWidget) w)->flow.flow_ctx)
    {
      FlowWidget main_flow = (FlowWidget) ((FlowWidget) w)->flow.main_flow_widget;

      ((FlowWidget) w)->flow.flow_ctx = main_flow->flow.flow_ctx;
      ((FlowDrawXLib *)((FlowCtx *)((FlowWidget) w)->flow.flow_ctx)->fdraw)->init_nav( w, main_flow->flow.flow_ctx);
    }
  }
  else
  {
    if ( !((FlowWidget) w)->flow.flow_ctx)
    {
      FlowDrawXLib *fdraw = new FlowDrawXLib( w, 
			  &((FlowWidget) w)->flow.flow_ctx, 
			  flow_init_proc, 
			  ((FlowWidget) w)->flow.client_data,
			  flow_eCtxType_Flow);
      ((FlowCtx *)((FlowWidget) w)->flow.flow_ctx)->fdraw = fdraw;
    }
  }
}

extern "C" Widget FlowCreate( 
	Widget parent, 
	char *name, 
	ArgList args, 
	int argCount,
        int	(*init_proc)(FlowCtx *ctx, void *client_data),
	void *client_data
	)
{
  FlowWidget w;

  w = (FlowWidget) XtCreateWidget( name, flowWidgetClass, parent, args, 
		argCount);
  w->flow.init_proc = init_proc;
  w->flow.flow_ctx = 0;
  w->flow.is_navigator = 0;
  w->flow.client_data = client_data;
  w->flow.scrollbars = 0;
  return (Widget) w;
}

extern "C" Widget ScrolledFlowCreate( 
	Widget parent, 
	char *name, 
	ArgList args, 
	int argCount,
        int	(*init_proc)(FlowCtx *ctx, void *client_data),
	void *client_data, 
	Widget *flow_w
)
{
  Widget 	form, scroll_h, scroll_v;
  FlowWidget 	flow;
  Arg 		arg[20];
  int		i;
  int		scroll_width = 15;

  form = XtCreateWidget( name, xmFormWidgetClass, parent, args,
		argCount);

  i = 0;
  XtSetArg( arg[i], XmNorientation, XmHORIZONTAL); i++;
  XtSetArg( arg[i], XmNrightAttachment, XmATTACH_FORM);i++;
  XtSetArg( arg[i], XmNleftAttachment, XmATTACH_FORM);i++;
  XtSetArg( arg[i], XmNbottomAttachment, XmATTACH_FORM);i++;
  XtSetArg( arg[i], XmNheight, scroll_width);i++;
  XtSetArg( arg[i], XmNrightOffset, scroll_width);i++;
  scroll_h = XtCreateWidget( "scroll_horizontal", xmScrollBarWidgetClass,
		form, arg, i);
  XtManageChild( scroll_h);

  i = 0;
  XtSetArg( arg[i], XmNorientation, XmVERTICAL); i++;
  XtSetArg( arg[i], XmNtopAttachment, XmATTACH_FORM);i++;
  XtSetArg( arg[i], XmNrightAttachment, XmATTACH_FORM);i++;
  XtSetArg( arg[i], XmNbottomAttachment, XmATTACH_FORM);i++;
  XtSetArg( arg[i], XmNwidth, scroll_width);i++;
  XtSetArg( arg[i], XmNbottomOffset, scroll_width);i++;
  scroll_v = XtCreateWidget( "scroll_vertical", xmScrollBarWidgetClass,
		form, arg, i);
  XtManageChild( scroll_v);

  i = 0;
  XtSetArg( arg[i], XmNtopAttachment, XmATTACH_FORM);i++;
  XtSetArg( arg[i], XmNleftAttachment, XmATTACH_FORM);i++;
  XtSetArg( arg[i], XmNbottomAttachment, XmATTACH_WIDGET);i++;
  XtSetArg( arg[i], XmNbottomWidget, scroll_h);i++;
  XtSetArg( arg[i], XmNrightAttachment, XmATTACH_WIDGET);i++;
  XtSetArg( arg[i], XmNrightWidget, scroll_v);i++;
/*
  XtSetArg( arg[i], XmNbottomAttachment, XmATTACH_FORM);i++;
  XtSetArg( arg[i], XmNrightAttachment, XmATTACH_FORM);i++;
*/
  flow = (FlowWidget) FlowCreate( form, (char*) "flow", arg, i, init_proc, client_data);
  XtManageChild( (Widget) flow);

  flow->flow.scroll_h = scroll_h;
  flow->flow.scroll_v = scroll_v;
  flow->flow.form = form;
  flow->flow.scrollbars = 1;

  *flow_w = (Widget) flow;
  return (Widget) form;
}

Widget FlowCreateNav( Widget parent, char *name, ArgList args, int argCount,
	Widget main_flow)
{
  FlowWidget w;

  w = (FlowWidget) XtCreateWidget( name, flowWidgetClass, parent, args, argCount);
  w->flow.is_navigator = 1;
  w->flow.flow_ctx = 0;
  w->flow.main_flow_widget = main_flow;
  return (Widget) w;
}