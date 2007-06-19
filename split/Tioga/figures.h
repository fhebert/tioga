/* figures.h */
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

#ifndef __figures_H__
#define __figures_H__

#include <namespace.h>

#include <math.h>
#include <stdbool.h>
#include <ctype.h>
#include "dvector.h"
#include "dtable.h"
#include "ruby.h"
#include "intern.h"
#include "generic.h"

#include <defs.h>

// Axes

#define NO_TICKS 0
#define MAJOR_TICKS_ONLY 1
#define MAJOR_AND_MINOR_TICKS 2

// Y Label Orientation

#define VERTICAL_BOTTOM_TO_TOP 0
#define HORIZONTAL 1
#define VERTICAL_TOP_TO_BOTTOM 2

// Frame Sides

#define LEFT -1
	// LEFT == LEFT_JUSTIFIED
#define RIGHT 1
	// RIGHT == RIGHT_JUSTIFIED
#define TOP 2
#define BOTTOM 3

// PlotAxis locations = Frame Sides plus the following

#define AT_X_ORIGIN 4
#define AT_Y_ORIGIN 5
    
// axis types

#define AXIS_HIDDEN 0
#define AXIS_LINE_ONLY 1
#define AXIS_WITH_MAJOR_TICKS_ONLY 2
#define AXIS_WITH_TICKS_ONLY 3
#define AXIS_WITH_MAJOR_TICKS_AND_NUMERIC_LABELS 4
#define AXIS_WITH_TICKS_AND_NUMERIC_LABELS 5

// edge types

#define EDGE_HIDDEN 0
#define EDGE_LINE_ONLY 1
#define EDGE_WITH_MAJOR_TICKS_ONLY 2
#define EDGE_WITH_TICKS 3

// Lines

#define LINE_CAP_BUTT 0
#define LINE_CAP_ROUND 1
#define LINE_CAP_SQUARE 2

#define LINE_JOIN_MITER 0
#define LINE_JOIN_ROUND 1
#define LINE_JOIN_BEVEL 2
    
// Text and Markers

#define LEFT_JUSTIFIED LEFT
#define CENTERED 0
#define RIGHT_JUSTIFIED RIGHT

#define ALIGNED_AT_TOP 0
#define ALIGNED_AT_MIDHEIGHT 1
#define ALIGNED_AT_BASELINE 2
#define ALIGNED_AT_BOTTOM 3

//  Rendering Modes

#define FILL 0
#define STROKE 1
#define FILL_AND_STROKE 2
#define DISCARD 3 
#define FILL_AND_CLIP 4
#define STROKE_AND_CLIP 5
#define FILL_STROKE_AND_CLIP 6
#define CLIP 7

typedef struct {

/* page attributes */
    bool root_figure;
    bool in_subplot;
    double page_left, page_bottom, page_top, page_right, page_width, page_height;
        // in output units.    READ ONLY from Ruby.
    
/* frame and bounds attributes.  READ ONLY from Ruby.  */
    // frame location as fractions of page width & height.
    double frame_left, frame_right, frame_top, frame_bottom, frame_width, frame_height;
    // bounds in figure coords.
    double bounds_left, bounds_right, bounds_top, bounds_bottom;
    double bounds_xmin, bounds_xmax, bounds_ymin, bounds_ymax;
    double bounds_width, bounds_height;
    bool xaxis_reversed, yaxis_reversed;
    
/* text attributes */
    int justification;
    int alignment;
    
    double default_font_size; // in points
    double default_text_scale;
    double default_text_height_dx; // in figure coords.  READ ONLY from Ruby.  set when change default_text_scale.
    double default_text_height_dy; // in figure coords.  READ ONLY from Ruby.  set when change default_text_scale.
    
    double label_left_margin; // as fraction of frame width
    double label_right_margin; // as fraction of frame width
    double label_top_margin; // as fraction of frame height
    double label_bottom_margin; // as fraction of frame height
    
    double text_shift_on_left; // default shift for text on LEFT side of frame
    double text_shift_on_right; // default shift for text on RIGHT side of frame
    double text_shift_on_top; // default shift for text on TOP side of frame
    double text_shift_on_bottom; // default shift for text on BOTTOM side of frame
    double text_shift_from_x_origin; // default shift for text away from y axis at x origin
    double text_shift_from_y_origin; // default shift for text away from x axis at y origin
    
/* graphics attributes */

    /* color attributes */
    double stroke_color_R;
    double stroke_color_G;
    double stroke_color_B;
    double fill_color_R;
    double fill_color_G;
    double fill_color_B;
    
    /* stroke attributes */
    double default_line_scale; // multipler for line_width
    double line_width;  // in points (prior to multiplication by default_line_scale)
    int line_cap;
    int line_join;
    double miter_limit;
    
    /* transparency */
    double stroke_opacity;
    double fill_opacity;
    
    /* Title */
    bool title_visible;
    int title_side;
    double title_position;
    
    double title_scale;
    double title_shift; // in char heights, positive for out from edge (or toward larger x or y value)
    double title_angle;
    int title_alignment;
    int title_justification;
    double title_color_R;
    double title_color_G;
    double title_color_B;
    
    /* X label */
    bool xlabel_visible;
    double xlabel_position; // xlabel side is given by xaxis_loc
    
    double xlabel_scale;
    double xlabel_shift; // in char heights, positive for out from edge (or toward larger x or y value)
    double xlabel_angle;
    int xlabel_side;
    int xlabel_alignment;
    int xlabel_justification;
    double xlabel_color_R;
    double xlabel_color_G;
    double xlabel_color_B;
    
    /* Y label */
    bool ylabel_visible;
    double ylabel_position; // ylabel side is given by yaxis_loc
    
    double ylabel_scale;
    double ylabel_shift; // in char heights, positive for out from edge (or toward larger x or y value)
    double ylabel_angle;
    int ylabel_side;
    int ylabel_alignment;
    int ylabel_justification;
    double ylabel_color_R;
    double ylabel_color_G;
    double ylabel_color_B;
    
    /* X axis */
    bool xaxis_visible;
    int xaxis_type;
    int xaxis_loc;
    // line
    double xaxis_line_width; // for axis line
    double xaxis_stroke_color_R; // for axis line and tick marks
    double xaxis_stroke_color_G;
    double xaxis_stroke_color_B;
    // tick marks
    double xaxis_major_tick_width; // same units as line_width
    double xaxis_minor_tick_width; // same units as line_width
    double xaxis_major_tick_length; // in units of numeric label char heights
    double xaxis_minor_tick_length; // in units of numeric label char heights
    bool xaxis_log_values;
    bool xaxis_ticks_inside; // inside frame or toward larger x or y value for specific location
    bool xaxis_ticks_outside; // inside frame or toward smaller x or y value for specific location
    double xaxis_tick_interval; // set to 0 to use default
    double xaxis_min_between_major_ticks; // in units of numeric label char heights
    int xaxis_number_of_minor_intervals; // set to 0 to use default
    // numeric labels on major ticks
    bool xaxis_use_fixed_pt;
    int xaxis_digits_max;
    int xaxis_numeric_label_decimal_digits; // set to negative to use default
    double xaxis_numeric_label_scale;
    double xaxis_numeric_label_shift; // in char heights, positive for out from edge (or toward larger x or y value)
    double xaxis_numeric_label_angle;
    int xaxis_numeric_label_alignment;
    int xaxis_numeric_label_justification;
    int xaxis_numeric_label_frequency;
    int xaxis_numeric_label_phase;
    // note that the top and bottom edges and drawn in addition to the x axis.
    // if the edge type specifies ticks, the ticks match those of the x axis.
    bool top_edge_visible;
    int top_edge_type;
    bool bottom_edge_visible;
    int bottom_edge_type;
    
    /* Y axis */
    bool yaxis_visible;
    int yaxis_type;
    int yaxis_loc;
    // line
    double yaxis_line_width; // for axis line
    double yaxis_stroke_color_R; // for axis line and tick marks
    double yaxis_stroke_color_G;
    double yaxis_stroke_color_B;
    // tick marks
    double yaxis_major_tick_width; // same units as line_width
    double yaxis_minor_tick_width; // same units as line_width
    double yaxis_major_tick_length; // in units of numeric label char heights
    double yaxis_minor_tick_length; // in units of numeric label char heights
    bool yaxis_log_values;
    bool yaxis_ticks_inside; // inside frame or toward larger x or y value for specific location
    bool yaxis_ticks_outside; // inside frame or toward smaller x or y value for specific location
    double yaxis_tick_interval; // set to 0 to use default
    double yaxis_min_between_major_ticks; // in units of numeric label char heights
    int yaxis_number_of_minor_intervals; // set to 0 to use default
    // numeric labels on major ticks
    bool yaxis_use_fixed_pt;
    int yaxis_digits_max;
    int yaxis_numeric_label_decimal_digits; // set to negative to use default
    double yaxis_numeric_label_scale;
    double yaxis_numeric_label_shift; // in char heights, positive for out from edge (or toward larger x or y value)
    double yaxis_numeric_label_angle;
    int yaxis_numeric_label_alignment;
    int yaxis_numeric_label_justification;
    int yaxis_numeric_label_frequency;
    int yaxis_numeric_label_phase;
    // note that the left and right edges and drawn in addition to the y axis.
    // if the edge type specifies ticks, the ticks match those of the y axis.
    int left_edge_type;
    bool left_edge_visible;
    int right_edge_type;
    bool right_edge_visible;
    
    /* Legend */
    // units are text heights for x and y for locations and sizes
    double legend_line_x0; // x location of start of line
    double legend_line_x1; // x location of end of line
    double legend_line_dy; // move the line up by this amount from the text baseline
    double legend_text_xstart; // x location of start of text
    double legend_text_width; // in text heights measured from legend_text_xstart
    double legend_text_ystart; // y location of first entry text
    double legend_text_dy; // move down by this amount for subsequent legend entries
    double legend_line_width; // for legend lines (or negative to use the weight from the plot)
    double legend_scale; // scaling factor for size of legend text
    int legend_alignment;
    int legend_justification;
    
    /* Debugging */
    int debug_verbosity_level; // 0 turns off the trace info

    /* Whether to complain about out non ok numbers in paths */
    int croak_on_nonok_numbers;
    
/* PRIVATE -- not to be included in the Ruby interface */
    double clip_left, clip_right, clip_top, clip_bottom; // in output coords
    VALUE fm;
} FM;

typedef FM Figure_Maker;

extern double bbox_llx, bbox_lly, bbox_urx, bbox_ury;

extern VALUE cFM; /* class object for FigureMaker */
extern char *data_dir;


/*======================================================================*/
// axes.c
extern void figure_moveto(FM *p, double x, double y); // figure coords
extern void figure_lineto(FM *p, double x, double y); // figure coords
extern void figure_join(FM *p, double x0, double y0, double x1, double y1); // figure coords
extern void figure_join_and_stroke(FM *p, double x0, double y0, double x1, double y1); // figure coords
extern char *Get_String(VALUE ary, int index);
extern VALUE FM_show_axis(VALUE fmkr, VALUE loc);
extern VALUE FM_show_edge(VALUE fmkr, VALUE loc);
extern VALUE FM_no_title(VALUE fmkr);
extern VALUE FM_no_xlabel(VALUE fmkr);
extern VALUE FM_no_ylabel(VALUE fmkr);
extern VALUE FM_no_xaxis(VALUE fmkr);
extern VALUE FM_no_yaxis(VALUE fmkr);
extern VALUE FM_no_left_edge(VALUE fmkr);
extern VALUE FM_no_right_edge(VALUE fmkr);
extern VALUE FM_no_top_edge(VALUE fmkr);
extern VALUE FM_no_bottom_edge(VALUE fmkr);

/*======================================================================*/
// figures.c
extern FM *Get_FM(VALUE fmkr);
extern bool Is_FM(VALUE fmkr);
void Init_FigureMaker(void);

/*======================================================================*/
// init.c
extern void Init_IDs(void);
extern VALUE FM_set_device_pagesize(VALUE fmkr, VALUE width, VALUE height); // size in output coords (decipoints)
extern VALUE FM_set_frame_sides(VALUE fmkr, VALUE left, VALUE right, VALUE top, VALUE bottom); // in page coords [0..1]
extern void Initialize_Figure(VALUE fmkr);
extern VALUE do_cmd(VALUE fmkr, VALUE cmd);
extern bool Get_bool(VALUE obj, ID name_ID);
extern int Get_int(VALUE obj, ID name_ID);
extern double Get_double(VALUE obj, ID name_ID); // for instance variables of the obj
extern char *Get_tex_preview_paper_width(VALUE fmkr);
extern char *Get_tex_preview_paper_height(VALUE fmkr);
extern char *Get_tex_preview_hoffset(VALUE fmkr);
extern char *Get_tex_preview_voffset(VALUE fmkr);
extern char *Get_tex_preview_figure_width(VALUE fmkr);
extern char *Get_tex_preview_figure_height(VALUE fmkr);
extern char *Get_tex_fontsize(VALUE fmkr);
extern char *Get_tex_fontfamily(VALUE fmkr);
extern char *Get_tex_fontseries(VALUE fmkr);
extern char *Get_tex_fontshape(VALUE fmkr);
extern char *Get_tex_preview_minwhitespace(VALUE fmkr);
extern bool Get_tex_preview_fullpage(VALUE fmkr);
extern char *Get_tex_preview_tiogafigure_command(VALUE fmkr);
extern char *Get_tex_preview_generated_preamble(VALUE fmkr);
extern double Get_tex_xoffset(VALUE fmkr);
extern double Get_tex_yoffset(VALUE fmkr);
extern char *Get_tex_preview_documentclass(VALUE fmkr);
extern char *Get_tex_preamble(VALUE fmkr);
extern char *Get_tex_xaxis_numeric_label(VALUE fmkr);
extern char *Get_tex_yaxis_numeric_label(VALUE fmkr);
extern char *Get_tex_preview_pagestyle(VALUE fmkr);
extern char *Get_tex_preview_tiogafigure_command(VALUE fmkr);
extern VALUE Get_line_type(VALUE fmkr);
extern void Set_line_type(VALUE fmkr, VALUE v);
extern VALUE Get_xaxis_tick_labels(VALUE fmkr);
extern VALUE Get_xaxis_locations_for_major_ticks(VALUE fmkr);
extern VALUE Get_xaxis_locations_for_minor_ticks(VALUE fmkr);
extern VALUE Get_yaxis_tick_labels(VALUE fmkr);
extern VALUE Get_yaxis_locations_for_major_ticks(VALUE fmkr);
extern VALUE Get_yaxis_locations_for_minor_ticks(VALUE fmkr);
extern VALUE FM_get_save_filename(VALUE fmkr, VALUE name);
extern VALUE FM_private_make(VALUE fmkr, VALUE name, VALUE cmd);
extern VALUE FM_private_make_portfolio(VALUE fmkr, VALUE name, VALUE fignums, VALUE fignames);

/*======================================================================*/
// makers.c
extern VALUE FM_private_make_contour(VALUE fmkr,
     VALUE dest_xs, VALUE dest_ys, VALUE gaps, // these Dvectors get the results
     VALUE xs, VALUE ys, // data x coordinates and y coordinates
     VALUE zs, VALUE z_level, // the Dtable of values and the desired contour level
     VALUE legit, // the Dtable of flags (nonzero means okay)
     VALUE method // int == 1 means use CONREC
     );
extern VALUE FM_private_make_steps(VALUE fmkr, VALUE Xdata, VALUE Ydata,
    VALUE xfirst, VALUE yfirst, VALUE xlast, VALUE ylast);
extern VALUE FM_private_make_spline_interpolated_points(VALUE fmkr, VALUE Xvec, VALUE Yvec, VALUE Xdata, VALUE Ydata,
    VALUE start_slope, VALUE end_slope);

/*======================================================================*/
// pdfcolor.c
extern void Free_Functions();
extern void Write_Functions(void);
extern void Free_Stroke_Opacities(void);
extern VALUE FM_stroke_opacity_set(VALUE fmkr, VALUE value);
extern void Free_Fill_Opacities(void);
extern VALUE FM_fill_opacity_set(VALUE fmkr, VALUE value);
extern void Write_Stroke_Opacity_Objects(void);
extern void Write_Fill_Opacity_Objects(void);
extern void Free_Shadings();
extern void Write_Shadings(void);
extern VALUE FM_private_axial_shading(VALUE fmkr, VALUE x0, VALUE y0,
        VALUE x1, VALUE y1, VALUE colormap, VALUE extend_start, VALUE extend_end);
extern VALUE FM_private_radial_shading(VALUE fmkr, VALUE x0, VALUE y0, VALUE r0,
        VALUE x1, VALUE y1, VALUE r1, VALUE colormap,
        VALUE a, VALUE b, VALUE c, VALUE d,
        VALUE extend_start, VALUE extend_end);
extern VALUE FM_private_create_colormap(VALUE fmkr, VALUE rgb_flag,
            VALUE length, VALUE Ps, VALUE C1s, VALUE C2s, VALUE C3s);
extern VALUE FM_get_color_from_colormap(VALUE fmkr, VALUE color_map, VALUE color_position);
extern VALUE FM_convert_to_colormap(VALUE fmkr, VALUE Rs, VALUE Gs, VALUE Bs);
extern VALUE FM_hls_to_rgb(VALUE fmkr, VALUE hls_vec);
extern VALUE FM_rgb_to_hls(VALUE fmkr, VALUE rgb_vec);
extern VALUE FM_title_color_set(VALUE fmkr, VALUE value);
extern VALUE FM_title_color_get(VALUE fmkr);
extern VALUE FM_xlabel_color_get(VALUE fmkr);
extern VALUE FM_xlabel_color_set(VALUE fmkr, VALUE value);
extern VALUE FM_ylabel_color_get(VALUE fmkr);
extern VALUE FM_ylabel_color_set(VALUE fmkr, VALUE value);
extern VALUE FM_xaxis_stroke_color_get(VALUE fmkr);
extern VALUE FM_xaxis_stroke_color_set(VALUE fmkr, VALUE value);
extern VALUE FM_yaxis_stroke_color_get(VALUE fmkr);
extern VALUE FM_yaxis_stroke_color_set(VALUE fmkr, VALUE value);

/*======================================================================*/
// pdfcoords.c
extern void Recalc_Font_Hts(FM *p);
extern VALUE FM_private_set_subframe(VALUE fmkr, VALUE left_margin, VALUE right_margin, VALUE top_margin, VALUE bottom_margin);
extern void c_private_set_default_font_size(FM *p, double size);
extern VALUE FM_private_set_default_font_size(VALUE fmkr, VALUE size); // size in points
extern VALUE FM_private_context(VALUE fmkr, VALUE cmd);
extern VALUE FM_doing_subplot(VALUE fmkr);
extern VALUE FM_doing_subfigure(VALUE fmkr);
extern VALUE FM_private_set_bounds(VALUE fmkr, VALUE left, VALUE right, VALUE top, VALUE bottom); /* in figure coords */
extern VALUE FM_convert_to_degrees(VALUE fmkr, VALUE dx, VALUE dy);
extern VALUE FM_convert_inches_to_output(VALUE fmkr, VALUE value);
extern VALUE FM_convert_output_to_inches(VALUE fmkr, VALUE value);
extern VALUE FM_convert_mm_to_output(VALUE fmkr, VALUE value);
extern VALUE FM_convert_output_to_mm(VALUE fmkr, VALUE value);
extern VALUE FM_convert_page_to_output_x(VALUE fmkr, VALUE value);
extern VALUE FM_convert_page_to_output_y(VALUE fmkr, VALUE value);
extern VALUE FM_convert_page_to_output_dx(VALUE fmkr, VALUE value);
extern VALUE FM_convert_page_to_output_dy(VALUE fmkr, VALUE value);
extern VALUE FM_convert_output_to_page_x(VALUE fmkr, VALUE value);
extern VALUE FM_convert_output_to_page_y(VALUE fmkr, VALUE value);
extern VALUE FM_convert_output_to_page_dx(VALUE fmkr, VALUE value);
extern VALUE FM_convert_output_to_page_dy(VALUE fmkr, VALUE value);
extern VALUE FM_convert_frame_to_page_x(VALUE fmkr, VALUE value);
extern VALUE FM_convert_frame_to_page_y(VALUE fmkr, VALUE value);
extern VALUE FM_convert_frame_to_page_dx(VALUE fmkr, VALUE value);
extern VALUE FM_convert_frame_to_page_dy(VALUE fmkr, VALUE value);
extern VALUE FM_convert_page_to_frame_x(VALUE fmkr, VALUE value);
extern VALUE FM_convert_page_to_frame_y(VALUE fmkr, VALUE value);
extern VALUE FM_convert_page_to_frame_dx(VALUE fmkr, VALUE value);
extern VALUE FM_convert_page_to_frame_dy(VALUE fmkr, VALUE value);
extern VALUE FM_convert_figure_to_frame_x(VALUE fmkr, VALUE value);
extern VALUE FM_convert_figure_to_frame_y(VALUE fmkr, VALUE value);
extern VALUE FM_convert_figure_to_frame_dx(VALUE fmkr, VALUE value);
extern VALUE FM_convert_figure_to_frame_dy(VALUE fmkr, VALUE value);
extern VALUE FM_convert_frame_to_figure_x(VALUE fmkr, VALUE value);
extern VALUE FM_convert_frame_to_figure_y(VALUE fmkr, VALUE value);
extern VALUE FM_convert_frame_to_figure_dx(VALUE fmkr, VALUE value);
extern VALUE FM_convert_frame_to_figure_dy(VALUE fmkr, VALUE value);
extern double convert_figure_to_output_x(FM *p, double x);
extern double convert_figure_to_output_y(FM *p, double y);
extern double convert_figure_to_output_dy(FM *p, double dy);
extern double convert_figure_to_output_dx(FM *p, double dx);
extern VALUE FM_convert_figure_to_output_x(VALUE fmkr, VALUE value);
extern VALUE FM_convert_figure_to_output_y(VALUE fmkr, VALUE value);
extern VALUE FM_convert_figure_to_output_dx(VALUE fmkr, VALUE value);
extern VALUE FM_convert_figure_to_output_dy(VALUE fmkr, VALUE value);
extern double convert_output_to_figure_x(FM *p, double x);
extern double convert_output_to_figure_y(FM *p, double y);
extern double convert_output_to_figure_dy(FM *p, double dy);
extern double convert_output_to_figure_dx(FM *p, double dx);
extern VALUE FM_convert_output_to_figure_x(VALUE fmkr, VALUE value);
extern VALUE FM_convert_output_to_figure_y(VALUE fmkr, VALUE value);
extern VALUE FM_convert_output_to_figure_dx(VALUE fmkr, VALUE value);
extern VALUE FM_convert_output_to_figure_dy(VALUE fmkr, VALUE value);

/*======================================================================*/
// pdffile.c
extern void Init_pdf(void);
extern void Record_Object_Offset(int obj_number);
extern void Open_pdf(VALUE fmkr, char *filename, bool quiet_mode);
extern void Start_Axis_Standard_State(FM *p, double r, double g, double b, double line_width);
extern void End_Axis_Standard_State(void);
extern void Write_gsave(void);
extern void Write_grestore(void);
extern void Close_pdf(VALUE fmkr, bool quiet_mode);
extern void Rename_pdf(char *oldname, char *newname);

/*======================================================================*/
// pdfimage.c
extern VALUE FM_private_show_jpg(VALUE fmkr, VALUE filename, 
   VALUE width, VALUE height, VALUE image_destination, VALUE mask_xo_num);
extern VALUE FM_private_create_image_data(VALUE fmkr, VALUE data,
   VALUE first_row, VALUE last_row, VALUE first_column, VALUE last_column,
   VALUE min_value, VALUE max_value, VALUE max_code, VALUE if_below_range, VALUE if_above_range);
extern VALUE FM_private_create_monochrome_image_data(VALUE fmkr, VALUE data,
   VALUE first_row, VALUE last_row, VALUE first_column, VALUE last_column,
   VALUE boundary, VALUE reverse);
extern VALUE FM_private_show_image(VALUE fmkr, VALUE llx, VALUE lly, VALUE lrx, VALUE lry, VALUE ulx, VALUE uly,
   VALUE interpolate, VALUE w, VALUE h, VALUE data, VALUE value_mask_min, VALUE value_mask_max,
   VALUE hival, VALUE lookup, VALUE mask_xo_num);
extern VALUE FM_private_show_rgb_image(VALUE fmkr, 
   VALUE llx, VALUE lly, VALUE lrx, VALUE lry, VALUE ulx, VALUE uly,
   VALUE interpolate, VALUE w, VALUE h, VALUE data, VALUE mask_xo_num);
extern VALUE FM_private_show_cmyk_image(VALUE fmkr, 
   VALUE llx, VALUE lly, VALUE lrx, VALUE lry, VALUE ulx, VALUE uly,
   VALUE interpolate, VALUE w, VALUE h, VALUE data, VALUE mask_xo_num);
extern VALUE FM_private_show_grayscale_image(VALUE fmkr, 
   VALUE llx, VALUE lly, VALUE lrx, VALUE lry, VALUE ulx, VALUE uly,
   VALUE interpolate, VALUE w, VALUE h, VALUE data, VALUE mask_xo_num);
extern VALUE FM_private_show_monochrome_image(VALUE fmkr, VALUE llx, VALUE lly, VALUE lrx, VALUE lry, VALUE ulx, VALUE uly,
   VALUE interpolate, VALUE reversed, VALUE w, VALUE h, VALUE data, VALUE mask_xo_num);

/*======================================================================*/
// pdfpath.c
extern void Unpack_RGB(VALUE rgb, double *rp, double *gp, double *bp);
void c_stroke_color_set(FM *p, double r, double g, double b);
extern VALUE FM_stroke_color_set(VALUE fmkr, VALUE value);
extern VALUE FM_stroke_color_get(VALUE fmkr);
void c_fill_color_set(FM *p, double r, double g, double b);
extern VALUE FM_fill_color_set(VALUE fmkr, VALUE value);
extern VALUE FM_fill_color_get(VALUE fmkr);
void c_line_width_set(FM *p, double line_width);
extern VALUE FM_line_width_set(VALUE fmkr, VALUE value);
extern VALUE FM_rescale_lines(VALUE fmkr, VALUE scaling);
void c_line_cap_set(FM *p, int line_cap);
extern VALUE FM_line_cap_set(VALUE fmkr, VALUE value);
void c_line_join_set(FM *p, int line_join);
extern VALUE FM_line_join_set(VALUE fmkr, VALUE value);
void c_miter_limit_set(FM *p, double miter_limit);
extern VALUE FM_miter_limit_set(VALUE fmkr, VALUE value);
extern VALUE FM_line_type_set(VALUE fmkr, VALUE line_type);
void update_bbox(FM *p, double x, double y);
extern VALUE FM_update_bbox(VALUE fmkr, VALUE x, VALUE y);
extern VALUE FM_bbox_left(VALUE fmkr);
extern VALUE FM_bbox_right(VALUE fmkr);
extern VALUE FM_bbox_top(VALUE fmkr);
extern VALUE FM_bbox_bottom(VALUE fmkr);
extern void c_moveto(FM *p, double x, double y);
extern VALUE FM_move_to_point(VALUE fmkr, VALUE x, VALUE y);  // x y m
extern void c_lineto(FM *p, double x, double y);
extern VALUE FM_append_point_to_path(VALUE fmkr, VALUE x, VALUE y);  // x y l
extern void c_curveto(FM *p, double x1, double y1, double x2, double y2, double x3, double y3);
extern VALUE FM_append_curve_to_path(VALUE fmkr, 
   VALUE x1, VALUE y1, VALUE x2, VALUE y2, VALUE x3, VALUE y3); 
extern VALUE FM_close_path(VALUE fmkr); // h
extern VALUE FM_append_arc_to_path(VALUE fmkr, VALUE x_start, VALUE y_start, VALUE x_corner, VALUE y_corner,
   VALUE x_end, VALUE y_end, VALUE dx, VALUE dy);
extern VALUE FM_append_rect_to_path(VALUE fmkr, VALUE x, VALUE y, VALUE width, VALUE height);  // x y w h re
extern VALUE FM_append_rounded_rect_to_path(VALUE fmkr, VALUE x, VALUE y, VALUE width, VALUE height, VALUE dx, VALUE dy);
extern VALUE FM_append_oval_to_path(VALUE fmkr, VALUE x, VALUE y, VALUE dx, VALUE dy, VALUE angle);
extern VALUE FM_append_circle_to_path(VALUE fmkr, VALUE x, VALUE y, VALUE dx);
extern VALUE FM_append_points_to_path(VALUE fmkr, VALUE x_vec, VALUE y_vec);
extern VALUE FM_private_append_points_with_gaps_to_path(VALUE fmkr, VALUE x_vec, VALUE y_vec, VALUE gaps, VALUE close_gaps);
extern VALUE FM_stroke(VALUE fmkr);  // S
extern VALUE FM_close_and_stroke(VALUE fmkr);  // s
extern VALUE FM_fill(VALUE fmkr); // f
extern VALUE FM_discard_path(VALUE fmkr); // n
extern VALUE FM_eofill(VALUE fmkr); // f*
extern VALUE FM_fill_and_stroke(VALUE fmkr); // B
extern VALUE FM_eofill_and_stroke(VALUE fmkr); // B*
extern VALUE FM_close_fill_and_stroke(VALUE fmkr); // b
extern VALUE FM_close_eofill_and_stroke(VALUE fmkr); // b*
extern VALUE FM_clip(VALUE fmkr);  // W n
extern VALUE FM_eoclip(VALUE fmkr);  // W* n
extern VALUE FM_stroke_line(VALUE fmkr, VALUE x1, VALUE y1, VALUE x2, VALUE y2);
extern VALUE FM_fill_rect(VALUE fmkr, VALUE x, VALUE y, VALUE width, VALUE height);
extern VALUE FM_stroke_rect(VALUE fmkr, VALUE x, VALUE y, VALUE width, VALUE height);
extern VALUE FM_fill_and_stroke_rect(VALUE fmkr, VALUE x, VALUE y, VALUE width, VALUE height);
extern VALUE FM_clip_rect(VALUE fmkr, VALUE x, VALUE y, VALUE width, VALUE height);
extern VALUE FM_fill_oval(VALUE fmkr, VALUE x, VALUE y, VALUE dx, VALUE dy, VALUE angle);
extern VALUE FM_stroke_oval(VALUE fmkr, VALUE x, VALUE y, VALUE dx, VALUE dy, VALUE angle);
extern VALUE FM_fill_and_stroke_oval(VALUE fmkr, VALUE x, VALUE y, VALUE dx, VALUE dy, VALUE angle);
extern VALUE FM_clip_oval(VALUE fmkr, VALUE x, VALUE y, VALUE dx, VALUE dy, VALUE angle);
extern VALUE FM_fill_rounded_rect(VALUE fmkr, VALUE x, VALUE y, VALUE width, VALUE height, VALUE dx, VALUE dy);
extern VALUE FM_stroke_rounded_rect(VALUE fmkr, VALUE x, VALUE y, VALUE width, VALUE height, VALUE dx, VALUE dy);
extern VALUE FM_fill_and_stroke_rounded_rect(VALUE fmkr, VALUE x, VALUE y, VALUE width, VALUE height, VALUE dx, VALUE dy);
extern VALUE FM_clip_rounded_rect(VALUE fmkr, VALUE x, VALUE y, VALUE width, VALUE height, VALUE dx, VALUE dy);
extern VALUE FM_fill_circle(VALUE fmkr, VALUE x, VALUE y, VALUE dx);
extern VALUE FM_stroke_circle(VALUE fmkr, VALUE x, VALUE y, VALUE dx);
extern VALUE FM_fill_and_stroke_circle(VALUE fmkr, VALUE x, VALUE y, VALUE dx);
extern VALUE FM_clip_circle(VALUE fmkr, VALUE x, VALUE y, VALUE dx);
extern VALUE FM_append_frame_to_path(VALUE fmkr);
extern VALUE FM_fill_frame(VALUE fmkr);
extern VALUE FM_stroke_frame(VALUE fmkr);
extern VALUE FM_fill_and_stroke_frame(VALUE fmkr);
extern VALUE FM_clip_to_frame(VALUE fmkr);

/*======================================================================*/
// pdftext.c
void Init_Font_Dictionary(void);
extern VALUE FM_register_font(VALUE fmkr, VALUE font_name);  // returns font number.
extern bool Used_Any_Fonts(void);
extern void Clear_Fonts_In_Use_Flags(void);
extern void Write_Font_Dictionaries(void);
extern void Write_Font_Descriptors(void);
extern void Write_Font_Widths(void);
extern VALUE FM_marker_string_info(VALUE fmkr, VALUE font_number, VALUE string, VALUE scale);
extern VALUE FM_private_show_marker(VALUE fmkr, VALUE integer_args, VALUE stroke_width, VALUE string,
    VALUE x, VALUE y, VALUE x_vec, VALUE y_vec,
    VALUE h_scale, VALUE v_scale, VALUE scale, VALUE it_angle, VALUE ascent_angle, VALUE angle,
    VALUE fill_color, VALUE stroke_color);

/*======================================================================*/
// texout.c
extern VALUE FM_rescale_text(VALUE fmkr, VALUE scaling);
extern void c_show_rotated_text(FM *p, char *text, int frame_side, double shift, double fraction,
   double scale, double angle, int justification, int alignment);
extern VALUE FM_show_rotated_text(VALUE fmkr, VALUE text, VALUE frame_side, VALUE shift,
    VALUE fraction, VALUE scale, VALUE angle, VALUE justification, VALUE alignment);
extern void c_show_rotated_label(FM *p, char *text, 
   double xloc, double yloc, double scale, double angle, int justification, int alignment);
extern VALUE FM_show_rotated_label(VALUE fmkr, VALUE text,
    VALUE xloc, VALUE yloc, VALUE scale, VALUE angle, VALUE justification, VALUE alignment);
extern VALUE FM_check_label_clip(VALUE fmkr, VALUE xloc, VALUE yloc);
extern void Open_tex(VALUE fmkr, char *filename, bool quiet_mode);
extern void Close_tex(VALUE fmkr, bool quiet_mode);
extern void Create_wrapper(VALUE fmkr, char *filename, bool quiet_mode);
extern void Init_tex(void);
extern void Rename_tex(char *oldname, char *newname);
extern void private_make_portfolio(char *filename, VALUE fignums, VALUE fignames);

/*======================================================================*/

// ENLARGE = the conversion factor from "big points" to output units
#define ENLARGE 10.0
#define BIG_POINTS_PER_INCH 72.0
#define INCHES_PER_MM 0.0393700787

// 10 miles should be enough!
#define iMAX_DEV_COORD_ALLOWED (BIG_POINTS_PER_INCH*12*5280*10)
#define MAX_DEV_COORD_ALLOWED ((double)iMAX_DEV_COORD_ALLOWED)

#define convert_inches_to_output(inches) ((ENLARGE*BIG_POINTS_PER_INCH)*(inches))
#define convert_output_to_inches(output) ((output)/(ENLARGE*BIG_POINTS_PER_INCH))

#define convert_mm_to_output(mm) ((ENLARGE*BIG_POINTS_PER_INCH*INCHES_PER_MM)*(mm))
#define convert_output_to_mm(output) ((output)/(ENLARGE*BIG_POINTS_PER_INCH*INCHES_PER_MM))

#define convert_page_to_output_x(p,x) ((p)->page_left + (x)*(p)->page_width)
#define convert_page_to_output_y(p,y) ((p)->page_bottom + (y)*(p)->page_height)
#define convert_page_to_output_dx(p,dx) ((dx)*(p)->page_width)
#define convert_page_to_output_dy(p,dy) ((dy)*(p)->page_height)

#define convert_output_to_page_x(p,x) (((x) - (p)->page_left)/(p)->page_width)
#define convert_output_to_page_y(p,y) (((y) - (p)->page_bottom)/(p)->page_height)
#define convert_output_to_page_dx(p,dx) ((dx)/(p)->page_width)
#define convert_output_to_page_dy(p,dy) ((dy)/(p)->page_height)

#define convert_frame_to_page_x(p,x) ((p)->frame_left + (x)*(p)->frame_width)
#define convert_frame_to_page_y(p,y) ((p)->frame_bottom + (y)*(p)->frame_height)
#define convert_frame_to_page_dx(p,dx) ((dx)*(p)->frame_width)
#define convert_frame_to_page_dy(p,dy) ((dy)*(p)->frame_height)

#define convert_page_to_frame_x(p,x) (((x) - (p)->frame_left)/(p)->frame_width)
#define convert_page_to_frame_y(p,y) (((y) - (p)->frame_bottom)/(p)->frame_height)
#define convert_page_to_frame_dx(p,dx) ((dx)/(p)->frame_width)
#define convert_page_to_frame_dy(p,dy) ((dy)/(p)->frame_height)

#define convert_figure_to_frame_x(p,x) (\
   ((p)->bounds_left > (p)->bounds_right)? (1.0 - ((x)-(p)->bounds_xmin)/(p)->bounds_width) : ((x)-(p)->bounds_xmin)/(p)->bounds_width)
#define convert_figure_to_frame_y(p,y) (\
   ((p)->bounds_bottom > (p)->bounds_top)? (1.0 - ((y)-(p)->bounds_ymin)/(p)->bounds_height) : ((y)-(p)->bounds_ymin)/(p)->bounds_height)
#define convert_figure_to_frame_dx(p,dx) (\
   ((p)->bounds_left > (p)->bounds_right)? -(dx)/(p)->bounds_width : (dx)/(p)->bounds_width)
#define convert_figure_to_frame_dy(p,dy) (\
   ((p)->bounds_bottom > (p)->bounds_top)? -(dy)/(p)->bounds_height : (dy)/(p)->bounds_height)

#define convert_frame_to_figure_x(p,x) (\
   ((p)->bounds_left > (p)->bounds_right)? ((p)->bounds_left - (x)*(p)->bounds_width): ((p)->bounds_left + (x)*(p)->bounds_width))
#define convert_frame_to_figure_y(p,y) (\
   ((p)->bounds_bottom > (p)->bounds_top)? ((p)->bounds_bottom - (y)*(p)->bounds_height): ((p)->bounds_bottom + (y)*(p)->bounds_height))
#define convert_frame_to_figure_dx(p,dx) (\
   ((p)->bounds_left > (p)->bounds_right)? (-(dx)*(p)->bounds_width): ((dx)*(p)->bounds_width))
#define convert_frame_to_figure_dy(p,dy) (\
   ((p)->bounds_bottom > (p)->bounds_top)? (-(dy)*(p)->bounds_height): ((dy)*(p)->bounds_height))

#ifndef USE_P
#define USE_P p = NULL;
#endif
#ifndef STRLEN
#define STRLEN 256
#endif
#ifndef PI
#define PI 3.1415926535897932384
#endif
#ifndef MAX
#define MAX(a,b)    (((a) > (b)) ? (a) : (b))
#endif
#ifndef MIN
#define MIN(a,b)    (((a) < (b)) ? (a) : (b))
#endif
#ifndef ABS
#define ABS(a)      ((a)<0 ? -(a) : (a))
#endif
#ifndef ROUND
#define ROUND(a)    (int)((a)<0.0 ? ((a)-0.5) : ((a)+0.5))
#endif
#ifndef SIGN
#define SIGN(a)         ((a)<0 ? -1 : 1)
#endif

#define DBL_ATTR(attr) \
static VALUE FM_##attr##_get(VALUE fmkr) { FM *p = Get_FM(fmkr); return rb_float_new(p->attr); } \
static VALUE FM_##attr##_set(VALUE fmkr, VALUE val) { \
   FM *p = Get_FM(fmkr); VALUE v = rb_Float(val); p->attr = NUM2DBL(v); return val; }

#define INT_ATTR(attr) \
static VALUE FM_##attr##_get(VALUE fmkr) { FM *p = Get_FM(fmkr); return INT2FIX(p->attr); } \
static VALUE FM_##attr##_set(VALUE fmkr, VALUE val) { \
   FM *p = Get_FM(fmkr); VALUE v = rb_Integer(val); p->attr = NUM2INT(v); return val; }

#define VAL_ATTR(attr) \
static VALUE FM_##attr##_get(VALUE fmkr) { FM *p = Get_FM(fmkr); return p->attr; } \
static VALUE FM_##attr##_set(VALUE fmkr, VALUE val) { \
   FM *p = Get_FM(fmkr); p->attr = val; return val; }

#define BOOL_ATTR(attr) \
static VALUE FM_##attr##_get(VALUE fmkr) { FM *p = Get_FM(fmkr); return (p->attr)? Qtrue : Qfalse; } \
static VALUE FM_##attr##_set(VALUE fmkr, VALUE val) { \
   FM *p = Get_FM(fmkr); p->attr = (val != Qfalse); return val; }

#define RO_DBL_ATTR(attr) \
static VALUE FM_##attr##_get(VALUE fmkr) { FM *p = Get_FM(fmkr); return rb_float_new(p->attr); }

#define RO_INT_ATTR(attr) \
static VALUE FM_##attr##_get(VALUE fmkr) { FM *p = Get_FM(fmkr); return INT2FIX(p->attr); }

#define RO_VAL_ATTR(attr) \
static VALUE FM_##attr##_get(VALUE fmkr) { FM *p = Get_FM(fmkr); return p->attr; }

#define RO_BOOL_ATTR(attr) \
static VALUE FM_##attr##_get(VALUE fmkr) { FM *p = Get_FM(fmkr); return (p->attr)? Qtrue : Qfalse; }

#endif   /* __figures_H__ */

