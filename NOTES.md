## Possible escape sequences

Before I got the manual from Kirk, when I was trying to disassemble the ROM...

```
<ESC>An - volume
<ESC>Pn - pitch
<ESC>Rn - rate
<ESC>Vn - tone
<ESC>S? - ??
<ESC>M? - ??
<ESC>Tn - timeout
<ESC>0n - if n=0 makes nonresponsive for a few seconds
<ESC>Yn - ?? valid options appear to be Y,y,X,x,Z,z
<ESC>?n - ?? valid options appear to be S,s,V,W,w
<ESC>Nn - ??
<ESC>On - ?? valid options appear to be L,l,K,k,J,j,A,a,I,i,S,s,R,r,T,t,P,p,M,m,N,n
<ESC>=
<ESC>|

Initialization sequence from speakup
  <ESC>=X  ; initialize
  <ESC>Oi  ; disable abbreviations
  <ESC>T2  ; timeout = 2
  <ESC>=M  ; ctl-x as instant flush command
  <ESC>N1  ; set number processor to 1

P,R,V,S,M all call x05FA with B set to 0,1,2,3,4

04e4: fe 41     cpi  $41        ; A
04e6: ca d8 06  jz   $06d8
04e9: fe 59     cpi  $59        ; Y
04eb: ca 6f 04  jz   $046f
04ee: fe 50     cpi  $50        ; P
04f0: 06 01     mvi  b,$01
04f2: ca fa 05  jz   $05fa
04f5: 04        inr  b
04f6: fe 52     cpi  $52        ; R
04f8: ca fa 05  jz   $05fa
04fb: 04        inr  b
04fc: fe 56     cpi  $56        ; V
04fe: ca fa 05  jz   $05fa
0501: 04        inr  b
0502: fe 53     cpi  $53        ; S
0504: ca fa 05  jz   $05fa
0507: 04        inr  b
0508: fe 4d     cpi  $4d        ; M
050a: ca fa 05  jz   $05fa
050d: fe 3f     cpi  $3f        ; ?
050f: ca 6b 08  jz   $086b
0512: fe 54     cpi  $54        ; T
0514: ca 4e 09  jz   $094e
0517: fe 4f     cpi  $4f        ; O
0519: ca b5 05  jz   $05b5
051c: fe 4e     cpi  $4e        ; N
051e: ca 90 05  jz   $0590
0521: fe 30     cpi  $30        ; 0
0523: ca 35 05  jz   $0535
0526: cd bb 16  call $16bb

06ec: f5        push psw
06ed: 7d        mov  a,l
06ee: fe 41     cpi  $41     ; A
06f0: ca 11 07  jz   $0711
06f3: fe 50     cpi  $50     ; P
06f5: ca 2e 07  jz   $072e
06f8: fe 52     cpi  $52     ; R
06fa: ca 8b 07  jz   $078b
06fd: fe 56     cpi  $56     ; V
06ff: ca 6b 07  jz   $076b
0702: fe 53     cpi  $53     ; S
0704: ca 4c 07  jz   $074c
0707: fe 4d     cpi  $4d     ; M
0709: ca de 07  jz   $07de
070c: f1        pop  psw

A 0711  ->  7CA0
P 072E  ->  7B7F
R 078B  ->  7B80
V 076B  ->  781B
S 074C  ->  781D
M 07DE  ->  7812
```