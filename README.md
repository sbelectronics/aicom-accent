# Aicom Accent Notes

Scott Baker, https://www.smbaker.com/

## Port Numbers

* Base Port: 0x2A8

* Port_Control = base
* Port_TTS = base+1

Top 8 bits of 0x2A8 seems to match the dip switches on my board next to the LS688: 1010101000.

## Constants 

* SYNTH_READABLE = 0x01

* SYNTH_WRITABLE = 0x02

* SYNTH_CLEAR = 0x18

## Talking to the board

To detect, read a word from the port and AND with 0xFFFC. Should get back 0x53FC if the synthesizer is currently silent.


```pseudocode

# All of the inp() and outp() below are bytes, not words.

# The writeable bit is '0' if the synth is writeable. Note that writeable is separate from 'full'. The synth can
# be writeable and full, and you'll overwrite the buffer.

Writeable = (inp(sync_port_control) & SYNTH_WRITABLE)!-SYNTH_WRITEABLE;

Full = (inp(sync_port_tts) == 'F');

flush:
   outp(port_tts, SYNTH_CLEAR)

# Processes speech on a '\r'. It won't talk immediately unless you end with punctuation such as `.`.
# Maybe want to convert '\n' to '\r'... not sure.

speak_loop:
   # assume s is a string
   while (*s) {
      while Full() ;
      while !Writeable() ;
      outp(synth_port_tts, *s)
      s++;

# Get the current status of the synth
#   S = silent
#   s = talking
#   F = full
#   M = almost done, less than 2s left

status = inp(synth_port_control)
```


## from vtechworks description

```
** On-board microprocessor.
** 800 byte wrap-around text buffer.
** IBM PC bus compatible; I/O addresses switch selectable.
** Standard RS-232C serial port; baud rate, parity and data
bits switch selectable; DTE or DCE set-up jumper selectable.
** Programmable automatic time-out for text input without
sentence punctuation from the host; 0.04 to 20 seconds.
** Text mode or spell mode speech.
** Various selections of voice characteristics for
different personalities.
** Phoneme input capability can be mixed with text.
** Escape sequences for software commands.
** User definable dictionary. 
```

## Escape sequences

From accent manual

```
System Option Commands

<ESC>=R                     Reset Accent
<ESC>=X                     Initialize Accent per H/W set-up
<ESC>=x                     Flush all internal buffers
<ESC>=E       **            RS-232C echo on
<ESC>=e       **     SEL    RS-232C echo off
<ESC>=C              SEL    Append <CR> for Status Reply
<ESC>=c                     Append no <CR>for Status Reply
<ESC>=A              SEL    Enable speech synthesizer
<ESC>=a                     Disable speech synthesizer
<ESC>=Q                     Speech pause
<ESC>=q              SEL    Speech continue
<ESC>Tn       **     7      Host input time-out; n = 0 to 9
<ESC>=F                     Enable <CR> as alternative 
                            start-speaking character 
<ESC>=f              SEL    Disable <CR> as alternative
                            start-speaking character 
<ESC>=V       **            Enable CTL-Q/S as Xon/Xoff char.
<ESC>=v       **     SEL    Disable CTL-Q/S as Xon/Xoff char.
<ESC>=B                     Enable fast processing mode
<ESC>=b              SEL    Disable fast processing mode
<ESC>=J              SEL    Enable DCD verbal message
<ESC>=j                     Disable DCD verbal message
<ESC>=K              SEL    Process Speech Option Commands 
                            only when speech buffer is empty
<ESC>=k                     Process Speech Option Commands
                            immediately 
<ESC>=U                     Verbal stress on single character
<ESC>=u              SEL    No stress on single character
<ESC>An              9      Set Audio Volume; n = 0 to 9
<ESC>+A                     Increase Audio Volume by 1 level
<ESC>-A                     Decrease Audio Volume by 1 level
<ESC>+P                     Increase Average Pitch by 1 level
<ESC>-P                     Decrease Average Pitch by 1 level
<ESC>+R                     Increase Speech Rate by 1 level
<ESC>-R                     Decrease Speech Rate by 1 level
<ESC>+V                     Increase Voice Characteristic by 1 
                            level 
<ESC>-V                     Decrease Voice Characteristic by 1 
                            level 
<ESC>+S                     Increase Space Pause Time by 1
                            level
<ESC>-S                     Decrease Space Pause Time by 1 
                            level
<ESC>+M                     Increase Sentence Intonation by 1
                            level 
<ESC>-M                     Decrease Sentence Intonation by 1
                            level 
<ESC>=P       *             Enable PC to RS-232C transmission
<ESC>=p       *      SEL    Disable PC to RS-232C transmission
<ESC>=L       *             Command string sent to Aux port
<ESC>=l       *      SEL    Command string not sent to Aux 
                            port
<ESC>=G       *      SEL    Status Reply to Host only
<ESC>=g       *             Status Reply to Aux port only
<ESC>=H       *             Status Reply to Host and to Aux 
                            port
<ESC>=Z       ***           Enable chained LPT operation for 
                            PC via device driver
<ESC>=z       ***    SEL    Disable chained LPT operation for
                            PC via device driver
<ESC>HP       *      SEL    Set PC Host
<ESC>HS       *             Set RS-232C Host
<ESC>=M                     Enable CTL-X as the instant Flush 
                            command 
<ESC>=m              SEL    Disable CTL-X as the instant Flush
                            command 
<ESC>=I       **            Enable Xon-always-sent after Flush
<ESC>=i       **     SEL    Disable Xon-always-sent after Flush
<ESC>=Y       **            Enable index marker Auto-Prompt
<ESC>=y       **     SEL    Disable index marker Auto-Prompt
<ESC>=W       ****          Enable foot switch status reply
<ESC>=w       ****   SEL    Disable foot switch status reply

Speech Option Commands

<ESC>OT              SEL    Text mode
<ESC>Ot                     Spell mode
<ESC>Op              SEL    Punctuation will not be spoken
<ESC>OP                     Punctuation will be spoken
<ESC>Om              SEL    <-> will not be spoken
<ESC>OM                     <-> will be spoken as DASH 
<ESC>ON                     <-> will be spoken as MINUS
<ESC>OI              SEL    Enable Accent's standard 
                            abbreviation table
<ESC>Oi                     Disable Accent's standard
                            abbreviation table
<ESC>OR              SEL    Full-set punctuation spoken
<ESC>Or                     Subset punctuation (,.;:!?) spoken 
<ESC>OA                     <A> or <a> always spoken as /EI/
<ESC>Oa              SEL    <A> always spoken as /EI/, <a> is 
                            spoken as /EI/ or UU/ dependent
                            on context 
<ESC>Pn              5      Average pitch level; n = 0 to 9
<ESC>Vn              5      Voice characteristic; n = 0 to 9 
<ESC>Rn              5      Speech rate; n = 0 to F, G, and H
<ESC>Mn              0      Sentence level intonation; n = 0 (full
                            intonation), 1 (monotone), 2, 3, or 4 
<ESC>Sn              0      Space pause time; n = 0 to 9
<ESC>Nn              0      Set number processor n = 0,1,2
<ESC>|~..~                  Direct parameter input to
                            synthesizer chip
<ESC>OL                     Fixed Abbreviation Pronunciation
<ESC>Ol              SEL    Automatic Abbreviation
                            Pronunciation

Status Request Commands

<ESC>?S                     Reply <S> when speech has ended
<ESC>?s                     Reply synthesizer status
                            immediately;  
                            <S> for speech--end,
                            <s> for speech-not-end,
                            and <M> speech-near-end
<ESC>|1                     Reply Accent's version verbally 

Index Marker Commands

<ESC>YX                     Enable CTL-F as the index marker
<ESC>Yx              SEL    Disable CTL-F as the index marker
<ESC>YY       **            Reply <ESC>Y plus index-marker 
                            character for Auto-Prompt
<ESC>Yy       **     SEL    Reply index-marker character only 
                            for Auto-Prompt 
<ESC>YZ                     Enable index-marker-count replying
                            for Status Reply 
<ESC>Yz              SEL    Disable index-marker-count replying
                            for Status Reply
<ESC>Y_                     User defined index-marker
```