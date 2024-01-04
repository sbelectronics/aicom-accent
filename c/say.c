#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <ctype.h>

/* Some parts borrowed from SpeakUp speech synthesizer driver */

#define SYNTH_CLEAR	0x18
#define SYNTH_READABLE	0x01
#define SYNTH_WRITABLE	0x02

char line[1024];

unsigned int synth_port_control;
unsigned int synth_port_tts;

static int detect(void)
{
    unsigned int x;
    x = inpw(synth_port_control) & 0xfffc;
    // 53 = S = silent
    // 73 = s = lots to say
    // 46 = F = full
    // 4D = M = less than 2 seconds left
    return (x==0x53fc) || (x==0x73fc) || (x==0x46FC) || (x==0x4DFc);
}

static int synth_writable(void)
{
    /* The WRITABLE bit is 0 if we can write, 1 if we cannot */
	return ((inp(synth_port_control) & SYNTH_WRITABLE)!=SYNTH_WRITABLE);
}

static void say_char(char ch)
{
    while (!synth_writable()) ;
    outp(synth_port_tts, ch);
}

static void say_line(char *s)
{
    while (*s) {
        if (*s=='\n') {
            say_char('\r');
        } else {
            say_char(*s);
        }
        s++;
    }
}

void say_stream(FILE *f, int prompt) {
    while (1) {
        char *res;

        if (prompt) {
            printf(">");
        }
        res = fgets(line, sizeof(line)-1, f);
        if (res == NULL) {
            /* EOF */
            return;
        }
        say_line(line);
    }
    return;
}

void say_file(char *fn)
{
    FILE *f = fopen(fn, "rt");

    if (f==NULL) {
        printf("Failed to open %s\n", fn);
        exit(-1);
    }

    say_stream(f, 0);

    fclose(f);
    return;
}

static void set_pitch(unsigned int x)
{
    say_char(0x1B);
    say_char('P');
    say_char('0' + x);
}

static void set_volume(unsigned int x)
{
    say_char(0x1B);
    say_char('A');
    say_char('0' + x);
}

static void set_tone(unsigned int x)
{
    say_char(0x1B);
    say_char('V');
    say_char('0' + x);
}

static void set_rate(unsigned int x)
{
    say_char(0x1B);
    say_char('R');
    if (x<=9) {
        say_char('0' + x);
    } else {
        say_char('a' + x - 10);
    }
}

int main(int argc, char **argv)
{
    int i, firstArg;
    char *fn = NULL;

    synth_port_control = 0x2A8;
    synth_port_tts = synth_port_control+1;

    /* Assume all the args are before all the words to say */

    for (i=1; i<argc; i++) {
        if ((argv[i][0] == '-') || (argv[i][1] == '/')) {
            if (i+1>=argc) {
                fprintf(stderr, "Argument %s needs a paramter", argv[i]);
                exit(1);
            }
            switch (toupper(argv[i][1])) {
                case 'F':
                    i++;
                    fn = argv[i];
                    break;
                case 'P':
                    i++;
                    set_pitch(atoi(argv[i]));
                    break;
                case 'V':
                    i++;
                    set_volume(atoi(argv[i]));
                    break;
                case 'T':
                    i++;
                    set_tone(atoi(argv[i]));
                    break;
                case 'R':
                    i++;
                    set_rate(atoi(argv[i]));
                    break;
                default:
                    fprintf(stderr, "Unknown switch %s", argv[i]);
                    exit(1);
                    break;
            }
        } else {
            break;
        }
    }
    firstArg = i;

    fprintf(stdout, "say.com for Aicom Accent\n");
    fprintf(stdout, "Scott Baker, https://www.smbaker.com/\n");

    if (!detect()) {
        fprintf(stderr, "Aicom Accent Not Detected\n");
        exit(-1);
    }

    fprintf(stdout, "speech board detected\n");

    if (fn!=NULL) {
        say_file(fn);
    }

    for (i=firstArg; i<argc; i++) {
        if (i>1) {
            say_char(' ');
        }
        say_line(argv[i]);
    }

    if ((firstArg<argc) || (fn!=NULL)) {
        say_char('.');   /* This will cause it to say the sentence, otherwise it will wait */
        say_char('\r');  /* I'm not sure this actually does anything... */
    }
}
