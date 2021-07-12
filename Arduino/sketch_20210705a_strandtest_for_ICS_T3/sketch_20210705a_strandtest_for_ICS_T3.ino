// sketch_20210705a_strandtest_for_ICS_T3

// First version Fri Jul  9 05:12:25 JST 2021 by @hohno_at_kuimc
// Prev update: Sat Jul 10 09:08:13 JST 2021
// Last update: Sun Jul 11 14:22:58 JST 2021

// Note: The tentative sketch is here:
// Arduino/workspace/CommonWorkArea/hohno-MBP-2016B/sketch_20210705a_strandtest_for_ICS_T3

#define SKETCH_INFO "hohno-MBP-2016B/sketch_20210705a_strandtest_for_ICS_T3"
#define SKETCH_DATE "2021-07-12 09:47:58 JST"
#define SKETCH_MEMO "(no memo now)"

// ---------------------------------------------------------

#define UNIXTIME_SEC	(1625873723 + 15)
#define TIMEZONE_SEC	(32400L)

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define PIN 6

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(60, PIN, NEO_GRB + NEO_KHZ800);

// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.


// ---------------------------------------------------------

void colorWipe(uint32_t, uint8_t);
void sub(int,  uint32_t);

#define MINIBUFFSIZE (60)


byte buff[MINIBUFFSIZE];

#define RED1	(63)
#define GRN1	(63)
#define BLU1	(63)

// ---------------------------------------------------------

void setup() {

	// This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
#if defined (__AVR_ATtiny85__)
	if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
#endif
  // End of trinket special code

	Serial.begin(57600);
	Serial.setTimeout(100);
	Serial.println();
	Serial.print("# "); Serial.println(SKETCH_INFO);
	Serial.print("# "); Serial.println(SKETCH_DATE);
	Serial.print("# "); Serial.println(SKETCH_MEMO);
	Serial.println();

  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
	colorWipe(strip.Color( RED1, GRN1, BLU1), 25);
	colorWipe(strip.Color(    0,    0,    0), 25);
}

// ---------------------------------------------------------

boolean updateOffset(int32_t *tp) {
	boolean flag;
	String myStr = Serial.readStringUntil('\n');
	Serial.print("[");		Serial.print(myStr);		Serial.println("]");
	int32_t offset = *tp;

flag = false;

	if (myStr.compareTo("F") == 0) {
		Serial.println("(F)");
		offset += 100;	// 10sec
		flag = true;
	} else if (myStr.compareTo("B") == 0) {
		Serial.println("(B)");
		offset -= 100;	// 10sec
		flag = true;
	} else if (myStr.compareTo("f") == 0) {
		Serial.println("(f)");
		offset += 5;	// 0.5sec
		flag = true;
	} else if (myStr.compareTo("b") == 0) {
		Serial.println("(b)");
		offset -= 5;	// 0.5sec
		flag = true;
	} else {
		myStr.getBytes(buff, MINIBUFFSIZE);
		// Serial.print("{");		Serial.print((char *)buff);		Serial.println("}");
		uint32_t xVal = 0;
		if (sscanf((char *)buff, "%lu", &xVal) > 0) {
			if (xVal > 0) {
				Serial.print("("); Serial.print(xVal); Serial.print(") -> ");
				int th = (xVal / 10000) % 100;
				int tm = (xVal % 10000) / 100;
				int ts = (xVal % 100);
				Serial.print("("); Serial.print(th); Serial.print(")");
				Serial.print("("); Serial.print(tm); Serial.print(")");
				Serial.print("("); Serial.print(ts); Serial.println(")");
				offset = (th * 3600L + tm * 60L + ts) * 10;	// unit: 100msec
				offset -= (millis() / 100);	 // unit: 100msec
				flag = true;
			}
		}
	}

	if (flag) {
		*tp = offset;
	}

	return(flag);
}

// ---------------------------------------------------------

struct tinfo {
	int HH;
	int M1;
	int M2;
	int SS;
};

// ---------------------------------------------------------

void updateHMS(struct tinfo *pp, struct tinfo *cp) {

		Serial.print(">> ");
		Serial.print(pp->HH); Serial.print(" "); Serial.print(pp->M1); Serial.print("/"); Serial.print(pp->M2); Serial.print(" "); Serial.print(pp->SS);
		Serial.print(" -> ");
		Serial.print(cp->HH); Serial.print(" "); Serial.print(cp->M1); Serial.print("/"); Serial.print(cp->M2); Serial.print(" "); Serial.println(cp->SS);

		sub(pp->HH, strip.Color(  0,   0,   0));
		sub(pp->M1, strip.Color(  0,   0,   0));
		if (pp->M2 >= 0) { sub(pp->M2, strip.Color(  0,   0,   0)); }
		sub(pp->SS, strip.Color(  0,   0,   0));

		if (cp->HH == cp->M1) { // 分針と時針が重なった場合
			sub(cp->HH, strip.Color(   0, GRN1,   0));
			if (cp->SS >= 30) {
				cp->M2 = cp->M1 + 1;
				sub(cp->M2, strip.Color(   0, GRN1,    0));
			} else {
				cp->M2 = -1;
			}
		} else {
			sub(cp->M1, strip.Color(RED1,    0, BLU1));
			if (cp->SS >= 30) {
				cp->M2 = cp->M1 + 1;
				sub(cp->M2, strip.Color(RED1,    0, BLU1));
			} else {
				cp->M2 = -1;
			}
			sub(cp->HH, strip.Color(   0, GRN1, BLU1));
		}

		if ((cp->SS == cp->HH) || (cp->SS == cp->M1) || (cp->SS == cp->M2))  { // 秒針が分針または時針と重なった場合
			sub(cp->SS, strip.Color(RED1,    0,    0));
		} else {
			sub(cp->SS, strip.Color(RED1, GRN1, BLU1));
		}
}

// ---------------------------------------------------------


void loop() {

	static boolean flag = false;
	static int32_t  tOffset = (((UNIXTIME_SEC + TIMEZONE_SEC) % 43200L) + 10) * 10; // unit: 100msec
	// The time in 100msec from the epoch of this Arduino. Therefore current time is "millis()/100 + tOffset) in 100msec
	// static int pHH = -1, pM1 = -1, pM2 = -1, pSS = -1;
	// static int cHH = 0, cM1 = 0, cM2 = 0, cSS = 0;
	static struct tinfo  p = {-1, -1, -1, -1}, c = {0, 0, 0, 0};
	uint32_t tVal;

	if (Serial.available() > 0) {
		flag = updateOffset(&tOffset);
	}

	tVal = millis() / 100;  // unit: 100msec
	tVal += tOffset;			  // unit: 100msec

	c.SS = (tVal / 10) % 60;
	c.M1 = ((tVal / 10) % 3600) / 60;
	c.HH = ((((tVal / 10) / 3600) % 12) * 5) + (c.M1 / 12);

	if ((p.HH != c.HH) || (p.M1 != c.M1) || ((p.M2 >= 0) && (p.M2 != c.M2)) || (p.SS != c.SS) || flag) {

		flag = false;

		updateHMS(&p, &c);

    strip.show();
		p.HH = c.HH; p.M1 = c.M1; p.M2 = c.M2; p.SS = c.SS;
	}
}

//colorWipe(strip.Color(0, 0, 0, 255), 50); // White RGBW
  // Send a theater pixel chase in...
  // theaterChase(strip.Color(127, 127, 127), 50); // White
  // theaterChase(strip.Color(127, 0, 0), 50); // Red
  // theaterChase(strip.Color(0, 0, 127), 50); // Blue

  // rainbow(20);
  // rainbowCycle(20);
  // theaterChaseRainbow(50);

// ---------------------------------------------------------

void sub(int p,  uint32_t c) {
	int k = ((p < 0) || (p >= 60)) ? 0 : p;
	int Npixels = strip.numPixels();
	const int Noffset = 30;
	k = (k + Noffset) % Npixels;
	strip.setPixelColor(k, c);
}

// ---------------------------------------------------------

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
	int Npixels = strip.numPixels();
	int Noffset = 30;
  for(int i = 0; i < Npixels; i++) {
    int j = (i==0) ? (Npixels-1) : (i-1);
		int k = i;
		j = (j + Noffset) % Npixels;
		k = (k + Noffset) % Npixels;
    // strip.setPixelColor(j,strip.Color(0,0,0));
    strip.setPixelColor(k, c);
    strip.show();
    delay(wait);
  }
}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, c);    //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
