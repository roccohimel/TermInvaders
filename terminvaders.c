// terminvaders.c

#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdlib.h>
#include <termios.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>


// check for keys pressed insted of input
struct termios original;

void enableRawMode() {
        struct termios raw;
        tcgetattr(STDIN_FILENO, &original);
        raw = original;
        raw.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

// disable the function
void disableRawMode() {
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &original);
}

// plot a string
void plot(int x, int y, char *z) {
        if (x < 1) x = 1;
        if (y < 1) y = 1;
        printf("\033[%d;%dH%s", y, x, z);
}

void ploti(int x, int y, int z) {
        if (x < 1) x = 1;
        if (y < 1) y = 1;
        printf("\033[%d;%dH%d", y, x, z);
}

// main
int main() {
        // clear buffer and enable alt term
        setvbuf(stdout, NULL, _IONBF, 0);
        system("tput smcup; clear");
        printf("\033[?25l");

        // enable raw keys
        enableRawMode();
        fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);

        // get max X and Y
        struct winsize w;
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
        int maxScreenX = w.ws_col;
        int maxScreenY = w.ws_row;

        // initialize tank positions
        int tankPositionX = (maxScreenX / 2) - 1;
        int tankPositionY = maxScreenY - 3;
        char *tankChar = "<|>";

        // initialize bullet positions
        int bulletPositionX = 0;
        int bulletPositionY = 0;
        char *bulletChar = "o";
        int bulletActive = 0;

	srand(time(NULL));
        int enemyPositionX = 2 + (rand() % maxScreenX - 2);
        int enemyPositionY = 10;
        char *enemyChar = "X";
        
      	int scoreValue = 0;
       	char *scoreMessage = "SCORE: ";
       	
       	char *helpMessage = "'q' to quit, 'a' and 'd' to move";
       	char *sepMessage = "|";
        
        // game loop
        while (1) {
                printf("\033[H\033[J");

                // set bounderies for tank
                if (tankPositionX < 1) tankPositionX = 1;
                if (tankPositionX > maxScreenX - 2)
                        tankPositionX = maxScreenX - 2;

                // plot characters
                plot(tankPositionX, tankPositionY, tankChar);
                plot(enemyPositionX, enemyPositionY, enemyChar);
		plot(1, 1, scoreMessage);
		ploti(8, 1, scoreValue);
		plot(maxScreenX / 2, 1, sepMessage);
		plot(maxScreenX - strlen(helpMessage), 1, helpMessage);

                // if bulletActive, y--
                if (bulletActive) {
                        bulletPositionY--;

                        if (bulletPositionY <= 1) {
                                bulletActive = 0;
			}
			if (bulletPositionY == enemyPositionY && bulletPositionX == enemyPositionX) {
				scoreValue++;
				enemyPositionX = rand() % maxScreenX;
				bulletActive = 0;                              

                        } else {
                                plot(bulletPositionX, bulletPositionY, bulletChar);
                        }
                }

                // get inputs
                char input = 0;
                read(STDIN_FILENO, &input, 1);

                // controls
                if (input == 'a') tankPositionX--;
                if (input == 'd') tankPositionX++;
                if (input == ' ' && !bulletActive) {
                        bulletActive = 1;
                        bulletPositionX = tankPositionX + 1;
                        bulletPositionY = tankPositionY - 1;
                }

                if (input == 'q') break;
                usleep(10000);
        }

        // end game
        disableRawMode();
        printf("\033[?25h");
        system("tput rmcup");
        return 0;
}
