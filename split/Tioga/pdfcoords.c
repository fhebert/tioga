/* pdfcoords.c */
/*
   Copyright (C) 2005  Bill Paxton

   This file is part of Tioga.

   Tioga is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Library Public License as published
   by the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   Tioga is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with Tioga; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

#include "figures.h"
#include "pdfs.h"

/* Frame and Bounds */
void Recalc_Font_Hts(FM *p)
{
   double dx, dy;
   dx = dy = ENLARGE * p->default_font_size * p->default_text_scale;  // font height in output coords
   dx = convert_output_to_page_dx(p,dx);
   dx = convert_page_to_frame_dx(p,dx);
   p->default_text_height_dx = convert_frame_to_figure_dx(p,dx);
   dy = convert_output_to_page_dy(p,dy);
   dy = convert_page_to_frame_dy(p,dy);
   p->default_text_height_dy = convert_frame_to_figure_dy(p,dy);
}

static void c_set_subframe(FM *p, double left_margin, double right_margin, double top_margin, double bottom_margin)
{
   double x, y, w, h;
   if (left_margin < 0 || right_margin < 0 || top_margin < 0 || bottom_margin < 0)
      RAISE_ERROR("Sorry: margins for set_subframe must be non-negative");
   if (left_margin + right_margin >= 1.0)
      RAISE_ERROR_gg("Sorry: margins too large: left_margin (%g) right_margin (%g)", left_margin, right_margin);
   if (top_margin + bottom_margin >= 1.0)
      RAISE_ERROR_gg("Sorry: margins too large: top_margin (%g) bottom_margin (%g)", top_margin, bottom_margin);
   x = p->frame_left += left_margin * p->frame_width;
   p->frame_right -= right_margin * p->frame_width;
   p->frame_top -= top_margin * p->frame_height;
   y = p->frame_bottom += bottom_margin * p->frame_height;
   w = p->frame_width = p->frame_right - p->frame_left;
   h = p->frame_height = p->frame_top - p->frame_bottom;
   Recalc_Font_Hts(p);
}

OBJ_PTR FM_private_set_subframe(OBJ_PTR fmkr, OBJ_PTR left_margin, OBJ_PTR right_margin, OBJ_PTR top_margin, OBJ_PTR bottom_margin)
{
   FM *p = Get_FM(fmkr);
   c_set_subframe(p, Number_to_double(left_margin), Number_to_double(right_margin), Number_to_double(top_margin), Number_to_double(bottom_margin));
   return fmkr;
}


void c_private_set_default_font_size(FM *p, double size) {
    p->default_font_size = size;
    Recalc_Font_Hts(p);
}

OBJ_PTR FM_private_set_default_font_size(OBJ_PTR fmkr, OBJ_PTR size) {
   FM *p = Get_FM(fmkr);
   c_private_set_default_font_size(p, Number_to_double(size));
   return fmkr;
}



// We need to do some extra work to use 'ensure' around 'context'
// which is necessary in case code does 'return' from the block being executed in the context
// otherwise we won't get a chance to restore the state
typedef struct {
   OBJ_PTR fmkr;
   FM *p;
   FM saved;
   OBJ_PTR cmd;
} Context_Info;

static OBJ_PTR do_context_body(OBJ_PTR args)
{
   Context_Info *cp = (Context_Info *)args; // nasty but it works
   fprintf(TF, "q\n");
   return do_cmd(cp->fmkr, cp->cmd);
}

static OBJ_PTR do_context_ensure(OBJ_PTR args)
{
   Context_Info *cp = (Context_Info *)args;
   fprintf(TF, "Q\n");
   *cp->p = cp->saved;
   return Qnil; // what should we be returning here?
}

OBJ_PTR FM_private_context(OBJ_PTR fmkr, OBJ_PTR cmd)
{
   Context_Info c;
   c.fmkr = fmkr;
   c.cmd = cmd;
   c.p = Get_FM(fmkr);
   c.saved = *c.p;
   return rb_ensure(do_context_body, (OBJ_PTR)&c, do_context_ensure, (OBJ_PTR)&c);
}

OBJ_PTR FM_doing_subplot(OBJ_PTR fmkr)
{
   FM *p = Get_FM(fmkr);
   p->in_subplot = true;
   return fmkr;
}

OBJ_PTR FM_doing_subfigure(OBJ_PTR fmkr)
{
   FM *p = Get_FM(fmkr);
   p->root_figure = false;
   return fmkr;
}

static void c_set_bounds(FM *p, double left, double right, double top, double bottom)
{
   if (constructing_path) RAISE_ERROR("Sorry: must finish with current path before calling set_bounds");
   p->bounds_left = left; p->bounds_right = right;
   p->bounds_bottom = bottom; p->bounds_top = top;
   if (left < right) {
      p->xaxis_reversed = false;
      p->bounds_xmin = left; p->bounds_xmax = right;
   } else if (right < left) {
      p->xaxis_reversed = true;
      p->bounds_xmin = right; p->bounds_xmax = left;
   } else { // left == right
      p->xaxis_reversed = false;
      if (left > 0.0) {
        p->bounds_xmin = left * (1.0 - 1e-6); p->bounds_xmax = left * (1.0 + 1e-6);
      } else if (left < 0.0) {
        p->bounds_xmin = left * (1.0 + 1e-6); p->bounds_xmax = left * (1.0 - 1e-6);
      } else {
        p->bounds_xmin = -1e-6; p->bounds_xmax = 1e-6;
      }
   }
   if (bottom < top) {
      p->yaxis_reversed = false;
      p->bounds_ymin = bottom; p->bounds_ymax = top;
   } else if (top < bottom) {
      p->yaxis_reversed = true;
      p->bounds_ymin = top; p->bounds_ymax = bottom;
   } else { // top == bottom
      p->yaxis_reversed = false;
      if (bottom > 0.0) {
        p->bounds_ymin = bottom * (1.0 - 1e-6); p->bounds_ymax = bottom * (1.0 + 1e-6);
      } else if (bottom < 0.0) {
        p->bounds_ymin = bottom * (1.0 + 1e-6); p->bounds_ymax = bottom * (1.0 - 1e-6);
      } else {
        p->bounds_xmin = -1e-6; p->bounds_xmax = 1e-6;
      }
   }
   p->bounds_width = p->bounds_xmax - p->bounds_xmin;
   p->bounds_height = p->bounds_ymax - p->bounds_ymin;
   Recalc_Font_Hts(p);
}

OBJ_PTR FM_private_set_bounds(OBJ_PTR fmkr, OBJ_PTR left, OBJ_PTR right, OBJ_PTR top, OBJ_PTR bottom)
{
   FM *p = Get_FM(fmkr);
   double left_boundary, right_boundary, top_boundary, bottom_boundary;
   left_boundary = Number_to_double(left);
   right_boundary = Number_to_double(right);
   top_boundary = Number_to_double(top);
   bottom_boundary = Number_to_double(bottom);
   c_set_bounds(p, left_boundary, right_boundary, top_boundary, bottom_boundary);
   return fmkr;
}


// Conversions

static double c_convert_to_degrees(FM *p, double dx, double dy)  // dx and dy in figure coords
{
   double angle;
   if (dx == 0.0 && dy == 0.0) angle = 0.0;
   else if (dx > 0.0 && dy == 0.0) angle = (p->bounds_left > p->bounds_right)? 180.0 : 0.0;
   else if (dx < 0.0 && dy == 0.0) angle = (p->bounds_left > p->bounds_right)? 0.0 : 180.0;
   else if (dx == 0.0 && dy > 0.0) angle = (p->bounds_bottom > p->bounds_top)? -90.0 : 90.0;
   else if (dx == 0.0 && dy < 0.0) angle = (p->bounds_bottom > p->bounds_top)? 90.0 : -90.0;
   else angle = atan2(convert_figure_to_output_dy(p,dy),convert_figure_to_output_dx(p,dx))*RADIANS_TO_DEGREES;
   return angle;
}

OBJ_PTR FM_convert_to_degrees(OBJ_PTR fmkr, OBJ_PTR dx, OBJ_PTR dy) // dx and dy in figure coords
{
   FM *p = Get_FM(fmkr);
   return Float_New(c_convert_to_degrees(p, Number_to_double(dx), Number_to_double(dy)));
}



OBJ_PTR FM_convert_inches_to_output(OBJ_PTR fmkr, OBJ_PTR v)
{
   double val = Number_to_double(v);
   val = convert_inches_to_output(val);
   return Float_New(val);
}

OBJ_PTR FM_convert_output_to_inches(OBJ_PTR fmkr, OBJ_PTR v)
{
   double val = Number_to_double(v);
   val = convert_output_to_inches(val);
   return Float_New(val);
}


OBJ_PTR FM_convert_mm_to_output(OBJ_PTR fmkr, OBJ_PTR v)
{
   double val = Number_to_double(v);
   val = convert_mm_to_output(val);
   return Float_New(val);
}

OBJ_PTR FM_convert_output_to_mm(OBJ_PTR fmkr, OBJ_PTR v)
{
   double val = Number_to_double(v);
   val = convert_output_to_mm(val);
   return Float_New(val);
}


OBJ_PTR FM_convert_page_to_output_x(OBJ_PTR fmkr, OBJ_PTR v)
{
   FM *p = Get_FM(fmkr); double val = Number_to_double(v);
   val = convert_page_to_output_x(p,val);
   return Float_New(val);
}

OBJ_PTR FM_convert_page_to_output_y(OBJ_PTR fmkr, OBJ_PTR v)
{
   FM *p = Get_FM(fmkr); double val = Number_to_double(v);
   val = convert_page_to_output_y(p,val);
   return Float_New(val);
}

OBJ_PTR FM_convert_page_to_output_dx(OBJ_PTR fmkr, OBJ_PTR v)
{
   FM *p = Get_FM(fmkr); double val = Number_to_double(v);
   val = convert_page_to_output_dx(p,val);
   return Float_New(val);
}

OBJ_PTR FM_convert_page_to_output_dy(OBJ_PTR fmkr, OBJ_PTR v)
{
   FM *p = Get_FM(fmkr); double val = Number_to_double(v);
   val = convert_page_to_output_dy(p,val);
   return Float_New(val);
}

OBJ_PTR FM_convert_output_to_page_x(OBJ_PTR fmkr, OBJ_PTR v)
{
   FM *p = Get_FM(fmkr); double val = Number_to_double(v);
   val = convert_output_to_page_x(p,val);
   return Float_New(val);
}

OBJ_PTR FM_convert_output_to_page_y(OBJ_PTR fmkr, OBJ_PTR v)
{
   FM *p = Get_FM(fmkr); double val = Number_to_double(v);
   val = convert_output_to_page_y(p,val);
   return Float_New(val);
}

OBJ_PTR FM_convert_output_to_page_dx(OBJ_PTR fmkr, OBJ_PTR v)
{
   FM *p = Get_FM(fmkr); double val = Number_to_double(v);
   return convert_output_to_page_dx(p,val);
   return Float_New(val);
}

OBJ_PTR FM_convert_output_to_page_dy(OBJ_PTR fmkr, OBJ_PTR v)
{
   FM *p = Get_FM(fmkr); double val = Number_to_double(v);
   val = convert_output_to_page_dy(p,val);
   return Float_New(val);
}

OBJ_PTR FM_convert_frame_to_page_x(OBJ_PTR fmkr, OBJ_PTR v)
{
   FM *p = Get_FM(fmkr); double val = Number_to_double(v);
   val = convert_frame_to_page_x(p,val);
   return Float_New(val);
}

OBJ_PTR FM_convert_frame_to_page_y(OBJ_PTR fmkr, OBJ_PTR v)
{
   FM *p = Get_FM(fmkr); double val = Number_to_double(v);
   val = convert_frame_to_page_y(p,val);
   return Float_New(val);
}

OBJ_PTR FM_convert_frame_to_page_dx(OBJ_PTR fmkr, OBJ_PTR v)
{
   FM *p = Get_FM(fmkr); double val = Number_to_double(v);
   val = convert_frame_to_page_dx(p,val);
   return Float_New(val);
}

OBJ_PTR FM_convert_frame_to_page_dy(OBJ_PTR fmkr, OBJ_PTR v)
{
   FM *p = Get_FM(fmkr); double val = Number_to_double(v);
   val = convert_frame_to_page_dy(p,val);
   return Float_New(val);
}

OBJ_PTR FM_convert_page_to_frame_x(OBJ_PTR fmkr, OBJ_PTR v)
{
   FM *p = Get_FM(fmkr); double val = Number_to_double(v);
   val = convert_page_to_frame_x(p,val);
   return Float_New(val);
}

OBJ_PTR FM_convert_page_to_frame_y(OBJ_PTR fmkr, OBJ_PTR v)
{
   FM *p = Get_FM(fmkr); double val = Number_to_double(v);
   val = convert_page_to_frame_y(p,val);
   return Float_New(val);
}

OBJ_PTR FM_convert_page_to_frame_dx(OBJ_PTR fmkr, OBJ_PTR v)
{
   FM *p = Get_FM(fmkr); double val = Number_to_double(v);
   val = convert_page_to_frame_dx(p,val);
   return Float_New(val);
}

OBJ_PTR FM_convert_page_to_frame_dy(OBJ_PTR fmkr, OBJ_PTR v)
{
   FM *p = Get_FM(fmkr); double val = Number_to_double(v);
   val = convert_page_to_frame_dy(p,val);
   return Float_New(val);
}

OBJ_PTR FM_convert_figure_to_frame_x(OBJ_PTR fmkr, OBJ_PTR v)
{
   FM *p = Get_FM(fmkr); double val = Number_to_double(v);
   val = convert_figure_to_frame_x(p,val);
   return Float_New(val);
}

OBJ_PTR FM_convert_figure_to_frame_y(OBJ_PTR fmkr, OBJ_PTR v)
{
   FM *p = Get_FM(fmkr); double val = Number_to_double(v);
   val = convert_figure_to_frame_y(p,val);
   return Float_New(val);
}

OBJ_PTR FM_convert_figure_to_frame_dx(OBJ_PTR fmkr, OBJ_PTR v)
{
   FM *p = Get_FM(fmkr); double val = Number_to_double(v);
   val = convert_figure_to_frame_dx(p,val);
   return Float_New(val);
}

OBJ_PTR FM_convert_figure_to_frame_dy(OBJ_PTR fmkr, OBJ_PTR v)
{
   FM *p = Get_FM(fmkr); double val = Number_to_double(v);
   val = convert_figure_to_frame_dy(p,val);
   return Float_New(val);
}

OBJ_PTR FM_convert_frame_to_figure_x(OBJ_PTR fmkr, OBJ_PTR v)
{
   FM *p = Get_FM(fmkr); double val = Number_to_double(v);
   val = convert_frame_to_figure_x(p,val);
   return Float_New(val);
}

OBJ_PTR FM_convert_frame_to_figure_y(OBJ_PTR fmkr, OBJ_PTR v)
{
   FM *p = Get_FM(fmkr); double val = Number_to_double(v);
   val = convert_frame_to_figure_y(p,val);
   return Float_New(val);
}

OBJ_PTR FM_convert_frame_to_figure_dx(OBJ_PTR fmkr, OBJ_PTR v)
{
   FM *p = Get_FM(fmkr); double val = Number_to_double(v);
   val = convert_frame_to_figure_dx(p,val);
   return Float_New(val);
}

OBJ_PTR FM_convert_frame_to_figure_dy(OBJ_PTR fmkr, OBJ_PTR v)
{
   FM *p = Get_FM(fmkr); double val = Number_to_double(v);
   val = convert_frame_to_figure_dy(p,val);
   return Float_New(val);
}

double convert_figure_to_output_x(FM *p, double x)
{
   x = convert_figure_to_frame_x(p,x);
   x = convert_frame_to_page_x(p,x);
   x = convert_page_to_output_x(p,x);
   return x;
}

double convert_figure_to_output_dy(FM *p, double dy)
{
   dy = convert_figure_to_frame_dy(p,dy);
   dy = convert_frame_to_page_dy(p,dy);
   dy = convert_page_to_output_dy(p,dy);
   return dy;
}

double convert_figure_to_output_dx(FM *p, double dx)
{
   dx = convert_figure_to_frame_dx(p,dx);
   dx = convert_frame_to_page_dx(p,dx);
   dx = convert_page_to_output_dx(p,dx);
   return dx;
}

double convert_figure_to_output_y(FM *p, double y)
{
   y = convert_figure_to_frame_y(p,y);
   y = convert_frame_to_page_y(p,y);
   y = convert_page_to_output_y(p,y);
   return y;
}

OBJ_PTR FM_convert_figure_to_output_x(OBJ_PTR fmkr, OBJ_PTR v)
{
   FM *p = Get_FM(fmkr); double val = Number_to_double(v);
   return Float_New(convert_figure_to_output_x(p,val));
}

OBJ_PTR FM_convert_figure_to_output_y(OBJ_PTR fmkr, OBJ_PTR v)
{
   FM *p = Get_FM(fmkr); double val = Number_to_double(v);
   return Float_New(convert_figure_to_output_y(p,val));
}

OBJ_PTR FM_convert_figure_to_output_dx(OBJ_PTR fmkr, OBJ_PTR v)
{
   FM *p = Get_FM(fmkr); double val = Number_to_double(v);
   return Float_New(convert_figure_to_output_dx(p,val));
}

OBJ_PTR FM_convert_figure_to_output_dy(OBJ_PTR fmkr, OBJ_PTR v)
{
   FM *p = Get_FM(fmkr); double val = Number_to_double(v);
   return Float_New(convert_figure_to_output_dy(p,val));
}

double convert_output_to_figure_x(FM *p, double x)
{
   x = convert_output_to_page_x(p,x);
   x = convert_page_to_frame_x(p,x);
   x = convert_frame_to_figure_x(p,x);
   return x;
}

double convert_output_to_figure_dy(FM *p, double dy)
{
   dy = convert_output_to_page_dy(p,dy);
   dy = convert_page_to_frame_dy(p,dy);
   dy = convert_frame_to_figure_dy(p,dy);
   return dy;
}

double convert_output_to_figure_dx(FM *p, double dx)
{
   dx = convert_output_to_page_dx(p,dx);
   dx = convert_page_to_frame_dx(p,dx);
   dx = convert_frame_to_figure_dx(p,dx);
   return dx;
}

double convert_output_to_figure_y(FM *p, double y)
{
   y = convert_output_to_page_y(p,y);
   y = convert_page_to_frame_y(p,y);
   y = convert_frame_to_figure_y(p,y);
   return y;
}

OBJ_PTR FM_convert_output_to_figure_x(OBJ_PTR fmkr, OBJ_PTR v)
{
   FM *p = Get_FM(fmkr); double val = Number_to_double(v);
   return Float_New(convert_output_to_figure_x(p,val));
}

OBJ_PTR FM_convert_output_to_figure_y(OBJ_PTR fmkr, OBJ_PTR v)
{
   FM *p = Get_FM(fmkr); double val = Number_to_double(v);
   return Float_New(convert_output_to_figure_y(p,val));
}

OBJ_PTR FM_convert_output_to_figure_dx(OBJ_PTR fmkr, OBJ_PTR v)
{
   FM *p = Get_FM(fmkr); double val = Number_to_double(v);
   return Float_New(convert_output_to_figure_dx(p,val));
}

OBJ_PTR FM_convert_output_to_figure_dy(OBJ_PTR fmkr, OBJ_PTR v)
{
   FM *p = Get_FM(fmkr); double val = Number_to_double(v);
   return Float_New(convert_output_to_figure_dy(p,val));
}

