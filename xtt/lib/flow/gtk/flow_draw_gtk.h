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

#ifndef flow_draw_gtk_h
#define flow_draw_gtk_h

#include <stdlib.h>

#include <gtk/gtk.h>
#include "flow_draw.h"

class FlowDrawGtk : public FlowDraw {
 public:
  GtkWidget  	*toplevel;
  GtkWidget	*nav_shell;
  GtkWidget	*nav_toplevel; 
  GdkDisplay	*display;
  GdkWindow	*window;
  GdkWindow	*nav_window;
  GdkScreen	*screen;
  GdkGC		*gc;
  GdkGC		*gc_erase;
  GdkGC		*gc_inverse;
  GdkGC		*gc_yellow;
  GdkGC		*gc_green;
  GdkGC		*gc_darkgray;
  GdkGC		*gcs[flow_eDrawType__][DRAW_TYPE_SIZE];
  // XFontStruct	*font_struct[draw_eFont__][DRAW_FONT_SIZE];
  GdkFont 	*font[draw_eFont__][DRAW_FONT_SIZE];
  GdkColormap 	*colormap;
  GdkCursor	*cursors[draw_eCursor__];
  GdkColor 	background;
  GdkColor 	foreground;
  guint		timer_id;
  GdkColor	color_vect[20];
  int		color_vect_cnt;
  int   	closing_down;

  FlowDrawGtk( GtkWidget *toplevel, 
		void **flow_ctx,
		int (*init_proc)(GtkWidget *w, FlowCtx *ctx, void *client_data),
		void  *client_data, 
		flow_eCtxType type);
  ~FlowDrawGtk();
  int init_nav( GtkWidget *nav_widget, void *flow_ctx);
  int event_handler( FlowCtx *ctx, GdkEvent event);
  void	enable_event( FlowCtx *ctx, flow_eEvent event, 
		flow_eEventType event_type, 
		int (*event_cb)(FlowCtx *ctx, flow_tEvent event));
  void clear( FlowCtx *ctx);
  void nav_clear( FlowCtx *ctx);

  void get_window_size( FlowCtx *ctx, int *width, int *height);
  void get_nav_window_size( FlowCtx *ctx, int *width, int *height);
  void set_nav_window_size( FlowCtx *ctx, int width, int height);

  int rect( FlowCtx *ctx, int x, int y, int width, int height, 
	    flow_eDrawType gc_type, int idx, int highlight, int dimmed);
  int rect_erase( FlowCtx *ctx, int x, int y, int width, int height,
		  int idx);
  int nav_rect( FlowCtx *ctx, int x, int y, int width, int height,
		flow_eDrawType gc_type, int idx, int highlight);
  int nav_rect_erase( FlowCtx *ctx, int x, int y, int width, int height,
		      int idx);
  int triangle( FlowCtx *ctx, int x, int y, int width, int height, 
		flow_eDrawType gc_type, int idx, int highlight, int dimmed);
  int triangle_erase( FlowCtx *ctx, int x, int y, int width, int height,
		      int idx);
  int nav_triangle( FlowCtx *ctx, int x, int y, int width, int height,
		    flow_eDrawType gc_type, int idx, int highlight);
  int nav_fill_triangle( FlowCtx *ctx, int x, int y, int width, int height,
			 flow_eDrawType gc_type);
  int nav_triangle_erase( FlowCtx *ctx, int x, int y, int width, int height,
			  int idx);
  int arrow( FlowCtx *ctx, int x1, int y1, int x2, int y2, 
	     int x3, int y3,
	     flow_eDrawType gc_type, int idx, int highlight);
  int arrow_erase( FlowCtx *ctx, int x1, int y1, int x2, int y2, 
		   int x3, int y3,
		   int idx);
  int nav_arrow( FlowCtx *ctx, int x1, int y1, int x2, int y2, 
		 int x3, int y3,
		 flow_eDrawType gc_type, int idx, int highlight);
  int nav_arrow_erase( FlowCtx *ctx, int x1, int y1, int x2, int y2, 
		       int x3, int y3,
		       int idx);
  int arc( FlowCtx *ctx, int x, int y, int width, int height, 
	   int angle1, int angle2,
	   flow_eDrawType gc_type, int idx, int highlight, int dimmed);
  int arc_erase( FlowCtx *ctx, int x, int y, int width, int height,
		 int angle1, int angle2,
		 int idx);
  int nav_arc( FlowCtx *ctx, int x, int y, int width, int height,
	       int angle1, int angle2,
	       flow_eDrawType gc_type, int idx, int highlight);
  int nav_arc_erase( FlowCtx *ctx, int x, int y, int width, int height,
		     int angle1, int angle2,
		     int idx);
  int line( FlowCtx *ctx, int x1, int y1, int x2, int y2,
	    flow_eDrawType gc_type, int idx, int highlight, int dimmed);
  int line_erase( FlowCtx *ctx, int x1, int y1, int x2, int y2,
		  int idx);
  int nav_line( FlowCtx *ctx, int x1, int y1, int x2, int y2,
		flow_eDrawType gc_type, int idx, int highlight);
  int nav_line_erase( FlowCtx *ctx, int x1, int y1, int x2, int y2,
		      int idx);
  int text( FlowCtx *ctx, int x, int y, char *text, int len,
	    flow_eDrawType gc_type, int idx, int highlight, int dimmed, int line, double size);
  int text_inverse( FlowCtx *ctx, int x, int y, char *text, int len,
		    flow_eDrawType gc_type, int idx, int line, double size);
  int text_erase( FlowCtx *ctx, int x, int y, char *text, int len,
		  flow_eDrawType gc_type, int idx, int line, double size);
  int nav_text( FlowCtx *ctx, int x, int y, char *text, int len,
		flow_eDrawType gc_type, int idx, int highlight, int line, double size);
  int nav_text_erase( FlowCtx *ctx, int x, int y, char *text, int len,
		      flow_eDrawType gc_type, int idx, int line, double size);
  int fill_rect( FlowCtx *ctx, int x, int y, int width, int height, 
		 flow_eDrawType gc_type);
  int fill_triangle( FlowCtx *ctx, int x, int y, int width, int height, 
		     flow_eDrawType gc_type);
  int image( FlowCtx *ctx, int x, int y, int width, int height,
	     flow_tImImage image, flow_tPixmap pixmap, flow_tPixmap clip_mask);
  int pixmaps_create( FlowCtx *ctx, flow_sPixmapData *pixmap_data,
		      void **pixmaps);
  void pixmaps_delete( FlowCtx *ctx, void *pixmaps);
  int pixmap( FlowCtx *ctx, int x, int y, flow_sPixmapData *pixmap_data,
	      void *pixmaps, flow_eDrawType gc_type, int idx, int highlight, int line);
  int pixmap_inverse( FlowCtx *ctx, int x, int y, flow_sPixmapData *pixmap_data,
		      void *pixmaps, flow_eDrawType gc_type, int idx, int line);
  int pixmap_erase( FlowCtx *ctx, int x, int y, flow_sPixmapData *pixmap_data,
		    void *pixmaps, flow_eDrawType gc_type, int idx, int line);
  int nav_pixmap( FlowCtx *ctx, int x, int y, flow_sPixmapData *pixmap_data,
		  void *pixmaps, flow_eDrawType gc_type, int idx, int highlight, int line);
  int nav_pixmap_erase( FlowCtx *ctx, int x, int y, flow_sPixmapData *pixmap_data,
			void *pixmaps, flow_eDrawType gc_type, int idx, int line);
  
  void set_timer( FlowCtx *ctx, int time_ms,
		       void (*callback_func)( FlowCtx *ctx), void **id);
  void cancel_timer( FlowCtx *ctx, void *id);
  void set_cursor( FlowCtx *ctx, draw_eCursor cursor);
  void set_nav_cursor( FlowCtx *ctx, draw_eCursor cursor);
  int get_text_extent( FlowCtx *ctx, const char *text, int len,
		       flow_eDrawType gc_type, int idx,
		       int *width, int *height, double size);
  void copy_area( FlowCtx *ctx, int x, int y);
  void clear_area( FlowCtx *ctx, int ll_x, int ur_x, int ll_y, int ur_y);
  int create_input( FlowCtx *ctx, int x, int y, char *text, int len,
		    int idx, int width, int height, void *node, int number, void **data)
    {return 1;}
  int close_input( FlowCtx *ctx, void *data) {return 1;}
  int get_input( FlowCtx *ctx, void *data, char **text) {return 1;}
  void move_input( FlowCtx *ctx, void *data, int x, int y,
		    flow_ePosition pos_type) {}
  void delete_secondary_ctx( FlowCtx *ctx);
  int create_secondary_ctx( 
			   FlowCtx *flow_ctx,
			   void **secondary_flow_ctx,
			   int (*init_proc)(FlowCtx *, void *),
			   void  *client_data, 
			   flow_eCtxType type);
  int change_ctx( 
		 FlowCtx *from_ctx,
		 FlowCtx *to_ctx);
  void set_inputfocus( FlowCtx *ctx);
  void set_click_sensitivity( FlowCtx *ctx, int value);
  void set_image_clip_mask( FlowCtx *ctx, flow_tPixmap pixmap, int x, int y);
  void reset_image_clip_mask( FlowCtx *ctx);
  void set_white_background( FlowCtx *ctx);
  int get_font_idx( int gc_type);
  int image_get_width( flow_tImImage image);
  int image_get_height( flow_tImImage image);
  void image_scale( float scale, flow_tImImage orig_im, flow_tImImage *im, 
		    flow_tPixmap *im_pixmap,
		    flow_tPixmap *im_mask);
  int image_load( const char *imagefile, float scale, float nav_scale,
		  flow_tImImage *orig_im, flow_tImImage *im, 
		  flow_tPixmap *im_pixmap, flow_tPixmap *im_mask,
		  flow_tPixmap *im_nav_pixmap, flow_tPixmap *im_nav_mask);
  int text_pango( FlowCtx *ctx, int x, int y, char *text, int len,
		  flow_eDrawType gc_type, int idx, int highlight, int dimmed, int line, double size);
  int text_inverse_pango( FlowCtx *ctx, int x, int y, char *text, int len,
			  flow_eDrawType gc_type, int idx, int line, double size);
  int text_erase_pango( FlowCtx *ctx, int x, int y, char *text, int len,
			flow_eDrawType gc_type, int idx, int line, double size);
  int get_text_extent_pango( FlowCtx *ctx, const char *text, int len,
			     flow_eDrawType gc_type, int idx, double size,
			     int *width, int *height);
  FlowPrintDraw *print_draw_new( void *context, const char *title, int page, void *flow_ctx, 
				 int page_border, int *sts);
};

#endif