#include <stdio.h>
#include <math.h>
#include <stdio.h>
#include <ncurses.h>
#include <stdint.h>

#define S_RATE  (44100)
#define B_SIZE  (1024)

//fmod equation
//x(t)=A(t)*[cos(2pi*fc*t + I(t)cos(2pi*fm*t+osc2_t) + osc1_t]
//A(t) time varying amplitude
//I(t) modulation index
//fc the carrier frequency
//fm the modulating frequency
//osc2_t,c phase constants


int main() {
    uint16_t buffer[B_SIZE];
    float p1=0,p2=0,p3=0;
    float osc1=40,osc2=80,osc3=40;
    float A=50;
    float osc2_t=0,osc1_t=0,osc3_t=0;
    int osc1_type=0,osc2_type=0,osc3_type=0;
    int third_mod=0;
    float I_t=0;
    int sign=1;
    int index=0;
    char ch;
    WINDOW *w = initscr();
    cbreak();
    nodelay(w, TRUE);
    noecho();
    while((ch = getch())) {
        if(ch=='p') { break; }
        else if(ch=='q') { osc1*=2; }
        else if(ch=='w') { osc1/=2; }
        else if(ch=='a') { osc2*=2; }
        else if(ch=='s') { osc2/=2; }
        else if(ch=='z') { osc3*=2; }
        else if(ch=='x') { osc3/=2; }
        else if(ch=='d') { osc2_t+=.01; }
        else if(ch=='f') { osc2_t-=.01; }
        else if(ch=='c') { osc1_t+=.01; }
        else if(ch=='v') { osc1_t-=.01; }
        else if(ch=='e') { osc3_t+=.01; }
        else if(ch=='r') { osc3_t-=.01; }
        else if(ch=='b') { osc1_type++;osc1_type%=3; }
        else if(ch=='g') { osc2_type++;osc2_type%=3; }
        else if(ch=='t') { osc3_type++;osc3_type%=3; }
        else if(ch=='m') { third_mod++;third_mod%=3; }

        p1 += osc1*2*M_PI/S_RATE+osc1_t;
        p2 += osc2*2*M_PI/S_RATE+osc2_t;
        p3 += osc3*2*M_PI/S_RATE+osc3_t;
        I_t += 0.0001*sign;
        if(I_t>100||I_t<0) sign=-sign;

        if(third_mod==0) {
            float c1=cos(p2);
            if(osc2_type==1) c1=c1<=0?-1:1;
            float c2=cos(p1+I_t*c1);
            if(osc1_type==1) c2=c2<=0?-1:1;
            buffer[index]=A*c2;
        }

        else if(third_mod==1) {
            float c1=cos(p2);
            if(osc2_type==1) c1=c1<=0?-1:1;
            float c2=cos(p1+I_t*c1);
            if(osc1_type==1) c2=c2<=0?-1:1;
            float c3=cos(p3+I_t*c2);
            if(osc3_type==1) c3=c3<=0?-1:1;
            buffer[index]=A*c3;
        }
        else if(third_mod==2) {
            float c1=cos(p2);
            if(osc2_type==1) c1=c1<=0?-1:1;
            float c2=cos(p1);
            if(osc1_type==1) c2=c2<=0?-1:1;
            float c3=cos(p3+I_t*c2+I_t*c1);
            if(osc3_type==1) c3=c3<=0?-1:1;
            buffer[index]=A*c3;
        }


        index++;
        if(index%B_SIZE==0) {
            fprintf(stderr,"osc1=%02.2f/%d\tosc2=%02.2f/%d\tosc3=%02.2f/%d\tt_c=%02.2f\tt_m=%02.2f\tt_ck=%02.2f\tI_t=%02.2f\t%s\r\n",
                osc1,osc1_type,osc2,osc2_type,osc3,osc3_type,osc1_t,osc2_t,osc3_t,I_t,
                third_mod==0?"osc2->osc1":third_mod==1?"osc1->osc2->osc3":"osc1->osc2<-osc3");
            fwrite(buffer, sizeof(uint16_t), B_SIZE, stdout);
            index=0;
        }
    }
    endwin();
}
