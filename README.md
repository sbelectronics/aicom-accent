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

# The writeable bit is '0' if the synth is writeable

Writeable = (inp(sync_port_control) & SYNTH_WRITABLE)!-SYNTH_WRITEABLE;

flush:
   outp(port_tts, SYNTH_CLEAR)

# Processes speech on a '\r'. It won't talk immediately unless you end with punctuation such as `.`.
# Maybe want to convert '\n' to '\r'... not sure.

speak_loop:
   # assume s is a string
   while (*s) {
      while !Writeable ;
      outp(synth_port_tts, *s)
      s++;

# Get the current status of the synth
#   S = silent
#   s = talking
#   F = full
#   M = almost done, less than 2s left

status = inp(synth_port_control)
```





  