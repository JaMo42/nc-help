#include "help.h"
#include "vector.h"
#include <locale.h>

#define PP_STR_IMPL(x) #x
#define PP_STR(x) PP_STR_IMPL(x)

static help_text_type help_text = {
#if 1
#if 1 /* unicode */
  {"k/↑", "Move cursor up"},
  {"j/↓", "Move cursor down"},
#else
  {"k/^", "Move cursor up"},
  {"j/v", "Move cursor down"},
#endif
#endif
#if 1 /* long text */
  {"li", "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do "
         "eiusmod tempor incididunt ut labore et dolore magna aliqua."},
#endif
#if 1 /* empty description/key */
  { "key", "" },
  { "", "description"},
#endif
#if 1 /* double-width characters */
  { "업", "Up" },
  { ">", "빠른 갈색 여우는 게으른 개를 뛰어넘는다" },
#endif
#if 1 /* emojis */
  { "🐞", "Single emoji" },
#if 0 /* composed (don't work properly) */
  { "🏴‍☠️", "Composed emoji (pirate)" },
  { "🏳️‍🌈", "Composed emoji (pride)" },
#endif
  { ">", "😀 😁😂🤣😃 😄😅😆😉 😊😋😎😍😘🥰😗😙 😚🙂 🤗🤩🤔🤨😮 😥 😣😏🙄 😶😑😐" },
#endif
#if 1 /* long word */
  { ">", "abcdefghijklmnopqrstuvwxyz" },
#endif
#if 1 /* very long word */
  { ">", "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ" },
  { ">", "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ "
         "and some more text"},
#endif
#if 1 /* a lot of text */
  { PP_STR (__COUNTER__), "a lot of text" },
  { PP_STR (__COUNTER__), "a lot of text" },
  { PP_STR (__COUNTER__), "a lot of text" },
  { PP_STR (__COUNTER__), "a lot of text" },
  { PP_STR (__COUNTER__), "a lot of text" },
  { PP_STR (__COUNTER__), "a lot of text" },
  { PP_STR (__COUNTER__), "a lot of text" },
  { PP_STR (__COUNTER__), "a lot of text" },
  { PP_STR (__COUNTER__), "a lot of text" },
  { PP_STR (__COUNTER__), "a lot of text" },
  { PP_STR (__COUNTER__), "a lot of text" },
  { PP_STR (__COUNTER__), "a lot of text" },
  { PP_STR (__COUNTER__), "a lot of text" },
  { PP_STR (__COUNTER__), "a lot of text" },
  { PP_STR (__COUNTER__), "a lot of text" },
  { PP_STR (__COUNTER__), "a lot of text" },
  { PP_STR (__COUNTER__), "a lot of text" },
  { PP_STR (__COUNTER__), "end of a lot of text" },
#endif
};

#define TEST_HELP_PRINT 0
#define TEST_SMALL_WINDOW 0

#if TEST_SMALL_WINDOW
#  define DO_RESIZE() \
     help_resize (&help, &w, &h); \
     help_center (&help, stdscr);
#else
#  define DO_RESIZE() help_resize_relative (&help, stdscr, 0.9f);
#endif

int
main ()
{
  help_type help;
  setlocale (LC_CTYPE, "");
  help_init (&help, help_text);

#if TEST_HELP_PRINT
  unsigned w_ = 30, h_ = 10;
  help_resize (&help, &w_, &h_);
#if 1
  w_ = 3;
  help_resize (&help, &w_, &h_);
  w_ = 10;
  help_resize (&help, &w_, &h_);
  h_ = 5;
  help_resize (&help, &w_, &h_);
  w_ = 40;
  h_ = 20;
  help_resize (&help, &w_, &h_);
  w_ = 70;
  help_resize (&help, &w_, &h_);
  w_ = 30;
  h_ = 10;
  help_resize (&help, &w_, &h_);
#endif
  help_print (&help, stdout);
  help_free (&help);
  return 0;
#else

  initscr ();
  curs_set (0);
  noecho ();
  cbreak ();
  nodelay (stdscr, FALSE);

  unsigned w = 30, h = 10;
  DO_RESIZE ();

  help_draw (&help);
  box (help.window, 0, 0);
  refresh ();
  wrefresh (help.window);

  int ch;
  bool running = TRUE;
  while (running)
    {
      ch = getch ();
      switch (ch)
        {
          case 'k':
            help_move_cursor (&help, -1);
            break;
          case 'K':
            help_move_cursor (&help, -5);
            break;
          case 'j':
            help_move_cursor (&help, 1);
            break;
          case 'J':
            help_move_cursor (&help, 5);
            break;
          case 'g':
            help_set_cursor (&help, 0);
            break;
          case 'G':
            help_set_cursor (&help, (unsigned)-1);
            break;
          case 'q':
          case '\n':
            running = FALSE;
            break;
          case KEY_RESIZE:
            clear ();
            DO_RESIZE ();
            break;
        }
      help_draw (&help);
      box (help.window, 0, 0);
      refresh ();
      wrefresh (help.window);
    }

  help_free (&help);
  endwin ();
  exit_curses (0);
  __builtin_unreachable ();
#endif
}

