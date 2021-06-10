#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>

typedef struct termios termios;

termios orig_termios;

void die(const char *s) {
  perror(s);
  exit(1);
}

void disableRawMode()
{
  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1)
    die("tcsetattr");
}

void enableRawMode()
{
  if (tcgetattr(STDIN_FILENO, &orig_termios) == -1)
    die("tcgetattr");
  atexit(disableRawMode);

  termios raw = orig_termios;
  raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
  raw.c_oflag &= ~(OPOST);
  raw.c_cflag |= (CS8);
  raw.c_lflag &= ~(ECHO | ICANON| IEXTEN| ISIG);
  raw.c_cc[VMIN] = 0;
  raw.c_cc[VTIME] = 1;
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);

  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1)
    die("tcsetattr");
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
      fprintf(stderr, "Running in debug mode\r\n");
    }
  }
    while (1) {
      c = '\0';
      if (read(STDIN_FILENO, &c, 1) == -1 && errno != EAGAIN)
        die("read");
      read(STDIN_FILENO, &c, 1);
      if (iscntrl(c))
      {
        if(debug)
          printf("%d\r\n", c);
      }
      else
      {
        if(debug)     
          printf("%d ('%c')\r\n", c, c);
        else {
           printf("%c", c);
          fflush(stdout);
        }
         
      }
      if (c == 'q') 
        break;
    }
    return 0;
  }