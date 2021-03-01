//
// Created by maxwe on 2021-02-26.
//

#include <string.h>
#include <stdio.h>


/*

#define N(a)       "%"#a"$hhn"
#define O(a,b)     "%10$"#a"d"N(b)
#define U          "%10$.*37$d"
#define G(a)       "%"#a"$s"
#define H(a,b)     G(a)G(b)
#define T(a)       a a
#define s(a)       T(a)T(a)
#define A(a)       s(a)T(a)a
#define n(a)       A(a)a
#define D(a)       n(a)A(a)
#define C(a)       D(a)a
#define R          C(C(N(12)G(12)))
#define o(a,b,c)   C(H(a,a))D(G(a))C(H(b,b)G(b))n(G(b))O(32,c)R
#define SS         O(78,55)R "\n\033[2J\n%26$s";
#define E(a,b,c,d) H(a,b)G(c)O(253,11)R G(11)O(255,11)R H(11,d)N(d)O(253,35)R
#define S(a,b)     O(254,11)H(a,b)N(68)R G(68)O(255,68)N(12)H(12,68)G(67)N(67)






    S(a,b) {
        [46] = (ACC += 254);
        [60] = ACC + a + b;
        ACC = 0;
        ACC = ![60]
        [60] = ACC
        [52] = ACC
        ACC += [52] + [60] + [58]
        [58] = ACC
    }



    E(a,b,c,d) {
      [46] = (a & b & c);
      e = d + [46];
      d = e;
      [97] = e + 253;
      ACC = 0
    }

    o(a,b,c) {
        if (a)
           c = O
        else if (b)
           c = X
    }

    U {
        ACC += [2]
    }

    R {
        ACC = 0
    }

    G(a) {
        ACC += a
    }

    H(a,b) {
        ACC += a + b
    }

    O(a,b) {
        b = (ACC += a)
    }

    SS {
        [101] = ACC + 78;
        ACC = 0;
        Clear all...
        print("[2J\n");
        print([66])
    }


// N(a)       -> a = ACC
// O(a,b)     -> b = (ACC += a)
// U          -> */





/*

    d[71], d[77], d[83], d[89], d[95], d[103], d[98], d[42] <- 10 ('\n')
    d[98]    <- 32 (' ')
    d[126]   <- 37 ('%')
    d[72:76] <- 45 ('-')
    d[84:88] <- 45 ('-')

    d[100] <- 73 ('I')
    d[101] <- 78 ('N')
    d[96] <- 80 ('P')
    d[102] <- 83 ('S')
    d[99] <- 87 ('W')
    d[129] <- 100 ('d')
    d[127:128] <- 104 ('h')
    d[67], d[69], d[79], d[81], d[91], d[93] <- 124 ('|')

    ZERO
    ZERO

    d[46] <- d[6]  & d[8]  & d[10];
    d[48] <- d[48] | d[46];
    d[48] <- d[48] | (d[12] & d[14] & d[16]);
    d[48] <- d[48] | (d[18] & d[20] & d[22]);
    d[48] <- d[48] | (d[6] & d[12] & d[18]);
    d[48] <- d[48] | (d[8] & d[14] & d[20]);
    d[48] <- d[48] | (d[10] & d[16] & d[22]);
    d[48] <- d[48] | (d[6] & d[14] & d[22]);
    d[48] <- d[48] | (d[10] & d[14] & d[18]);

    d[50] <- d[50] | (d[24]  & d[26]  & d[28]);
    d[50] <- d[50] | (d[30] & d[32] & d[34]);
    d[50] <- d[50] | (d[36] & d[38] & d[40]);
    d[50] <- d[50] | (d[24] & d[30] & d[36]);
    d[50] <- d[50] | (d[26] & d[32] & d[38]);
    d[50] <- d[50] | (d[28] & d[34] & d[40]);
    d[50] <- d[50] | (d[24] & d[32] & d[40]);
    d[50] <- d[50] | (d[28] & d[32] & d[36]);

    d[2] <- !d[2]
    d[54] <- !d[2]

    d[46] <- d[48] | d[50]

    d[54] <- !(d[46] & d[54])

    d[54] <- !d[54]


*/


// U

// O(255,38)

// R

// G(38)
// O(255,36)

// R

// H(13,23)

// O(255,11)

// R

// H(11,36)

// O(254, 36)

// R

// G(36)

// O(255,36)

// R






char d[538] = { 1, 0, 10, 0, 10 };
int  acc;


/*cstr e[]{ d+6,  d+8,  d+10, d+12, d+14, d+16, d+18, d+20, d+22, 0,
          d+46, d+52, d+48, d+24, d+26, d+28, d+30, d+32, d+34, d+36,
          d+38, d+40, d+50, (scanf(d+126,d+4), d+4+18*(1-d[2]%2)+d[4]*2), d, d+66, d+68, d+70, d+78, d+80,
          d+82, d+90, d+92, d+94, d+97, d+54, d[2], d+2,  d+71, d+77,
          d+83, d+89, d+95, d+72, d+73, d+74, d+75, d+76, d+84, d+85,
          d+86, d+87, d+88, d+100,d+101,d+96, d+102,d+99, d+67, d+69,
          d+79, d+81, d+91, d+93, d+98, d+103,d+58, d+60, d+98, d+126,
          d+127,d+128,d+129 };*/




void accZero(){
  acc &= 0xFFFFFF00;
}

void accWrite(int to) {
  d[to] = acc;
}
void accInc(int a) {
  acc += a;
}
void accIncInd(int a) {
  acc += d[a];
}
void accIncIndTimes(int a, int times) {
  acc += (d[a] * times);
}
void accAppend(int at) {
  acc += strlen(d + at);
}
void accAppendPair(int a, int b) {
  accAppend(a);
  accAppend(b);
}
void accAppendTimes(int at, int mult) {
  acc += strlen(d + at) * mult;
}
void incWrite(int inc, int writeTo) {
  acc += inc;
  d[writeTo] = acc;
}

void writeXO(int a, int b, int to) {
  accAppendTimes(a, 47);
  accAppendTimes(b, 56);
  accInc(32);
  accWrite(to);
  accZero();
}

void checkWin(int a, int b, int c, int out) {
  accAppendPair(a, b);
  accAppend(c);
  incWrite(253, 46);
  accZero();

  accAppend(46);
  incWrite(255, 46);
  accZero();

  accAppendPair(46, out);
  accWrite(out);
  incWrite(253, 97);
  accZero();
}

void checkConflict(int a, int b){

  incWrite(254, 46);
  accAppendPair(a, b);
  accWrite(60);
  accZero();

  accAppend(60);
  incWrite(255, 60);
  accWrite(52);
  accAppendPair(52, 60);
  accAppend(58);
  accWrite(58);
}


void readIn(int fmtAt, int to) {
  scanf(d + fmtAt, d + to);
}
void printAt(int at) {
  printf("\n\033\n%s", d + at);
}
int getPos() {
  return 4 + 18 * (1 - d[2]%2) + d[4]*2;
}


int main(){

  acc = 0;

  while ( *d ) {

    incWrite(10, 71);
    accWrite(77);
    accWrite(83);
    accWrite(89);
    accWrite(95);
    accWrite(103);
    accWrite(98);


    readIn(126, 4);
    accWrite(getPos());

    incWrite(22, 98);
    incWrite(5, 126);
    incWrite(8, 72);
    accWrite(73);
    accWrite(74);
    accWrite(75);
    accWrite(76);
    accWrite(84);
    accWrite(85);
    accWrite(86);
    accWrite(87);
    accWrite(88);

    incWrite(28, 100);
    incWrite(5, 101);
    incWrite(2, 96);
    incWrite(3, 102);
    incWrite(4, 99);
    incWrite(13, 129);
    incWrite(4, 127);
    accWrite(128);

    incWrite(20, 67);
    accWrite(69);
    accWrite(79);
    accWrite(81);
    accWrite(91);
    accWrite(93);

    accZero();
    accZero();


    checkWin(6,  8,  10, 48);
    checkWin(12, 14, 16, 48);
    checkWin(18, 20, 22, 48);

    checkWin(6,  12, 18, 48);
    checkWin(8,  14, 20, 48);
    checkWin(10, 16, 22, 48);

    checkWin(6,  14, 22, 48);
    checkWin(10, 14, 18, 48);


    checkWin(24, 26, 28, 50);
    checkWin(30, 32, 34, 50);
    checkWin(36, 38, 40, 50);

    checkWin(24, 30, 36, 50);
    checkWin(26, 32, 38, 50);
    checkWin(28, 34, 40, 50);

    checkWin(24, 32, 40, 50);
    checkWin(28, 32, 36, 50);

    accZero();


    accIncInd(2);

    incWrite(255, 2);

    accZero();

    accAppend(2);
    incWrite(255, 54);

    accZero();

    accAppendPair(48, 50);
    incWrite(255, 46);

    accZero();

    accAppendPair(46, 54);
    incWrite(254, 54);

    accZero();

    accAppend(54);
    incWrite(255, 54);

    accZero();

    checkConflict(6,  24);
    checkConflict(8,  26);
    checkConflict(10, 28);

    checkConflict(12, 30);
    checkConflict(14, 32);
    checkConflict(16, 34);

    checkConflict(18, 36);
    checkConflict(20, 38);
    checkConflict(22, 40);


    accAppendPair(48, 50);
    accAppendPair(54, 58);
    accWrite(46);

    accZero();

    accAppend(46);
    incWrite(255, 0);

    accZero();

    accAppendTimes(46, 73);
    accWrite(100);

    accZero();

    accInc(16);
    accAppendTimes(0, 30);
    accWrite(98);

    accZero();

    writeXO(24, 6, 66);
    writeXO(26, 8, 68);
    writeXO(28, 10, 70);

    writeXO(30, 12, 78);
    writeXO(32, 14, 80);
    writeXO(34, 16, 82);

    writeXO(36, 18, 90);
    writeXO(38, 20, 92);
    writeXO(40, 22, 94);

    accIncIndTimes(2, 128);
    accWrite(60);

    accZero();

    accAppendPair(48, 54);
    accAppend(50);
    accWrite(46);

    accZero();

    accAppendTimes(46, 255);
    accAppend(60);
    accWrite(60);

    accZero();

    accAppend(60);
    incWrite(49, 97);

    accZero();

    accAppendPair(48, 50);
    accAppend(58);
    accWrite(46);

    accZero();

    accAppendTimes(46, 55);
    accAppendTimes(54, 52);
    incWrite(32, 99);

    accZero();

    accAppendTimes(54, 248);
    incWrite(78, 101);

    accZero();

    printAt(66);

  }


/*




T *2
    s *4
    A *7
    n *8
    D *15
    C *16




C(D(G(36)))

A(G(36))

SS





        d[71] = 10;
        d[77] = 10;
        d[83] = 10;
        d[89] = 10;
        d[95] = 10;
        d[103] = 10;
        d[98] = 10;
        d[42] = 10;

        d[98] = 32;
        d[126] = 37;



        d[100] = 73;
        d[101] = 78;
        d[96]  = 80;
        d[102] = 83;
        d[99]  = 87;
        d[129] = 100;
        d[127] = 104;
        d[128] = 104;

        d[67] = 124;
        d[69] = 124;
        d[79] = 124;
        d[81] = 124;
        d[91] = 124;
        d[93] = 124;

        scanf_s();


    d[71], d[77], d[83], d[89], d[95], d[103], d[98], d[42] <- 10 ('\n')
    d[98]    <- 32 (' ')
    d[126]   <- 37 ('%')
    d[72:76] <- 45 ('-')
    d[84:88] <- 45 ('-')

    d[100] <- 73 ('I')
    d[101] <- 78 ('N')
    d[96] <- 80 ('P')
    d[102] <- 83 ('S')
    d[99] <- 87 ('W')
    d[129] <- 100 ('d')
    d[127:128] <- 104 ('h')
    d[67], d[69], d[79], d[81], d[91], d[93] <- 124 ('|')

    ZERO
    ZERO



    d[2] <- !d[2]
    d[54] <- !d[2]

    d[46] <- d[48] | d[50]

    d[54] <- !(d[46] & d[54])

    d[54] <- !d[54]  */
}