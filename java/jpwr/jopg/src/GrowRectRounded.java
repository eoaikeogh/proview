/* 
 * Proview   Open Source Process Control.
 * Copyright (C) 2005-2012 SSAB EMEA AB.
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

package jpwr.jopg;
import jpwr.rt.*;
import java.io.*;
import java.util.*;

public class GrowRectRounded extends GlowArrayElem {
    GrowCmn cmn;
    String n_name;
    double x_right;
    double x_left;
    double y_high;
    double y_low;
    int original_border_drawtype;
    int original_fill_drawtype;
    int fill_drawtype;
    int border;
    double shadow_width;
    int shadow_contrast;
    int shadow;
    int relief;
    int disable_shadow;
    int fixposition;
    int gradient;
    int gradient_contrast;
    int disable_gradient;
    int dynamicsize;    
    GlowTransform trf;
    double round_amount;

    GlowPoint ll;
    GlowPoint ur;
    int draw_type;
    int line_width;
    int display_level;
    int fill;

    public GrowRectRounded(GrowCmn cmn) {
	this.cmn = cmn;
	trf = new GlowTransform();
	ll = new GlowPoint();
	ur = new GlowPoint();
    }


    public int type() {
	return Glow.eObjectType_GrowRectRounded;
    }

    public void open(BufferedReader reader) {
	String line;
	StringTokenizer token;
	boolean end_found = false;

	try {
	    while( (line = reader.readLine()) != null) {
		token = new StringTokenizer(line);
		int key = Integer.valueOf(token.nextToken());
		if ( cmn.debug) System.out.println( "GrowRectRounded : " + line);

		switch ( key) {
		case Glow.eSave_GrowRectRounded: 
		    break;
		case Glow.eSave_GrowRectRounded_n_name:
		    if ( token.hasMoreTokens())
			n_name = token.nextToken();			 
		    break;
		case Glow.eSave_GrowRectRounded_x_right: 
		    x_right = new Double(token.nextToken()).doubleValue(); 
		    break;
		case Glow.eSave_GrowRectRounded_x_left: 
		    x_left = new Double(token.nextToken()).doubleValue(); 
		    break;
		case Glow.eSave_GrowRectRounded_y_high: 
		    y_high = new Double(token.nextToken()).doubleValue(); 
		    break;
		case Glow.eSave_GrowRectRounded_y_low: 
		    y_low = new Double(token.nextToken()).doubleValue(); 
		    break;
		case Glow.eSave_GrowRectRounded_original_border_drawtype: 
		    original_border_drawtype = Integer.valueOf(token.nextToken()); 
		    break;
		case Glow.eSave_GrowRectRounded_original_fill_drawtype: 
		    original_fill_drawtype = Integer.valueOf(token.nextToken()); 
		    break;
		case Glow.eSave_GrowRectRounded_fill_drawtype: 
		    fill_drawtype = Integer.valueOf(token.nextToken()); 
		    break;
		case Glow.eSave_GrowRectRounded_border: 
		    border = Integer.valueOf(token.nextToken()); 
		    break;
		case Glow.eSave_GrowRectRounded_round_amount: 
		    round_amount = Double.valueOf(token.nextToken()); 
		    break;
		case Glow.eSave_GrowRectRounded_shadow_width: 
		    shadow_width = new Double(token.nextToken()).doubleValue(); 
		    break;
		case Glow.eSave_GrowRectRounded_shadow_contrast: 
		    shadow_contrast = Integer.valueOf(token.nextToken()); 
		    break;
		case Glow.eSave_GrowRectRounded_shadow: 
		    shadow = Integer.valueOf(token.nextToken()); 
		    break;
		case Glow.eSave_GrowRectRounded_relief: 
		    relief = Integer.valueOf(token.nextToken()); 
		    break;
		case Glow.eSave_GrowRectRounded_disable_shadow: 
		    disable_shadow = Integer.valueOf(token.nextToken()); 
		    break;
		case Glow.eSave_GrowRectRounded_fixposition:
		    fixposition = Integer.valueOf(token.nextToken());
		    break;
		case Glow.eSave_GrowRectRounded_gradient:
		    gradient = Integer.valueOf(token.nextToken());
		    break;
		case Glow.eSave_GrowRectRounded_gradient_contrast:
		    gradient_contrast = Integer.valueOf(token.nextToken());
		    break;
		case Glow.eSave_GrowRectRounded_disable_gradient:
		    disable_gradient = Integer.valueOf(token.nextToken());
		    break;
		case Glow.eSave_GrowRectRounded_dynamicsize:
		    dynamicsize = Integer.valueOf(token.nextToken());
		    break;
		case Glow.eSave_GrowRectRounded_dynamic:
		    if ( cmn.dynamicsize > 0) {
			for ( int j = 0; j < cmn.dynamicsize; j++) 
			    line = reader.readLine();  // TODO handle backslash and citationmarks
		    }
		    break;
		case Glow.eSave_GrowRectRounded_rect_part: 
		    glowrect_open( reader);
		    break;
		case Glow.eSave_GrowRectRounded_trf: 
		    trf.open( reader);
		    break;
		case Glow.eSave_End:
		    end_found = true;
		    break;
		default:
		    System.out.println( "Syntax error in GrowRectRounded");
		    break;
		}
		if ( end_found)
		    break;
	    }
		
	} catch ( Exception e) {
	    System.out.println( "IOException GrowRectRounded");
	}
    }

    public void glowrect_open(BufferedReader reader) {
	String line;
	StringTokenizer token;
	boolean end_found = false;

	try {
	    while( (line = reader.readLine()) != null) {
		token = new StringTokenizer(line);
		int key = Integer.valueOf(token.nextToken());
		if ( cmn.debug) System.out.println( "GlowRect : " + line);

		switch ( key) {

		case Glow.eSave_Rect: 
		    break;
		case Glow.eSave_Rect_draw_type: 
		    draw_type = Integer.valueOf(token.nextToken());
		    break;
		case Glow.eSave_Rect_line_width:
		    line_width = Integer.valueOf(token.nextToken());
		    break;
		case Glow.eSave_Rect_display_level:
		    display_level = Integer.valueOf(token.nextToken());
		    break;
		case Glow.eSave_Rect_fill:
		    fill = Integer.valueOf(token.nextToken());
		    break;
		case Glow.eSave_Rect_ll:
		    ll.open( reader);
		    break;
		case Glow.eSave_Rect_ur:
		    ur.open( reader);
		    break;
		case Glow.eSave_End:
		    end_found = true;
		    break;
		default:
		    System.out.println( "Syntax error in GlowRect");
		    break;
		}
		if ( end_found)
		    break;

	    }

	} catch ( Exception e) {
	    System.out.println( "IOException GlowRect");
	}
    }

    public int eventHandler( GlowEvent event, double fx, double fy) {
	GlowPointDX rp;

	switch ( event.event) {
	case Glow.eEvent_CursorMotion:		
	    return 0;
	default: ;
	}

	rp = trf.reverse( fx, fy);
	//rp = new GlowPoint();
	//rp.x = fx;
	//rp.y = fy;
	if ( ll.x <= rp.x && rp.x <= ur.x &&
	     ll.y <= rp.y && rp.y <= ur.y) {
	    System.out.println( "Event handler: Hit in rect");
	    return 1;
	}  
	else
	    return 0;
    }

    public void draw() {
	draw( null, 0, 0, null, null);
    }


    public void draw(GlowTransform t, int highlight, int hot, Object node, Object colornode) {
	if ( cmn.nodraw != 0)
	    return;

	int chot = 0;
	int idx;
	int drawtype;

	if ( node != null && ((GrowNode)node).line_width != 0)
	    idx = (int)( cmn.mw.zoom_factor_y / cmn.mw.base_zoom_factor * 
			 ((GrowNode)node).line_width - 1);
	else
	    idx = (int)( cmn.mw.zoom_factor_y / cmn.mw.base_zoom_factor * line_width - 1);
	idx += hot;

	idx = Math.max( 0, idx);
	idx = Math.min( idx, Glow.DRAW_TYPE_SIZE-1);
	int x1, y1, x2, y2, ll_x, ll_y, ur_x, ur_y;

	if (t == null) {
	    x1 = (int)( trf.x( ll.x, ll.y) * cmn.mw.zoom_factor_x + 0.5) - cmn.mw.offset_x;
	    y1 = (int)( trf.y( ll.x, ll.y) * cmn.mw.zoom_factor_y + 0.5) - cmn.mw.offset_y;
	    x2 = (int)( trf.x( ur.x, ur.y) * cmn.mw.zoom_factor_x + 0.5) - cmn.mw.offset_x;
	    y2 = (int)( trf.y( ur.x, ur.y) * cmn.mw.zoom_factor_y + 0.5) - cmn.mw.offset_y;
	}
	else {
	    x1 = (int)( trf.x( t, ll.x, ll.y) * cmn.mw.zoom_factor_x + 0.5) - cmn.mw.offset_x;
	    y1 = (int)( trf.y( t, ll.x, ll.y) * cmn.mw.zoom_factor_y + 0.5) - cmn.mw.offset_y;
	    x2 = (int)( trf.x( t, ur.x, ur.y) * cmn.mw.zoom_factor_x + 0.5) - cmn.mw.offset_x;
	    y2 = (int)( trf.y( t, ur.x, ur.y) * cmn.mw.zoom_factor_y + 0.5) - cmn.mw.offset_y;
	}

	ll_x = Math.min( x1, x2);
	ur_x = Math.max( x1, x2);
	ll_y = Math.min( y1, y2);
	ur_y = Math.max( y1, y2);
	int amount = (int)( round_amount/100 * Math.min(ur_x - ll_x, ur_y - ll_y) + 0.5);
	if ( fill != 0) {

	    int ish = (int)( shadow_width / 100 * Math.min(ur_x - ll_x, ur_y - ll_y) + 0.5);
	    boolean display_shadow = ((node != null && ((GrowNode)node).shadow != 0) || shadow != 0) && disable_shadow == 0;
	    int fillcolor = GlowColor.get_drawtype( fill_drawtype, Glow.eDrawType_FillHighlight,
						    highlight, colornode, 1, 0);

	    int grad = gradient;
	    if ( gradient == Glow.eGradient_No && 
		 (node != null && ((GrowNode)node).gradient != Glow.eGradient_No) && disable_gradient == 0)
		grad = ((GrowNode)node).gradient;

	    if ( !display_shadow && ish == 0) {
		if ( grad == Glow.eGradient_No) {
		    if ( chot != 0)
			drawtype = GlowColor.shift_drawtype( fillcolor, chot, 0);
		    else
			drawtype = fillcolor;

		    cmn.gdraw.fill_rect( ll_x, ll_y + amount, ur_x - ll_x, ur_y - ll_y - 2*amount, drawtype);
		    cmn.gdraw.fill_rect( ll_x + amount, ll_y, ur_x - ll_x - 2*amount, amount, drawtype);
		    cmn.gdraw.fill_rect( ll_x + amount, ur_y - amount, ur_x - ll_x - 2*amount, amount, drawtype);
		    cmn.gdraw.fill_arc( ll_x, ll_y, 2*amount, 2*amount, 90, 90, 
					  drawtype);
		    cmn.gdraw.fill_arc( ll_x, ur_y - 2*amount, 2*amount, 2*amount, 180, 90, 
					  drawtype);
		    cmn.gdraw.fill_arc( ur_x - 2*amount, ur_y - 2*amount, 2*amount, 2*amount, 270, 90, 
					  drawtype);
		    cmn.gdraw.fill_arc( ur_x - 2*amount, ll_y, 2*amount, 2*amount, 0, 90, 
					  drawtype);
		}
		else {
		    int f1, f2;
		    double rotation;
		    if ( t != null)
			rotation = trf.rot( t);
		    else
			rotation = trf.rot();

		    if ( gradient_contrast >= 0) {
			f2 = GlowColor.shift_drawtype( fillcolor, -gradient_contrast/2 + chot, 0);
			f1 = GlowColor.shift_drawtype( fillcolor, (int)((float)(gradient_contrast)/2+0.6) + chot, 0);
		    }
		    else {
			f2 = GlowColor.shift_drawtype( fillcolor, -(int)((float)(gradient_contrast)/2-0.6) + chot, 0);
			f1 = GlowColor.shift_drawtype( fillcolor, gradient_contrast/2 + chot, 0);
		    }
		    cmn.gdraw.gradient_fill_rectrounded( ll_x, ll_y, ur_x - ll_x, ur_y - ll_y, amount,
							   fillcolor, f1, f2, cmn.gdraw.gradient_rotate( rotation, grad));	
		}
	    }
	    else {

		int drawtype_incr = shadow_contrast;
		if ( relief == Glow.eRelief_Down)
		    drawtype_incr = -shadow_contrast;

		// Draw light shadow
		drawtype = GlowColor.shift_drawtype( fillcolor, -drawtype_incr + chot, colornode);
		cmn.gdraw.fill_rect( ll_x + amount, ll_y, ur_x - ll_x - 2*amount, ish, drawtype);
		cmn.gdraw.fill_rect( ll_x, ll_y + amount, ish, ur_y - ll_y - 2*amount, drawtype);
		cmn.gdraw.fill_arc( ll_x, ll_y, 2*amount, 2*amount, 90, 90, 
				    drawtype);
		cmn.gdraw.fill_arc( ll_x, ur_y - 2*amount, 2*amount, 2*amount, 180, 45, 
				    drawtype);
		cmn.gdraw.fill_arc( ur_x - 2*amount, ll_y, 2*amount, 2*amount, 45, 45, 
				    drawtype);
		// Draw dark shadow
		drawtype = GlowColor.shift_drawtype( fillcolor, drawtype_incr + chot, colornode);
		cmn.gdraw.fill_rect( ll_x + amount, ur_y - ish, ur_x - ll_x - 2*amount, ish, drawtype);
		cmn.gdraw.fill_rect( ur_x - ish, ll_y + amount, ish, ur_y - ll_y - 2*amount, drawtype);
		cmn.gdraw.fill_arc( ll_x, ur_y - 2*amount, 2*amount, 2*amount, 225, 45, 
				    drawtype);
		cmn.gdraw.fill_arc( ur_x - 2*amount, ll_y, 2*amount, 2*amount, 0, 45, 
				    drawtype);
		cmn.gdraw.fill_arc( ur_x - 2*amount, ur_y - 2*amount, 2*amount, 2*amount, 270, 90, 
				    drawtype);

		if ( grad == Glow.eGradient_No) {
		    if ( chot != 0)
			drawtype = GlowColor.shift_drawtype( fillcolor, chot, 0);
		    else
			drawtype = fillcolor;
		    
		    if ( amount > ish) {

			cmn.gdraw.fill_rect( ll_x+ish, ll_y + amount, ur_x - ll_x-2*ish, ur_y - ll_y - 2*amount, drawtype);
			cmn.gdraw.fill_rect( ll_x + amount, ll_y+ish, ur_x - ll_x - 2*amount, amount-ish, drawtype);
			cmn.gdraw.fill_rect( ll_x + amount, ur_y - amount, ur_x - ll_x - 2*amount, amount-ish, drawtype);
			cmn.gdraw.fill_arc( ll_x+ish, ll_y+ish, 2*amount-2*ish, 2*amount-2*ish, 90, 90, 
					    drawtype);
			cmn.gdraw.fill_arc( ll_x+ish, ur_y - 2*amount+ish, 2*amount-2*ish, 2*amount-2*ish, 180, 90, 
					    drawtype);
			cmn.gdraw.fill_arc( ur_x - 2*amount+ish, ur_y - 2*amount+ish, 2*amount-2*ish, 2*amount-2*ish, 270, 90, 
					    drawtype);
			cmn.gdraw.fill_arc( ur_x - 2*amount+ish, ll_y+ish, 2*amount-2*ish, 2*amount-2*ish, 0, 90, 
					    drawtype);
		    }
		    else {
			cmn.gdraw.fill_rect( ll_x+amount, ll_y + amount, ur_x - ll_x-2*amount, ur_y - ll_y - 2*amount, drawtype);
		    }
		}
		else {
		    int f1, f2;
		    double rotation;
		    if ( t != null)
			rotation = trf.rot( t);
		    else
			rotation = trf.rot();

		    if ( gradient_contrast >= 0) {
			f2 = GlowColor.shift_drawtype( fillcolor, -gradient_contrast/2 + chot, 0);
			f1 = GlowColor.shift_drawtype( fillcolor, (int)((float)(gradient_contrast)/2+0.6) + chot, 0);
		    }
		    else {
			f2 = GlowColor.shift_drawtype( fillcolor, -(int)((float)(gradient_contrast)/2-0.6) + chot, 0);
			f1 = GlowColor.shift_drawtype( fillcolor, gradient_contrast/2 + chot, 0);
		    }
		    cmn.gdraw.gradient_fill_rectrounded( ll_x + ish, ll_y + ish, ur_x - ll_x - 2 * ish, ur_y - ll_y - 2 * ish,
							 amount - ish, fillcolor, f1, f2, cmn.gdraw.gradient_rotate( rotation, grad));	
		}
	    }
	}
	if ( border != 0 || fill == 0) {
	    drawtype = GlowColor.get_drawtype( draw_type, Glow.eDrawType_LineHighlight,
					       highlight, colornode, 0, 0);
	    int c = 0;
	    cmn.gdraw.line( ll_x + amount, ll_y, ur_x - amount, ll_y, drawtype, idx, 0);
	    cmn.gdraw.line( ll_x, ll_y-c + amount, ll_x, ur_y-c - amount, drawtype, idx, 0);
	    cmn.gdraw.line( ll_x + amount, ur_y, ur_x - amount, ur_y, drawtype, idx, 0);
	    cmn.gdraw.line( ur_x-c, ll_y + amount, ur_x-c, ur_y - amount, drawtype, idx, 0);
	    cmn.gdraw.arc( ll_x, ll_y, 2*amount, 2*amount, 90, 90, 
			   drawtype, idx, 0);
	    cmn.gdraw.arc( ll_x, ur_y - 2*amount, 2*amount, 2*amount, 180, 90, 
			   drawtype, idx, 0);
	    cmn.gdraw.arc( ur_x - 2*amount, ur_y - 2*amount, 2*amount, 2*amount, 270, 90, 
			   drawtype, idx, 0);
	    cmn.gdraw.arc( ur_x - 2*amount, ll_y, 2*amount, 2*amount, 0, 90, 
			   drawtype, idx, 0);
	}	
    }

    public void get_borders( GlowTransform t, GlowGeometry g) {
	double ll_x, ur_x, ll_y, ur_y, x1, x2, y1, y2;

	if ( t != null) {
	    x1 = trf.x( t, ll.x, ll.y);
	    x2 = trf.x( t, ur.x, ur.y);
	    y1 = trf.y( t, ll.x, ll.y);
	    y2 = trf.y( t, ur.x, ur.y);
	}
	else {
	    x1 = trf.x( ll.x, ll.y);
	    x2 = trf.x( ur.x, ur.y);
	    y1 = trf.y( ll.x, ll.y);
	    y2 = trf.y( ur.x, ur.y);
	}

	ll_x = Math.min( x1, x2);
	ur_x = Math.max( x1, x2);
	ll_y = Math.min( y1, y2);
	ur_y = Math.max( y1, y2);

	if ( ll_x < g.ll_x)
	    g.ll_x = ll_x;
	if ( ur_x > g.ur_x)
	    g.ur_x = ur_x;
	if ( ll_y < g.ll_y)
	    g.ll_y = ll_y;
	if ( ur_y > g.ur_y)
	    g.ur_y = ur_y;
    }

    public void get_node_borders() { 
	GlowGeometry g = new GlowGeometry();
	g.ll_x = g.ll_y = 1e37;
	g.ur_x = g.ur_y = -1e37;
	get_borders( null, g);
	x_left = g.ll_x;
	x_right = g.ur_x;
	y_low = g.ll_y;
	y_high = g.ur_y;
    }

    void set_scale( double scale_x, double scale_y, 
		    double x0, double y0, int type) {
	double old_x_left, old_x_right, old_y_low, old_y_high;

	if ( trf.s_a11 != 0 && trf.s_a22 != 0 &&
	     Math.abs( scale_x - trf.a11 / trf.s_a11) < Float.MIN_VALUE &&
	     Math.abs( scale_y - trf.a22 / trf.s_a22) < Float.MIN_VALUE)
	    return;

	switch( type) {
	case Glow.eScaleType_LowerLeft:
	    x0 = x_left;
	    y0 = y_low;
	    break;
	case Glow.eScaleType_LowerRight:
	    x0 = x_right;
	    y0 = y_low;
	    break;
	case Glow.eScaleType_UpperRight:
	    x0 = x_right;
	    y0 = y_high;
	    break;
	case Glow.eScaleType_UpperLeft:
	    x0 = x_left;
	    y0 = y_high;
	    break;
	case Glow.eScaleType_FixPoint:
	    break;
	case Glow.eScaleType_Center:
	    x0 = (x_left + x_right) / 2;
	    y0 = (y_low + y_high) /2;
	    break;
	default:
	    ;
	}
	
	old_x_left = x_left;
	old_x_right = x_right;
	old_y_low = y_low;
	old_y_high = y_high;
	trf.scale_from_stored( scale_x, scale_y, x0, y0);
	get_node_borders();

	switch( type) {
	case Glow.eScaleType_LowerLeft:
	    x_left = old_x_left;
	    y_low = old_y_low;
	    break;
	case Glow.eScaleType_LowerRight:
	    x_right = old_x_right;
	    y_low = old_y_low;
	    break;
	case Glow.eScaleType_UpperRight:
	    x_right = old_x_right;
	    y_high = old_y_high;
	    break;
	case Glow.eScaleType_UpperLeft:
	    x_left = old_x_left;
	    y_high = old_y_high;
	    break;
	case Glow.eScaleType_FixPoint:
	    break;
	case Glow.eScaleType_Center:
	    x0 = (x_left + x_right) / 2;
	    y0 = (y_low + y_high) /2;
	    break;
	default:
	    ;
	}
	cmn.ctx.draw( old_x_left * cmn.mw.zoom_factor_x - cmn.mw.offset_x - Glow.DRAW_MP,
		      old_y_low * cmn.mw.zoom_factor_y - cmn.mw.offset_y - Glow.DRAW_MP,
		      old_x_right * cmn.mw.zoom_factor_x - cmn.mw.offset_x + Glow.DRAW_MP,
		      old_y_high * cmn.mw.zoom_factor_y - cmn.mw.offset_y + Glow.DRAW_MP);
	cmn.ctx.draw( x_left * cmn.mw.zoom_factor_x - cmn.mw.offset_x - Glow.DRAW_MP,
		      y_low * cmn.mw.zoom_factor_y - cmn.mw.offset_y - Glow.DRAW_MP,
		      x_right * cmn.mw.zoom_factor_x - cmn.mw.offset_x + Glow.DRAW_MP,
		      y_high * cmn.mw.zoom_factor_y - cmn.mw.offset_y + Glow.DRAW_MP);
    }
}