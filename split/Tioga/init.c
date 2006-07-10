/* init.c */
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

VALUE rb_Integer_class, rb_Numeric_class;
ID save_dir_ID, model_number_ID, add_model_number_ID, quiet_mode_ID;
ID tex_preview_documentclass_ID, tex_preview_preamble_ID, tex_preview_pagestyle_ID;
ID tex_preview_paper_width_ID, tex_preview_paper_height_ID;
ID tex_preview_hoffset_ID, tex_preview_voffset_ID;
ID tex_preview_figure_width_ID, tex_preview_figure_height_ID, tex_preview_tiogafigure_command_ID;
ID tex_preview_fullpage_ID, tex_preview_minwhitespace_ID;
ID do_cmd_ID, initialized_ID, tex_xoffset_ID, tex_yoffset_ID;
ID tex_preview_fontsize_ID, tex_preview_fontfamily_ID, tex_preview_fontseries_ID, tex_preview_fontshape_ID;

void Init_IDs(void)
{
    
   rb_Numeric_class = rb_define_class("Numeric", rb_cObject);
   rb_Integer_class = rb_define_class("Integer", rb_Numeric_class);
	do_cmd_ID = rb_intern("do_cmd");
   // class variables
	initialized_ID = rb_intern("@@initialized");
	// instance variables
	save_dir_ID = rb_intern("@save_dir");
	model_number_ID = rb_intern("@model_number");
	add_model_number_ID = rb_intern("@add_model_number");
	quiet_mode_ID = rb_intern("@quiet_mode");    
	tex_xoffset_ID = rb_intern("@tex_xoffset");
	tex_yoffset_ID = rb_intern("@tex_yoffset");
    tex_preview_documentclass_ID = rb_intern("@tex_preview_documentclass");
    tex_preview_preamble_ID = rb_intern("@tex_preview_preamble");
    tex_preview_pagestyle_ID = rb_intern("@tex_preview_pagestyle");
    
    tex_preview_paper_width_ID = rb_intern("@tex_preview_paper_width");
    tex_preview_paper_height_ID = rb_intern("@tex_preview_paper_height");
    tex_preview_hoffset_ID = rb_intern("@tex_preview_hoffset");
    tex_preview_voffset_ID = rb_intern("@tex_preview_voffset");
    tex_preview_figure_width_ID = rb_intern("@tex_preview_figure_width");
    tex_preview_figure_height_ID = rb_intern("@tex_preview_figure_height");

    tex_preview_fullpage_ID = rb_intern("@tex_preview_fullpage");
    tex_preview_minwhitespace_ID = rb_intern("@tex_preview_minwhitespace");

    tex_preview_tiogafigure_command_ID = rb_intern("@tex_preview_tiogafigure_command");
    
    tex_preview_fontsize_ID = rb_intern("@tex_preview_fontsize");
    tex_preview_fontfamily_ID = rb_intern("@tex_preview_fontfamily");
    tex_preview_fontseries_ID = rb_intern("@tex_preview_fontseries");
    tex_preview_fontshape_ID = rb_intern("@tex_preview_fontshape");

}

void c_set_device_pagesize(FM *p, double width, double height) { // sizes in units of 1/720 inch
   p->page_left = 0;
   p->page_right = width;
   p->page_bottom = 0;
   p->page_top = height;
   p->page_width = p->page_right - p->page_left;
   p->page_height = p->page_top - p->page_bottom;
   p->clip_left = p->page_left;
   p->clip_right = p->page_right;
   p->clip_top = p->page_top;
   p->clip_bottom = p->page_bottom;
}


VALUE FM_set_device_pagesize(VALUE fmkr, VALUE width, VALUE height)
{
   FM *p = Get_FM(fmkr);
   width = rb_Float(width);
   height = rb_Float(height);
   c_set_device_pagesize(p, NUM2DBL(width), NUM2DBL(height));
   return fmkr;
}


void c_set_frame_sides(FM *p, double left, double right, double top, double bottom) { // sizes in page coords [0..1]
   if (left > 1.0 || left < 0.0) rb_raise(rb_eArgError, "Sorry: value of left must be between 0 and 1 for set_frame_sides");
   if (right > 1.0 || right < 0.0) rb_raise(rb_eArgError, "Sorry: value of right must be between 0 and 1 for set_frame_sides");
   if (top > 1.0 || top < 0.0) rb_raise(rb_eArgError, "Sorry: value of top must be between 0 and 1 for set_frame_sides");
   if (bottom > 1.0 || bottom < 0.0) rb_raise(rb_eArgError, "Sorry: value of bottom must be between 0 and 1 for set_frame_sides");
   if (left >= right) rb_raise(rb_eArgError, "Sorry: value of left must be smaller than value of right for set_frame_sides");
   if (bottom >= top) rb_raise(rb_eArgError, "Sorry: value of bottom must be smaller than value of top for set_frame_sides");
   p->frame_left = left;
   p->frame_right = right;
   p->frame_bottom = bottom;
   p->frame_top = top;
   p->frame_width = right - left;
   p->frame_height = top - bottom;
}

VALUE FM_set_frame_sides(VALUE fmkr, VALUE left, VALUE right, VALUE top, VALUE bottom)
{
   FM *p = Get_FM(fmkr);
   left = rb_Float(left);
   right = rb_Float(right);
   top = rb_Float(top);
   bottom = rb_Float(bottom);
   c_set_frame_sides(p, NUM2DBL(left), NUM2DBL(right), NUM2DBL(top), NUM2DBL(bottom));
   return fmkr;
}


void Initialize_Figure(VALUE fmkr) {
   FM *p = Get_FM(fmkr);
   /* Page */
   p->root_figure = true;
   p->in_subplot = false;
   c_private_set_default_font_size(p, 12.0);
   c_set_device_pagesize(p, 5 * 72.0 * ENLARGE, 5 * 72.0 * ENLARGE);
   /* default frame */
   c_set_frame_sides(p, 0.2, 0.8, 0.8, 0.2);
   /* default bounds */
   p->bounds_left = p->bounds_bottom = p->bounds_xmin = p->bounds_ymin = 0;
   p->bounds_right = p->bounds_top = p->bounds_xmax = p->bounds_ymax = 1;
   p->bounds_width = p->bounds_right - p->bounds_left;
   p->bounds_height = p->bounds_top - p->bounds_bottom;
   /* text attributes */
   p->justification = CENTERED;
   p->alignment = ALIGNED_AT_BASELINE;
   p->label_left_margin = 0; // as fraction of frame width
   p->label_right_margin = 0; // as fraction of frame width
   p->label_top_margin = 0; // as fraction of frame height
   p->label_bottom_margin = 0; // as fraction of frame height
   p->text_shift_on_left = 1.8;
   p->text_shift_on_right = 2.5;
   p->text_shift_on_top = 0.7;
   p->text_shift_on_bottom = 2.0;
   p->text_shift_from_x_origin = 1.8;
   p->text_shift_from_y_origin = 2.0;
   p->default_text_scale = 1.0;  Recalc_Font_Hts(p);
   /* graphics attributes */
   p->stroke_color = Qnil;
   p->fill_color = Qnil;
   p->default_line_scale = 1.0;
   p->line_width = 1.2;
   p->line_cap = LINE_CAP_ROUND;
   p->line_join = LINE_JOIN_ROUND;
   p->line_type = Qnil; // means solid line
   p->miter_limit = 2.0;
   
   p->stroke_opacity = 1.0;
   p->fill_opacity = 1.0;
   
   /* Title */
   p->title_visible = true;
   p->title = Qnil;
   p->title_side = TOP;
   p->title_position = 0.5;
   p->title_scale = 1.1;
   p->title_shift = 0.7; // in char heights, positive for out from edge (or toward larger x or y value)
   p->title_angle = 0.0;
   p->title_alignment = ALIGNED_AT_BASELINE;
   p->title_justification = CENTERED;
   p->title_color = Qnil;
   
   /* X label */
   p->xlabel_visible = true;
   p->xlabel = Qnil;
   p->xlabel_side = BOTTOM;
   p->xlabel_position = 0.5;
   p->xlabel_scale = 1.0;
   p->xlabel_shift = 2.0; // in char heights, positive for out from edge (or toward larger x or y value)
   p->xlabel_angle = 0.0;
   p->xlabel_alignment = ALIGNED_AT_BASELINE;
   p->xlabel_justification = CENTERED;
   p->xlabel_color = Qnil;
   
   /* Y label */
   p->ylabel_visible = true;
   p->ylabel = Qnil;
   p->ylabel_side = LEFT;
   p->ylabel_position = 0.5;
   p->ylabel_scale = 1.0;
   p->ylabel_shift = 1.8; // in char heights, positive for out from edge (or toward larger x or y value)
   p->ylabel_angle = 0.0;
   p->ylabel_alignment = ALIGNED_AT_BASELINE;
   p->ylabel_justification = CENTERED;
   p->ylabel_color = Qnil;
   
   /* X axis */
   p->xaxis_visible = true;
   p->xaxis_type = AXIS_WITH_TICKS_AND_NUMERIC_LABELS;
   p->xaxis_loc = BOTTOM;
   // line
   p->xaxis_line_width = 1.0; // for axis line
   p->xaxis_stroke_color = Qnil; // for axis line and tick marks
   // tick marks
   p->xaxis_major_tick_width = 0.9; // same units as line_width
   p->xaxis_minor_tick_width = 0.7; // same units as line_width
   p->xaxis_major_tick_length = 0.6; // in units of numeric label char heights
   p->xaxis_minor_tick_length = 0.3; // in units of numeric label char heights
   p->xaxis_log_values = false;
   p->xaxis_ticks_inside = true; // inside frame or toward larger x or y value for specific location
   p->xaxis_ticks_outside = false; // inside frame or toward smaller x or y value for specific location
   p->xaxis_tick_interval = 0.0; // set to 0 to use default
   p->xaxis_min_between_major_ticks = 2; // in units of numeric label char heights
   p->xaxis_number_of_minor_intervals = 0; // set to 0 to use default
   p->xaxis_locations_for_major_ticks = Qnil; // set to nil to use defaults
   p->xaxis_locations_for_minor_ticks = Qnil; // set to nil to use defaults
   // numeric labels on major ticks
   p->xaxis_use_fixed_pt = false;
   p->xaxis_digits_max = 0;
   p->xaxis_tick_labels = Qnil; // set to nil to use defaults. else must have a label for each major tick
   p->xaxis_numeric_label_decimal_digits = -1; // set to negative to use default
   p->xaxis_numeric_label_scale = 0.7;
   p->xaxis_numeric_label_shift = 0.3; // in char heights, positive for out from edge (or toward larger x or y value)
   p->xaxis_numeric_label_angle = 0.0;
   p->xaxis_numeric_label_alignment = ALIGNED_AT_MIDHEIGHT;
   p->xaxis_numeric_label_justification = CENTERED;
   p->top_edge_type = EDGE_WITH_TICKS;
   p->top_edge_visible = true;
   p->bottom_edge_type = EDGE_WITH_TICKS;
   p->bottom_edge_visible = true;
   
   /* Y axis */
   p->yaxis_visible = true;
   p->yaxis_type = AXIS_WITH_TICKS_AND_NUMERIC_LABELS;
   p->yaxis_loc = LEFT;
   // line
   p->yaxis_line_width = 1.0; // for axis line
   p->yaxis_stroke_color = Qnil; // for axis line and tick marks
   // tick marks
   p->yaxis_major_tick_width = 0.9; // same units as line_width
   p->yaxis_minor_tick_width = 0.7; // same units as line_width
   p->yaxis_major_tick_length = 0.6; // in units of numeric label char heights
   p->yaxis_minor_tick_length = 0.3; // in units of numeric label char heights
   p->yaxis_log_values = false;
   p->yaxis_ticks_inside = true; // inside frame or toward larger x or y value for specific location
   p->yaxis_ticks_outside = false; // inside frame or toward smaller x or y value for specific location
   p->yaxis_tick_interval = 0.0; // set to 0 to use default
   p->yaxis_min_between_major_ticks = 2; // in units of numeric label char heights
   p->yaxis_number_of_minor_intervals = 0; // set to 0 to use default
   p->yaxis_locations_for_major_ticks = Qnil; // set to nil to use defaults
   p->yaxis_locations_for_minor_ticks = Qnil; // set to nil to use defaults
   // numeric labels on major ticks
   p->yaxis_use_fixed_pt = false;
   p->yaxis_digits_max = 0;
   p->yaxis_tick_labels = Qnil; // set to nil to use defaults. else must have a label for each major tick
   p->yaxis_numeric_label_decimal_digits = -1; // set to negative to use default
   p->yaxis_numeric_label_scale = 0.7;
   p->yaxis_numeric_label_shift = 0.5; // in char heights, positive for out from edge (or toward larger x or y value)
   p->yaxis_numeric_label_angle = 0.0;
   p->yaxis_numeric_label_alignment = ALIGNED_AT_MIDHEIGHT;
   p->yaxis_numeric_label_justification = CENTERED;
   p->left_edge_type = EDGE_WITH_TICKS;
   p->left_edge_visible = true;
   p->right_edge_type = EDGE_WITH_TICKS;
   p->right_edge_visible = true;
   /* Legend */
   p->legend_line_x0 = 0.5;
   p->legend_line_x1 = 2.0;
   p->legend_line_dy = 0.4;
   p->legend_text_width = -1;
   p->legend_text_xstart = 2.8;
   p->legend_text_ystart = 2.0;
   p->legend_text_dy = 1.9;
   p->legend_line_width = -1;
   p->legend_scale = 0.6;
   p->legend_alignment = ALIGNED_AT_BASELINE;
   p->legend_justification = LEFT_JUSTIFIED;
   p->debug_verbosity_level = 0;
}

VALUE do_cmd(VALUE fmkr, VALUE cmd) { return rb_funcall(fmkr, do_cmd_ID, 1, cmd); }

static void Type_Error(VALUE obj, ID name_ID, char *expected)
{
   char *name = rb_id2name(name_ID);
   while (name[0] == '@') name++;
   rb_raise(rb_eArgError, "Require %s value for '%s'", expected, name);
}

bool Get_bool(VALUE obj, ID name_ID) {
   VALUE v = rb_ivar_get(obj, name_ID);
   if (v != Qfalse && v != Qtrue && v != Qnil)
      Type_Error(v, name_ID, "true or false");
   return v == Qtrue;
}

int Get_int(VALUE obj, ID name_ID) {
   VALUE v = rb_ivar_get(obj, name_ID);
   if (!rb_obj_is_kind_of(v,rb_Integer_class))
      Type_Error(v, name_ID, "Integer");
   v = rb_Integer(v);
   return NUM2INT(v);
}

double Get_double(VALUE obj, ID name_ID) {
   VALUE v = rb_ivar_get(obj, name_ID);
   if (!rb_obj_is_kind_of(v,rb_Numeric_class))
      Type_Error(v, name_ID, "Numeric");
   v = rb_Float(v);
   return NUM2DBL(v);
}


char *Get_tex_preview_paper_width(VALUE fmkr) {
   VALUE v = rb_ivar_get(fmkr, tex_preview_paper_width_ID);
   if (v == Qnil) return NULL;
   return StringValuePtr(v);
}

char *Get_tex_preview_paper_height(VALUE fmkr) {
   VALUE v = rb_ivar_get(fmkr, tex_preview_paper_height_ID);
   if (v == Qnil) return NULL;
   return StringValuePtr(v);
}

char *Get_tex_preview_hoffset(VALUE fmkr) {
   VALUE v = rb_ivar_get(fmkr, tex_preview_hoffset_ID);
   if (v == Qnil) return NULL;
   return StringValuePtr(v);
}

char *Get_tex_preview_voffset(VALUE fmkr) {
   VALUE v = rb_ivar_get(fmkr, tex_preview_voffset_ID);
   if (v == Qnil) return NULL;
   return StringValuePtr(v);
}

char *Get_tex_preview_figure_width(VALUE fmkr) {
   VALUE v = rb_ivar_get(fmkr, tex_preview_figure_width_ID);
   if (v == Qnil) return NULL;
   return StringValuePtr(v);
}

char *Get_tex_preview_figure_height(VALUE fmkr) {
   VALUE v = rb_ivar_get(fmkr, tex_preview_figure_height_ID);
   if (v == Qnil) return NULL;
   return StringValuePtr(v);
}


char *Get_tex_preview_fontsize(VALUE fmkr) {
   VALUE v = rb_ivar_get(fmkr, tex_preview_fontsize_ID);
   if (v == Qnil) return NULL;
   return StringValuePtr(v);
}

char *Get_tex_preview_fontfamily(VALUE fmkr) {
   VALUE v = rb_ivar_get(fmkr, tex_preview_fontfamily_ID);
   if (v == Qnil) return NULL;
   return StringValuePtr(v);
}

char *Get_tex_preview_fontseries(VALUE fmkr) {
   VALUE v = rb_ivar_get(fmkr, tex_preview_fontseries_ID);
   if (v == Qnil) return NULL;
   return StringValuePtr(v);
}

char *Get_tex_preview_fontshape(VALUE fmkr) {
   VALUE v = rb_ivar_get(fmkr, tex_preview_fontshape_ID);
   if (v == Qnil) return NULL;
   return StringValuePtr(v);
}

char *Get_tex_preview_minwhitespace(VALUE fmkr) {
   VALUE v = rb_ivar_get(fmkr, tex_preview_minwhitespace_ID);
   if (v == Qnil) return NULL;
   return StringValuePtr(v);
}

bool Get_tex_preview_fullpage(VALUE fmkr) {
   VALUE v = rb_ivar_get(fmkr, tex_preview_fullpage_ID);
   return v != Qfalse && v != Qnil;
}

/* gets the generated preamble */
char *Get_tex_preview_generated_preamble(VALUE fmkr) {
  /* it is a class constant... */
  VALUE v = rb_const_get(CLASS_OF(fmkr), 
			 rb_intern("TEX_PREAMBLE"));
  if (v == Qnil) return NULL;
  return StringValuePtr(v);
}


double Get_tex_xoffset(VALUE fmkr) { return Get_double(fmkr, tex_xoffset_ID); }
double Get_tex_yoffset(VALUE fmkr) { return Get_double(fmkr, tex_yoffset_ID); }

static char *Get_save_dir(VALUE fmkr) {
   VALUE v = rb_ivar_get(fmkr, save_dir_ID);
   if (v == Qnil) return NULL;
   return StringValuePtr(v);
}

char *Get_tex_preview_documentclass(VALUE fmkr) {
   VALUE v = rb_ivar_get(fmkr, tex_preview_documentclass_ID);
   if (v == Qnil) return NULL;
   return StringValuePtr(v);
}

char *Get_tex_preview_preamble(VALUE fmkr) {
   VALUE v = rb_ivar_get(fmkr, tex_preview_preamble_ID);
   if (v == Qnil) return NULL;
   return StringValuePtr(v);
}

char *Get_tex_preview_pagestyle(VALUE fmkr) {
   VALUE v = rb_ivar_get(fmkr, tex_preview_pagestyle_ID);
   if (v == Qnil) return NULL;
   return StringValuePtr(v);
}

char *Get_tex_preview_tiogafigure_command(VALUE fmkr) {
   VALUE v = rb_ivar_get(fmkr, tex_preview_tiogafigure_command_ID);
   if (v == Qnil) return NULL;
   return StringValuePtr(v);
}

static int Get_model_number(VALUE fmkr) {
   VALUE v = rb_ivar_get(fmkr, model_number_ID);
   if (v == Qnil) return 0;
   v = rb_Integer(v);
   return NUM2INT(v);
}

static bool Get_add_model_number(VALUE fmkr) {
   VALUE v = rb_ivar_get(fmkr, add_model_number_ID);
   return v != Qfalse && v != Qnil;
}

static bool Get_quiet_mode(VALUE fmkr) {
   VALUE v = rb_ivar_get(fmkr, quiet_mode_ID);
   return v != Qfalse && v != Qnil;
}

static bool Get_initialized() {
   VALUE v = rb_cvar_get(cFM, initialized_ID);
   return v != Qfalse && v != Qnil;
}

static void Set_initialized() {
   rb_cv_set(cFM, "@@initialized", Qtrue);
}

static void Make_Save_Fname(VALUE fmkr, char *full_name, char *f_name,
   bool with_save_dir, bool with_pdf_extension, bool add_mod_num) {
   int i, j, k, len, mod_len, mod_num, did_mod_num = false;
   char c, *fmt, model[STRLEN], *save=NULL;
   if (with_save_dir) save = Get_save_dir(fmkr);
   if (add_mod_num) {
      mod_num = Get_model_number(fmkr);
      fmt = ( mod_num < 10 )? "000%i" :
            ( mod_num < 100 )? "00%i" :
            ( mod_num < 1000 )? "0%i" : "%i";
      sprintf(model, fmt, mod_num);
      }
   if (with_save_dir && save != NULL && strlen(save) > 0) { 
      sprintf(full_name, "%s/", save); j = strlen(full_name); }
   else j = 0;
   if (f_name == NULL) f_name = "plot";
   len = strlen(f_name);
   for (i=0; i < len; i++) {
      c = f_name[i];
      if (add_mod_num && c == '.' && !did_mod_num) {
         mod_len = strlen(model); full_name[j++] = '_';
         for (k=0; k < mod_len; k++) full_name[j++] = model[k]; /* insert model number before . */
         did_mod_num = true;
         }
      full_name[j++] = c;
      }
   full_name[j] = '\0';
   char *dot = strrchr(full_name,'.');
   if (dot == NULL || strcmp(dot+1,"pdf") != 0) { /* add pdf extension */
      if (add_mod_num && !did_mod_num) {
         mod_len = strlen(model); full_name[j++] = '_';
         for (k=0; k < mod_len; k++) full_name[j++] = model[k]; /* insert model number at end */
      }
   full_name[j] = '\0';
   if (!with_pdf_extension) return;
   strcpy(full_name+j, ".pdf");
   }
}
   
VALUE FM_get_save_filename(VALUE fmkr, VALUE name) {
   char full_name[STRLEN];
   bool add_mod_num = Get_add_model_number(fmkr);
   Make_Save_Fname(fmkr, full_name, (name == Qnil)? NULL : StringValuePtr(name), false, false, add_mod_num);
   return rb_str_new2(full_name);
}
   
VALUE FM_private_make(VALUE fmkr, VALUE name, VALUE cmd) {
   char full_name[STRLEN], mod_num_name[STRLEN];
   FM *p = Get_FM(fmkr);
   FM saved = *p;
   VALUE result;
   bool add_mod_num = Get_add_model_number(fmkr);
   bool quiet = Get_quiet_mode(fmkr);
   if (!Get_initialized()) {
      Init_pdf();
      Init_tex();
      Set_initialized();
   }
   Make_Save_Fname(fmkr, full_name, (name == Qnil)? NULL : StringValuePtr(name), true, true, false);
   Open_pdf(fmkr, full_name, quiet);
   Open_tex(fmkr, full_name, quiet);
   Write_gsave();
   p->root_figure = true;
   p->in_subplot = false;
   result = rb_funcall(fmkr, do_cmd_ID, 1, cmd);
   Write_grestore();
   if (result == Qfalse) quiet = true;
   Close_pdf(fmkr, quiet);
   Close_tex(fmkr, quiet);
   if (!add_mod_num) {
      Create_wrapper(fmkr, full_name, quiet);   
   } else { // need to do this after make the plot so get correct model number
      Make_Save_Fname(fmkr, mod_num_name, (name == Qnil)? NULL : StringValuePtr(name), true, true, true);
      Rename_pdf(full_name, mod_num_name);
      Rename_tex(full_name, mod_num_name);
      Create_wrapper(fmkr, mod_num_name, quiet);   
   }
   *p = saved;
   return result;
}

