#ifndef HELP_H
#define HELP_H
#include <ncurses.h>

typedef const char *help_text_type[][2];
typedef const char *const (*help_text_storage_type)[2];

typedef struct
{
  unsigned char top;
  unsigned char bottom;
  unsigned char left;
  unsigned char right;
} help_padding;

#define HELP_NO_PADDING ((help_padding) {0, 0, 0, 0})
#define HELP_BORDER(n) ((help_padding) {(n), (n), (n), (n)})

typedef struct
{
  unsigned height;
  unsigned key_width;
  unsigned desc_width;
  help_text_storage_type text;
  WINDOW *window;
  help_padding padding;
  char **render_data;
  unsigned cursor;
  unsigned max_cursor;
} help_type;

void
help_init_impl (
  help_type *help,
  help_text_type text,
  unsigned text_size
);

/* Initialize HELP with TEXT.
   Note: does not require ncurses to be initialized. */
#define help_init(help, text) \
  help_init_impl ((help), (text), sizeof (text) / sizeof ((text)[0]))

/* Release any memory held by the HELP object. */
void
help_free (
  help_type *help
);

/* W and H should hold the desired width and height values and will recieve
   the actual values. W and H include the padding specified in HELP. */
void
help_resize (
  help_type *help,
  unsigned *w,
  unsigned *h
);

/* Center the HELP window inside OUTER. */
void
help_center (
  help_type *help,
  WINDOW *outer
);

/* Resize and center the HELP window inside OUTER,
   using PERCENT % of OUTER's size. */
static inline void
help_resize_relative (
  help_type *help,
  WINDOW *outer,
  float percent
);

/* Resize and center the HELP window inside OUTER with HORIZONTAL character
   padding at the top/bottom and VERTICAL characters padding at the
   left/right. */
static inline void
help_resize_offset (
  help_type *help,
  WINDOW *outer,
  unsigned vertical,
  unsigned horizontal
);

/* Draw the help text into its window. */
void
help_draw (
  help_type *help
);

/* Set the cursor. If POS is out of bounds it gets clamped. */
void
help_set_cursor (
  help_type *help,
  unsigned pos
);

/* Move the cursor by BY (negative value moves up, positive down). If the
   resulting position would be out of bounds it gets clamped. */
void
help_move_cursor (
  help_type *help,
  int by
);

/* Print the HELP text into STREAM. No previous call to `help_resize` required.
 */
void
help_print (
  help_type *help,
  FILE *stream
);



/**** Inline function definitions ****/

static inline void
help_resize_relative (help_type *help, WINDOW *outer, float percent)
{
  unsigned w = getmaxx (outer) * percent;
  unsigned h = getmaxy (outer) * percent;
  help_resize (help, &w, &h);
  help_center (help, outer);
}

static inline void
help_resize_offset (help_type *help, WINDOW *outer, unsigned vertical,
                    unsigned horizontal)
{
  unsigned w = getmaxx (outer) - (vertical << 1);
  unsigned h = getmaxy (outer) - (horizontal << 1);
  help_resize (help, &w, &h);
  help_center (help, outer);
}

#endif /* HELP_H */
