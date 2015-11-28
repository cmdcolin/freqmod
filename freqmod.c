#include <stdio.h>
#include <math.h>
#include <stdlib.h>
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
//osc1_t,osc2_t phase constants

struct synth_state {
    float osc1;
    float osc2;
    float osc3;
    float osc1_t;
    float osc2_t;
    float osc3_t;
    int osc1_type;
    int osc2_type;
    int osc3_type;
    int third_mod;
};

int save_synth(char *,struct synth_state *);
int open_synth(char *,struct synth_state *);

int main() {
    uint16_t buffer[B_SIZE];
    struct synth_state s = {40,80,40,0,0,0,0,0,0,0};
    if(open_synth(".synthrc",&s)<1) { fprintf(stderr,"Using synth defaults"); }
    
    float A=50;
    float t=0;
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
        else if(ch=='q') { s.osc1*=2; }
        else if(ch=='w') { s.osc1/=2; }
        else if(ch=='a') { s.osc2*=2; }
        else if(ch=='s') { s.osc2/=2; }
        else if(ch=='z') { s.osc3*=2; }
        else if(ch=='x') { s.osc3/=2; }
        else if(ch=='e') { s.osc1_t+=.01; }
        else if(ch=='r') { s.osc1_t-=.01; }
        else if(ch=='d') { s.osc2_t+=.01; }
        else if(ch=='f') { s.osc2_t-=.01; }
        else if(ch=='c') { s.osc3_t+=.01; }
        else if(ch=='v') { s.osc3_t-=.01; }
        else if(ch=='t') { s.osc1_type++;s.osc1_type%=3; }
        else if(ch=='g') { s.osc2_type++;s.osc2_type%=3; }
        else if(ch=='b') { s.osc3_type++;s.osc3_type%=3; }
        else if(ch=='m') { s.third_mod++;s.third_mod%=3; }
        else if(ch=='o') { if(save_synth(".synthrc",&s)<0) { perror("save_synth"); break; } }


        t += 1.0/S_RATE;
        float p1 = s.osc1*2*M_PI*t+s.osc1_t;
        float p2 = s.osc2*2*M_PI*t+s.osc2_t;
        float p3 = s.osc3*2*M_PI*t+s.osc3_t;

        I_t += 0.0001*sign;
        if(I_t>100||I_t<0) sign=-sign;

        if(s.third_mod==0) {
            float c1=cos(p1+s.osc1_t);
            if(s.osc1_type==1) c1 = c1<=0?-1:1;
            if(s.osc1_type==2) c1 = 1/s.osc1 * (s.osc1 - abs(index % (int) (s.osc1*2) - s.osc1) );
            float c2=cos(p2+I_t*c1+s.osc2_t);
            if(s.osc2_type==1) c2 = c2<=0?-1:1;
            if(s.osc2_type==2) c2 = 1/s.osc2 * (s.osc2 - abs(index % (int) (s.osc2*2) - s.osc2) );
            buffer[index]=A*c2;
        }

        else if(s.third_mod==1) {
            float c1=cos(p1);
            if(s.osc1_type==1) c1 = c1<=0?-1:1;
            if(s.osc1_type==2) c1 = 1/s.osc1 * (s.osc1 - abs(index % (int) (s.osc1*2) - s.osc1) );
            float c2=cos(p2+I_t*c1);
            if(s.osc2_type==1) c2 = c2<=0?-1:1;
            if(s.osc2_type==2) c2 = 1/s.osc2 * (s.osc2 - abs(index % (int) (s.osc2*2) - s.osc2) );
            float c3=cos(p3+I_t*c2);
            if(s.osc3_type==1) c3 = c3<=0?-1:1;
            if(s.osc3_type==2) c3 = 1/s.osc3 * (s.osc3 - abs(index % (int) (s.osc3*2) - s.osc3) );
            buffer[index]=A*c3;
        }
        else if(s.third_mod==2) {
            float c1=cos(p2);
            if(s.osc2_type==1) c1 = c1<=0?-1:1;
            if(s.osc2_type==2) c1 = 1/s.osc2 * (s.osc2 - abs(index % (int) (s.osc2*2) - s.osc2) );
            float c2=cos(p1);
            if(s.osc1_type==1) c2 = c2<=0?-1:1;
            if(s.osc1_type==2) c2 = 1/s.osc1 * (s.osc1 - abs(index % (int) (s.osc1*2) - s.osc1) );
            float c3=cos(p3+I_t*c2+I_t*c1);
            if(s.osc3_type==1) c3 = c3<=0?-1:1;
            if(s.osc3_type==2) c3 = 1/s.osc3 * (s.osc3 - abs(index % (int) (s.osc3*2) - s.osc3) );
            buffer[index]=A*c3;
        }


        index++;
        if(index%B_SIZE==0) {
            if(s.third_mod>0) {
                werase(w);
                fprintf(stderr,"osc1=%02.2f/%d\tosc2=%02.2f/%d\tosc3=%02.2f/%d\tt_c=%02.2f\tt_m=%02.2f\tt_ck=%02.2f\tI_t=%02.2f\t%s\r",
                    s.osc1,s.osc1_type,s.osc2,s.osc2_type,s.osc3,s.osc3_type,s.osc1_t,s.osc2_t,s.osc3_t,I_t,
                    s.third_mod==1?"osc1->osc2->osc3":"osc1->osc2<-osc3");
            }
            else {
                werase(w);
                fprintf(stderr,"osc1=%02.2f/%d\tosc2=%02.2f/%d\tt_c=%02.2f\tt_m=%02.2f\tI_t=%02.2f\t%s\r",
                    s.osc1,s.osc1_type,s.osc2,s.osc2_type,s.osc1_t,s.osc2_t,I_t,"osc1->osc2");
            }
            fwrite(buffer, sizeof(uint16_t), B_SIZE, stdout);
            index=0;
        }
    }
    endwin();
}



int open_synth(char *filename,struct synth_state* state) {
    FILE *f=fopen(filename, "r");
    if(f) {
        fscanf(f, "osc1=%f\n",&state->osc1);
        fscanf(f, "osc2=%f\n",&state->osc2);
        fscanf(f, "osc3=%f\n",&state->osc3);
        fscanf(f, "osc1_t=%f\n",&state->osc1_t);
        fscanf(f, "osc2_t=%f\n",&state->osc2_t);
        fscanf(f, "osc3_t=%f\n",&state->osc3_t);
        fscanf(f, "osc1_type=%d\n",&state->osc1_type);
        fscanf(f, "osc2_type=%d\n",&state->osc2_type);
        fscanf(f, "osc3_type=%d\n",&state->osc3_type);
        fscanf(f, "third_mod=%d\n",&state->third_mod);
        fclose(f);
        return 1;
    }
    else return -1;
}

int save_synth(char *filename,struct synth_state* state) {
    FILE *f=fopen(filename, "w");
    if(f) {
        fprintf(f, "osc1=%f\n",state->osc1);
        fprintf(f, "osc2=%f\n",state->osc2);
        fprintf(f, "osc3=%f\n",state->osc3);
        fprintf(f, "osc1_t=%f\n",state->osc1_t);
        fprintf(f, "osc2_t=%f\n",state->osc2_t);
        fprintf(f, "osc3_t=%f\n",state->osc3_t);
        fprintf(f, "osc1_type=%d\n",state->osc1_type);
        fprintf(f, "osc2_type=%d\n",state->osc2_type);
        fprintf(f, "osc3_type=%d\n",state->osc3_type);
        fprintf(f, "third_mod=%d\n",state->third_mod);
        fclose(f);
        return 1;
    }
    else return -1;
}
