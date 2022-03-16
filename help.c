#define _GNU_SOURCE
#include "help.h"
#include "vector.h"
#include <wchar.h>

static long
utf8_sum (const char *in_str, int (*f) (wchar_t))
{
  const unsigned char *str = (const unsigned char *)in_str;
  long ret = 0;
  uint32_t ch;
  while (*str)
    {
      if (*str < 0x80)
        ch = *str++;
      else if (*str < 0xe0)
        {
          ch = ((str[0] & 0x1f) << 6) | (str[1] & 0x3f);
          str += 2;
        }
      else if (*str < 0xf0)
        {
          ch = (((str[0] & 0xf) << 12)
                | ((str[1] & 0x3f) << 6)
                | (str[2] & 0x3f));
          str += 3;
        }
      else
        {
          ch = (((str[0] & 0x7) << 18)
                | ((str[1] & 0x3f) << 12)
                | ((str[2] & 0x3f) << 6)
                | (str[3] & 0x3f));
          str += 4;
        }
      ret += f (ch);
    }
  return ret;
}

static int
padding_offset (wchar_t ch)
{
  int w;
  if (ch < (1 << 7))
    return 0;
  w = wcwidth (ch);
  if (ch < (1 << 11))
    return 2 - w;
  if (ch < (1 << 16))
    return 3 - w;
  else
    return 4 - w;
}

#define utf8_width(s) utf8_sum ((s), wcwidth)
#define utf8_padding_offset(s) utf8_sum ((s), padding_offset)

void
help_init_impl (help_type *help, help_text_type text, unsigned text_size)
{
  const char *key, *desc;
  unsigned i, w;
  help->text = text;
  help->height = text_size;
  help->key_width = 0;
  help->desc_width = 0;
  for (i = 0; i < text_size; ++i)
    {
      key = text[i][0];
      desc = text[i][1];
      if (*key)
        {
          w = utf8_width (key);
          if (w > help->key_width)
            help->key_width = w;
        }
      if (*desc)
        {
          w = utf8_width (desc);
          if (w > help->desc_width)
            help->desc_width = w;
        }
    }
  help->window = NULL;
  help->padding = HELP_BORDER (1);
  help->render_data = NULL;
  help->cursor = 0;
  help->max_cursor = 0;
}

static inline void
help_free_render_data (help_type *help)
{
  vector_for_each (help->render_data, line)
    vector_free (*line);
  vector_free (help->render_data);
  help->render_data = NULL;
}

void
help_free (help_type *help)
{
  if (help->render_data)
    help_free_render_data (help);
  if (help->window)
    delwin (help->window);
}

static void
help_render (help_type *help, unsigned window_width, unsigned *rendered_width)
{
  const unsigned content_width = ((window_width
                                   ? window_width
                                   : getmaxx (help->window))
                                  - help->padding.left - help->padding.right);
  const unsigned width = content_width - help->key_width - 2;
  unsigned i, current_width, token_width, j;
  char *line, *token, *out_line;
  if (help->desc_width <= width)
    {
      /* No formatting needed, draw directly from text */
      if (help->render_data)
        help_free_render_data (help);
      return;
    }
  line = malloc (help->desc_width * 4);
  if (help->render_data)
    {
      vector_for_each (help->render_data, line)
        vector_free (*line);
      vector_clear (help->render_data);
    }
  if (rendered_width)
    *rendered_width = 0;
  for (i = 0; i < help->height; ++i)
    {
      memcpy (line, help->text[i][1], strlen (help->text[i][1]) + 1);
      token = strtok (line, " ");
      out_line = vector_create (char, content_width);
      /* Key and padding */
      j = help->key_width + 2 + utf8_padding_offset (help->text[i][0]);
      vector__size (out_line) = j;
      sprintf (out_line, "%-*s", j, help->text[i][0]);
      /* Description */
      current_width = 0;
      while (token)
        {
          token_width = utf8_width (token);
          if (current_width + token_width > width)
            {
              if (current_width == 0)
                {
                  /* Single word was wider than width, adjust window size and
                     restart. */
                  free (line);
                  vector_free (out_line);
                  return help_render (help,
                                      (help->padding.left + help->padding.right
                                       + help->key_width + 2 + token_width),
                                      rendered_width);
                }
              /* Line limit reached */
              vector_back (out_line) = '\0';
              vector_push (help->render_data, out_line);
              if (rendered_width)
              {
                --current_width ;
                if (current_width > *rendered_width)
                  *rendered_width = current_width;
              }
              out_line = vector_create (char, content_width);
              /* Padding */
              j = help->key_width + 2;
              vector__size (out_line) = j;
              memset (out_line, ' ', j);
              current_width = 0;
            }
          while (*token)
            vector_push (out_line, *token++);
          vector_push (out_line, ' ');
          current_width += token_width + 1;
          token = strtok (NULL, " ");
        }
      /* Add line */
      vector_back (out_line) = '\0';
      vector_push (help->render_data, out_line);
      if (rendered_width)
        {
          current_width -= !!current_width;
          if (current_width > *rendered_width)
            *rendered_width = current_width;
        }
    }
  /* loop only keeps track of description width */
  if (rendered_width)
    *rendered_width += help->key_width + 2;
  free (line);
}

void
help_draw (help_type *help)
{
  const unsigned content_height = (getmaxy (help->window) - help->padding.top
                                   - help->padding.bottom);
  const unsigned end = help->cursor + content_height;
  unsigned i, line;
  wclear (help->window);
  if (help->render_data)
    {
      for (line = 1, i = help->cursor; i != end; ++i, ++line)
        mvwaddstr (help->window, line, 1, help->render_data[i]);
    }
  else
    {
      for (line = 1, i = help->cursor; i != end; ++i, ++line)
        {
          mvwaddstr (help->window, line, help->padding.left, help->text[i][0]);
          mvwaddstr (help->window,
                     line, help->padding.left + help->key_width + 2,
                     help->text[i][1]);
        }
    }
}

void
help_resize (help_type *help, unsigned *w, unsigned *h)
{
  const unsigned content_width = (help->key_width + 2 + help->desc_width
                                  + help->padding.left + help->padding.right);
  unsigned rendered_width, content_height;
  bool did_render = FALSE;
  /* Width */
  if (*w > content_width)
    {
      *w = content_width;
      if (help->render_data)
        help_free_render_data (help);
    }
  else if (content_width > *w)
    {
      if (help->window)
        wresize (help->window, getmaxy (help->window), *w);
      else
        help->window = newwin (1, *w, 0, 0);
      help_render (help, 0, &rendered_width);
      *w = rendered_width + help->padding.left + help->padding.right;
      did_render = TRUE;
    }
  /* Height */
  content_height = ((
    help->render_data ? vector_size (help->render_data) : help->height
  ) + help->padding.top + help->padding.bottom);
  if (*h > content_height)
    *h = content_height;
  /* Resize */
  if (help->window)
    wresize (help->window, *h, *w);
  else
    help->window = newwin (*h, *w, 0, 0);
  if (!did_render)
    help_render (help, 0, NULL);
  help->max_cursor = (content_height
                      - (*h
                         - ((help->padding.top - help->padding.bottom) << 1)));
  help->cursor = 0;
}

void
help_center (help_type *help, WINDOW *outer)
{
  mvwin (help->window,
         (getmaxy (outer) - getmaxy (help->window)) / 2,
         (getmaxx (outer) - getmaxx (help->window)) / 2);
}

void
help_set_cursor (help_type *help, unsigned pos)
{
  if (pos > help->max_cursor)
    help->cursor = help->max_cursor;
  else
    help->cursor = pos;
}

void
help_move_cursor (help_type *help, int by)
{
  if (by < 0 && (unsigned)-by > help->cursor)
    help->cursor = 0;
  else if (help->cursor + by > help->max_cursor)
    help->cursor = help->max_cursor;
  else
    help->cursor += by;
}

void
help_print (help_type *help, FILE *stream)
{
  unsigned i;
  for (i = 0; i < help->height; ++i)
    {
      fprintf (stream, "%-*s  %s\n",
               help->key_width + (int)utf8_padding_offset (help->text[i][0]),
               help->text[i][0], help->text[i][1]);
    }
}

