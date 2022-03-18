#include <ncurses.h>
#include "help.h"
#include <locale>

#define PP_STR_IMPL(x) #x
#define PP_STR(x) PP_STR_IMPL(x)

static help_text_type help_text = {
  {"k/↑", "Move cursor up"},
  {"j/↓", "Move cursor down"},
  {"li", "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do "
         "eiusmod tempor incididunt ut labore et dolore magna aliqua."},
  { "key", "" },
  { "", "description"},
  { "업", "Up" },
  { ">", "빠른 갈색 여우는 게으른 개를 뛰어넘는다" },
  { "🐞", "Single emoji" },
  { ">", "😀 😁😂🤣😃 😄😅😆😉 😊😋😎😍😘🥰😗😙 😚🙂 🤗🤩🤔🤨😮 😥 😣😏🙄 😶😑😐" },
  { ">", "abcdefghijklmnopqrstuvwxyz" },
  { ">", "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ" },
  { ">", "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ "
         "and some more text"},
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
};

int
main ()
{
  std::setlocale (LC_CTYPE, "");
  nc_help::Help help (help_text);

  initscr ();
  curs_set (0);
  noecho ();
  cbreak ();
  nodelay (stdscr, FALSE);

  help.resize_relative (0.9f);

  help.draw ();
  ::box (help.window (), 0, 0);
  ::refresh ();
  help.refresh ();

  int ch;
  bool running = true;
  while (running)
    {
      ch = getch ();
      switch (ch)
        {
          case 'k':
            help.move_cursor (-1);
            break;
          case 'K':
            help.move_cursor (-5);
            break;
          case 'j':
            help.move_cursor (1);
            break;
          case 'J':
            help.move_cursor (5);
            break;
          case 'g':
            help.set_cursor (0);
            break;
          case 'G':
            help.set_cursor (static_cast<unsigned> (-1));
            break;
          case 'q':
          case '\n':
            running = false;
            break;
          case KEY_RESIZE:
            ::clear ();
            help.resize_relative (0.9f);
            break;
        }
      help.draw ();
      ::box (help.window (), 0, 0);
      ::refresh ();
      help.refresh ();
    }

  endwin ();
}

