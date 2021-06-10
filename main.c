#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>

typedef struct termios termios;

termios orig_termios;
void disableRawMode()
{
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

void enableRawMode()
{
  tcgetattr(STDIN_FILENO, &orig_termios);
  atexit(disableRawMode);
  termios raw = orig_termios;
  raw.c_lflag &= ~(ECHO | ICANON);
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

int main(int argc, char **argv)
{
  enableRawMode();
  char c;
  char debug = 0;

  for (int i = 1; i < argc; i++)
  {
    if (strncmp(argv[i], "-D", 2) == 0)
    {
      debug = 1;
      fprintf(stderr, "Running in debug mode\n");
    }
  }
    while (read(STDIN_FILENO, &c, 1) == 1 && c != 'q')
    {
      if (iscntrl(c))
      {
        if(debug)
          printf("%d\n", c);
      }
      else
      {
        if(debug)
          printf("%d ('%c')\n", c, c);
        else {
           printf("%c", c);
          fflush(stdout);
        }
         
      }
    }
    disableRawMode();
    return 0;
  }