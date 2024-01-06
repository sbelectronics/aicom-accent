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

static void set_timeout(unsigned int x);

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
    /* SMBAKER: assuming "writeable" means we can put a character into the 
     * ring buffer, but does not ensure the ring buffer isn't "full" and 
     * we won't overwrite data.
     */

    /* The WRITABLE bit is 0 if we can write, 1 if we cannot */
	return ((inp(synth_port_control) & SYNTH_WRITABLE)!=SYNTH_WRITABLE);
}

static int synth_full(void)
{
    return (inp(synth_port_tts) == 'F');
}


static void wait_lots_to_say(void)
{
    unsigned int x;
    while (1) {
        x = inpw(synth_port_control) & 0xfffc;
        if (x!=0x73fc) {  // little s == lots to say
            return;
        }
    }
}

static void say_char(char ch)
{
    /* SMBAKER: Living dangerously with no timeouts here */
    while (synth_full()) ;
    while (!synth_writable()) ;
    outp(synth_port_tts, ch);
}

static void say_line(char *s)
{
    char last;
    last = 0;
    while (*s) {
        if (*s=='\n') {
            if (last!='\r') {   // only emit one \r on \r\n
                say_char('\r');
            }
        } else if (*s=='\r') {  // only emit one \r on \n\r
            if (last!='\n') {
                say_char('\r');
            }
        } else {
            say_char(*s);
        }
        last = *s;
        s++;
    }
}

void say_stream(FILE *f, int prompt) {
    set_timeout(1);  // make sure we don't wait for punctuation
    while (1) {
        char *res;

        if (prompt) {
            printf(">");
        }
        res = fgets(line, sizeof(line)-1, f);
        if (res == NULL) {
            /* EOF */
            goto out;
        }
        if (!prompt) {
            //wait_lots_to_say();
            fputs(line, stdout);
        }
        say_line(line);
    }
out:
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

static void set_timeout(unsigned int x)
{
    say_char(0x1B);
    say_char('T');
    say_char('0' + x);    
}

static void reset()
{
    say_char(0x1B);
    say_char('=');
    say_char('R');    
}

static void init()
{
    say_char(0x1B);
    say_char('=');
    say_char('X');    
}

static void version()
{
    say_char(0x1B);
    say_char('|');
    say_char('1');    
}

static void set_spell(int x)
{
    say_char(0x1B);
    say_char('O');
    if (x) {
        say_char('t');
    } else {
        say_char('T');
    }
}

static void set_cr(int x)
{
    say_char(0x1B);
    say_char('=');
    if (x) {
        say_char('F');
    } else {
        say_char('f');
    }
}

static void flush()
{
    say_char(0x1B);
    say_char('=');
    say_char('x');  
}

static void check_2args(int i, int argc, char **argv)
{
    if (i+1>=argc) {
        fprintf(stderr, "Argument %s needs a paramter", argv[i]);
        exit(1);
    }
}

static void banner(void)
{
    fprintf(stdout, "say.com for Aicom Accent\n");
    fprintf(stdout, "Scott Baker, https://www.smbaker.com/\n");
}

static void show_help(void)
{
    banner();
    fprintf(stdout,"\n-F <fn> ... say file\n");
    fprintf(stdout,"-Q      ... quiet / omit console output\n");
    fprintf(stdout,"-V      ... verbose / show debugging console output\n");
    fprintf(stdout,"-U      ... do not flush on exit\n");
    fprintf(stdout,"-D      ... set volume, pitch, rate, tone to defaults\n");
    fprintf(stdout,"-A <n>  ... set volume to n (0..9)\n");
    fprintf(stdout,"-P <n>  ... set pitch to n (0..9)\n");
    fprintf(stdout,"-R <n>  ... set rate to n (0..17)\n");
    fprintf(stdout,"-T <n>  ... set tone to n (0..9)\n");
    fprintf(stdout,"-M <n>  ... set timeout\n");
    fprintf(stdout,"-L <n>  ... spell (n=1) or speak (n=0)\n");
    fprintf(stdout,"-I      ... initialize\n");
    fprintf(stdout,"-Y      ... report version\n");
    fprintf(stdout,"-Z      ... reset\n");
}

int main(int argc, char **argv)
{
    int i, firstArg;
    char *fn = NULL;
    int _pitch, _tone, _volume, _rate, _timeout, _noflush, 
        _flush, _reset, _init, _version, _spell, _stream;
    int verbosity;

    verbosity = 1;
    _pitch = -1;
    _tone = -1;
    _volume = -1;
    _rate = -1;
    _timeout = -1;
    _spell = -1;
    _noflush = 0;
    _reset = 0;
    _init = 0;
    _version = 0;
    _flush = 0;
    _stream = 0;
    synth_port_control = 0x2A8;
    synth_port_tts = synth_port_control+1;

    if (argc==1) {
        show_help();
        exit(0);
    }

    /* Assume all the args are before all the words to say */

    for (i=1; i<argc; i++) {
        if ((argv[i][0] == '-') || (argv[i][1] == '/')) {
            switch (toupper(argv[i][1])) {
                case 'F':
                    check_2args(i, argc, argv);
                    i++;
                    fn = argv[i];
                    break;
                case 'P':
                    check_2args(i, argc, argv);
                    i++;
                    _pitch = atoi(argv[i]);
                    break;
                case 'A':
                    check_2args(i, argc, argv);
                    i++;
                    _volume = atoi(argv[i]);
                    break;
                case 'T':
                    check_2args(i, argc, argv);
                    i++;
                    _tone = atoi(argv[i]);
                    break;
                case 'R':
                    check_2args(i, argc, argv);
                    i++;
                    _rate = atoi(argv[i]);
                    break;
                case 'M':
                    check_2args(i, argc, argv);
                    i++;
                    _timeout = atoi(argv[i]);
                    break;
                case 'L':
                    check_2args(i, argc, argv);
                    i++;
                    _spell = atoi(argv[i]);
                    break;                          
                case 'D':
                    _rate = 5;
                    _pitch = 5;
                    _volume = 9;
                    _tone = 5;
                    break;
                case 'U':
                    _noflush = 1;
                    break;
                case 'Q':
                    verbosity = 0;
                    break;
                case 'V':
                    verbosity = 2;
                    break;
                case 'I':
                    _init = 1;
                    break;
                case 'X':
                    _flush = 1;
                    break;
                case 'Y':
                    _version = 1;
                    break;
                case 'Z':
                    _reset = 1;
                    break;
                case 'S':
                    _stream = 1;
                    break;
                case 'H':
                    show_help();
                    exit(0);
                    break;
                default:
                    fprintf(stderr, "Unknown switch %s", argv[i]);
                    show_help();
                    exit(1);
                    break;
            }
        } else {
            break;
        }
    }
    firstArg = i;

    if (verbosity>=1) banner();

    if (!detect()) {
        fprintf(stderr, "Aicom Accent Not Detected\n");
        exit(-1);
    }

    if (verbosity>=2) fprintf(stdout, "speech board detected\n");

    if (_init) init();
    if (_reset) reset();
    if (_flush) flush();
    if (_tone>=0) set_tone(_tone);
    if (_rate>=0) set_rate(_rate);
    if (_pitch>=0) set_pitch(_pitch);
    if (_volume>=0) set_volume(_volume);
    if (_timeout>=0) set_timeout(_timeout);
    if (_spell>=0) set_spell(_spell);
    if (_version) version();

    if (fn!=NULL) {
        say_file(fn);
    }

    for (i=firstArg; i<argc; i++) {
        if (i>firstArg) {
            say_char(' ');
        }
        say_line(argv[i]);
    }

    if (_stream) {
        say_stream(stdin, 1);
    }

    if ((!_noflush) && ((firstArg<argc) || (fn!=NULL))) {
        say_char('.');   /* This will cause it to say the sentence, otherwise it will wait */
        say_char('\r');  /* I'm not sure this actually does anything... */
    }

    return 0;
}
