/* winsize.c - handle window size changes and information. */

/* Copyright (C) 2005 Free Software Foundation, Inc.

   This file is part of GNU Bash, the Bourne Again SHell.

   Bash is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   Bash is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with Bash.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "config.h"

#include <stdc.h>

#include "bashtypes.h"

#if defined (HAVE_UNISTD_H)
#  include <unistd.h>
#endif

#include <sys/ioctl.h>

/* Try to find the definitions of `struct winsize' and TIOGCWINSZ */

#if defined (GWINSZ_IN_SYS_IOCTL) && !defined (TIOCGWINSZ)
#  include <sys/ioctl.h>
#endif /* GWINSZ_IN_SYS_IOCTL && !TIOCGWINSZ */

#if defined (STRUCT_WINSIZE_IN_TERMIOS) && !defined (STRUCT_WINSIZE_IN_SYS_IOCTL)
#  include <termios.h>
#endif /* STRUCT_WINSIZE_IN_TERMIOS && !STRUCT_WINSIZE_IN_SYS_IOCTL */

/* Not in either of the standard places, look around. */
#if !defined (STRUCT_WINSIZE_IN_TERMIOS) && !defined (STRUCT_WINSIZE_IN_SYS_IOCTL)
#  if defined (HAVE_SYS_STREAM_H)
#    include <sys/stream.h>
#  endif /* HAVE_SYS_STREAM_H */
#  if defined (HAVE_SYS_PTEM_H) /* SVR4.2, at least, has it here */
#    include <sys/ptem.h>
#    define _IO_PTEM_H          /* work around SVR4.2 1.1.4 bug */
#  endif /* HAVE_SYS_PTEM_H */
#  if defined (HAVE_SYS_PTE_H)  /* ??? */
#    include <sys/pte.h>
#  endif /* HAVE_SYS_PTE_H */
#endif /* !STRUCT_WINSIZE_IN_TERMIOS && !STRUCT_WINSIZE_IN_SYS_IOCTL */

#include <stdio.h>

/* Return the fd from which we are actually getting input. */
#define input_tty() (shell_tty != -1) ? shell_tty : fileno (stderr)

#if !defined (errno)
extern int errno;
#endif /* !errno */

extern int shell_tty;

#if defined (READLINE)
extern void rl_set_screen_size __P((int, int));
#endif
extern void sh_set_lines_and_columns __P((int, int));

#include <pwd.h>
#define ESC "\033"
char *myname;
char getsize[] = ESC "7"  ESC "[r" ESC "[999;999H" ESC "[6n";
char restore[] = ESC "8";
struct termios tioorig;
char size[] = ESC "[%d;%dR";
int tty;
static void onintr (int sig);

static void
onintr(int sig)
{
       tcsetattr (STDERR_FILENO, TCSANOW, &tioorig);
}

int
set_new_window_size ()
{

       struct termios tio;
       struct winsize ws = { 0, 0, 0, 0};

       tty = STDERR_FILENO;
       tcgetattr(tty, &tioorig);
       tcgetattr(tty, &tio);
       tio.c_cflag |= (CLOCAL | CREAD);
       tio.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);

       signal(SIGINT, onintr);
       signal(SIGQUIT, onintr);
       signal(SIGTERM, onintr);
       tcsetattr(tty, TCSANOW, &tio);

       write(tty, getsize, strlen(getsize));
       scanf(ESC"[%hu;%huR", &ws.ws_row, &ws.ws_col);
       write(tty, restore, strlen(restore));
       ioctl (tty, TIOCSWINSZ, &ws);
       tcsetattr(tty, TCSANOW, &tioorig);
       signal(SIGINT, SIG_DFL);
       signal(SIGQUIT, SIG_DFL);
       signal(SIGTERM, SIG_DFL);
       return 0;
}

void
get_new_window_size (from_sig, rp, cp)
     int from_sig;
     int *rp, *cp;
{
#if defined (TIOCGWINSZ)
  struct winsize win;
  int tty;

  tty = input_tty ();
  if (tty >= 0 && (ioctl (tty, TIOCGWINSZ, &win) == 0) &&
      win.ws_row > 0 && win.ws_col > 0)
    {
      sh_set_lines_and_columns (win.ws_row, win.ws_col);
#if defined (READLINE)
      rl_set_screen_size (win.ws_row, win.ws_col);
      if (rp)
	*rp = win.ws_row;
      if (cp)
	*cp = win.ws_col;
#endif
    }
#endif
}
