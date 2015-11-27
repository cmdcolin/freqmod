#include <stdio.h>
#include <math.h>
#include <stdio.h>
#include <ncurses.h>

#define S_RATE  (44100)

int buffer[S_RATE];
int main() {
    float p1=0,p2=0;
    float f_c=40,f_m=80;
    float amplitude=3200000;
    float theta_m=0;
    float theta_c=0;
    float I_t=0;
    int sign=1;
    char ch;
    WINDOW *w = initscr();
    cbreak();
    nodelay(w, TRUE);
    noecho();
    while(1) {
        for(int i=0;i<S_RATE;i++) {
            if((ch=getch())!=-1) {
                if(ch=='p') { endwin(); return 0; }
                else if(ch=='f') { f_c*=2; }
                else if(ch=='F') { f_c/=2; }
                else if(ch=='m') { f_m*=2; }
                else if(ch=='M') { f_m/=2; }
                else if(ch=='t') { theta_m+=.001; }
                else if(ch=='T') { theta_m-=.001; }
                else if(ch=='c') { theta_c+=.001; }
                else if(ch=='C') { theta_c-=.001; }
                fprintf(stderr,"f_c=%02.2f f_m=%02.2f theta_c=%02.2f theta_m=%02.2f I_t=%02.2f\r\n",f_c,f_m,theta_c,theta_m,I_t);
            }
            p1 += f_c*2*M_PI/S_RATE+theta_c;
            p2 += f_m*2*M_PI/S_RATE+theta_m;
            I_t += 0.0001*sign;
            if(I_t>100||I_t<0) sign=-sign;
            
            //x(t)=A(t)*[cos(2pi*fc*t + I(t)cos(2pi*fm*t+theta_m) + theta_c]
            //A(t) time varying amplitude
            //I(t) modulation index
            //fc the carrier frequency
            //fm the modulating frequency
            //theta_m,c phase constants

            buffer[i]=(int) (amplitude*(cos(p1+I_t*(cos(p2)))));
        }
        fwrite(buffer, sizeof(int), S_RATE, stdout);
    }
}
