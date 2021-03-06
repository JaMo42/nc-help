/* This is free and unencumbered software released into the public domain.

   Anyone is free to copy, modify, publish, use, compile, sell, or
   distribute this software, either in source code form or as a compiled
   binary, for any purpose, commercial or non-commercial, and by any
   means.

   In jurisdictions that recognize copyright laws, the author or authors
   of this software dedicate any and all copyright interest in the
   software to the public domain. We make this dedication for the benefit
   of the public at large and to the detriment of our heirs and
   successors. We intend this dedication to be an overt act of
   relinquishment in perpetuity of all present and future rights to this
   software under copyright law.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
   IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
   OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
   OTHER DEALINGS IN THE SOFTWARE.

   For more information, please refer to <http://unlicense.org/> */
#ifndef HELP_H
#define HELP_H
#include <ncurses.h>

#ifdef __cplusplus
extern "C" {
#endif

struct help_key_desc
{
  const char *key;
  const char *desc;
};

typedef struct help_key_desc help_text_type[];
typedef struct help_key_desc *help_text_storage_type;

typedef struct
{
  unsigned char top;
  unsigned char bottom;
  unsigned char left;
  unsigned char right;
} help_padding_t;

/* creates a padding object with all values set to 0. */
#define HELP_NO_PADDING ((help_padding_t) {0, 0, 0, 0})

/* creates a padding object with all values set to `n`. */
#define HELP_BORDER(n) ((help_padding_t) {(n), (n), (n), (n)})

#define HELP_LABEL(s) { NULL, (s) }

typedef struct
{
  unsigned height;
  unsigned key_width;
  unsigned desc_width;
  unsigned desc_min_width;
  help_text_storage_type text;
  WINDOW *window;
  help_padding_t padding;
  char **render_data;
  unsigned cursor;
  unsigned max_cursor;
  bool use_render_data;
} help_type;

void
help_init_impl (
  help_type *help,
  help_text_type text,
  unsigned text_size
);

/* Initializes the help object.  Can be called before `initscr`. */
#define help_init(help, text) \
  help_init_impl ((help), (text), sizeof (text) / sizeof ((text)[0]))

/* Releases any memory held by the help object. */
void
help_free (
  help_type *help
);

/* Resizes the ncurses window and wraps text accordingly.  `w` and `h` hold the
   desired width and height.  If the text gets wrapped the width may be reduced
   to fit the new text width.  If the width is too narrow for the wrapped
   content it gets increased.  If the height is greater than the number of lines
   (after wrapping) it gets decreased.  The actual width and height values get
   written back to the given pointers. */
void
help_resize (
  help_type *help,
  unsigned *w,
  unsigned *h
);

/* Centers the ncurses window inside the given outer window. */
void
help_center (
  help_type *help,
  WINDOW *outer
);

/* Resizes the ncurses window to a percentage of the outer windows size and
   centers it inside it.  (see `help_resize` regarding actual size) */
static inline void
help_resize_relative (
  help_type *help,
  WINDOW *outer,
  float percent
);

/* Resizes the ncurses window to have the given offsets from the outer windows
   borders and centers it inside it.  (see `help_resize` regarding actual size)
 */
static inline void
help_resize_offset (
  help_type *help,
  WINDOW *outer,
  unsigned vertical,
  unsigned horizontal
);

/* Draws the contents to the ncurses window.  `help_resize` should have been
   called once before this. */
void
help_draw (
  help_type *help
);

/* Refreshes the ncurses window. */
static inline void
help_refresh (
  help_type *help
);

/* Checks whether help text can be scrolled. */
static inline bool
help_can_scroll (
  const help_type *help
);

/* Sets the cursor position.  If it is out of bounds it gets clamped. */
void
help_set_cursor (
  help_type *help,
  unsigned pos
);

/* Moves the cursor by the given amount.  If resulting position would be out of
   bounds it gets clamped. */
void
help_move_cursor (
  help_type *help,
  int by
);

/* Prints the help text into the given stream.  Does not require a previous
   call to `help_resize`, but if present the wrapped text gets printed. */
void
help_print (
  const help_type *help,
  FILE *stream
);



/**** Inline function definitions ****/

static inline void __attribute__((unused))
help_resize_relative (help_type *help, WINDOW *outer, float percent)
{
  unsigned w = getmaxx (outer) * percent;
  unsigned h = getmaxy (outer) * percent;
  help_resize (help, &w, &h);
  help_center (help, outer);
}

static inline void __attribute__((unused))
help_resize_offset (help_type *help, WINDOW *outer, unsigned vertical,
                    unsigned horizontal)
{
  unsigned w = getmaxx (outer) - (vertical << 1);
  unsigned h = getmaxy (outer) - (horizontal << 1);
  help_resize (help, &w, &h);
  help_center (help, outer);
}

static inline void __attribute__((unused))
help_refresh (help_type *help)
{
  wrefresh (help->window);
}

static inline bool __attribute__((unused))
help_can_scroll (const help_type *help)
{
  return help->max_cursor > 0;
}

#ifdef __cplusplus
}
#endif



/**** C++ wrapper ****/

#ifdef __cplusplus
#include <cstddef>
namespace nc_help
{
class Help
{
public:
  template <std::size_t N>
    Help (struct help_key_desc (&text)[N])
    { help_init_impl (&self_, text, N); }

  ~Help ()
  { help_free (&self_); }

  inline WINDOW *
  window ()
  { return self_.window; }

  inline help_padding_t &
  padding ()
  { return self_.padding; }

  inline void
  resize (unsigned &w, unsigned &h)
  { help_resize (&self_, &w, &h); }

  inline void
  resize (unsigned &&w, unsigned &&h)
  {
    unsigned my_w = w, my_h = h;
    this->resize (my_w, my_h);
  }

  inline void
  center (WINDOW *outer = stdscr)
  { help_center (&self_, outer); }

  inline void
  resize_relative (float percent, WINDOW *outer = stdscr)
  { help_resize_relative (&self_, outer, percent); }

  inline void
  resize_offset (unsigned v, unsigned h, WINDOW *outer = stdscr)
  { help_resize_offset (&self_, outer, v, h); }

  inline void
  draw ()
  { help_draw (&self_); }

  inline void
  refresh ()
  { help_refresh (&self_); }

  inline bool
  can_scroll () const
  { return help_can_scroll (&self_); }

  inline void
  set_cursor (unsigned pos)
  { help_set_cursor (&self_, pos); }

  inline void
  move_cursor (int by)
  { help_move_cursor (&self_, by); }

  inline void
  print (FILE *stream = stdout) const
  { help_print (&self_, stream); }

private:
  help_type self_ = {};
};
}
#endif

#endif /* HELP_H */
