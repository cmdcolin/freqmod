#include <stdio.h>
#include <math.h>
#include <stdio.h>
#include <ncurses.h>

#define S_RATE  (44100)
 
int buffer[S_RATE];
int main() {
    float phase=0;
    float freq_Hz=440;
    float amplitude=3200000;
    char ch;
    WINDOW *w = initscr();
    cbreak();
    nodelay(w, TRUE);
    noecho();
    while(1) {
        for(int i=0;i<S_RATE;i++) {
            if((ch=getch())!=-1) {
                if(ch=='q') { endwin();return 0; }
                else if(ch=='u') { freq_Hz+=100; }
                else if(ch=='d') { freq_Hz-=100; }
            }
            float freq_radians_per_sample = freq_Hz*2*M_PI/S_RATE;
            phase += freq_radians_per_sample; 
            buffer[i]=(int) (amplitude*sin(phase));
        }
        fwrite(buffer, sizeof(int), S_RATE, stdout);
    }
}
