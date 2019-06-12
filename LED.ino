#include <FastLED.h>

#define NUM_LEDS 200
#define LED_PIN 3
#define SOUND_PIN 1
#define brightness 255

const int sampleWindow = 10; // Sample window width in mS (50 mS = 20Hz)
unsigned int sample;

float sensorvalue = 0, lastmaxsensorvalue = 0, lastminsensorvalue = 1024.0;
int curshow = NUM_LEDS;
float val;

int findLEDNum(int len, float soundMin, float soundMax, float sound);

int loopCount = 0;

float fade_scale = 1.3;

float fscale(float originalMin, float originalMax, float newBegin, float newEnd, float inputValue, float curve);
void FillLEDsFromPaletteColors(uint8_t colorIndex, int curshow, int flag);
void RunBeats(int x);
int leftOver = 0;
int seperator = 0;
void RunBeats2(int x);
int curshowArray[20] = {0};

void insertCurshow(int x)
{
    int i;
    for (i = 0; i < 19; i++)
    {
        curshowArray[i + 1] = curshowArray[i];
    }
    curshowArray[0] = x;
}
int averageCurshow()
{
    int i, sum = 0;
    for (i = 0; i < 20; i++)
    {
        sum += curshowArray[i];
    }
    int res = sum / 20;
    return res;
}

CRGBPalette16 currentPalette;
TBlendType currentBlending;

CRGB currentColor;
CRGB leds[NUM_LEDS];

void setup()
{
    Serial.begin(9600);
    // put your setup code here, to run once:
    FastLED.addLeds<WS2812B, LED_PIN, BRG>(leds, NUM_LEDS);
    FastLED.setBrightness(brightness);
    for (int i = 0; i < NUM_LEDS; i++)
        leds[i] = CRGB(0, 0, 255);
    currentPalette = PartyColors_p;
    currentBlending = LINEARBLEND;
    FastLED.show();
    delay(1000);
}

void loop()
{
    unsigned long startMillis = millis(); // Start of sample window
    unsigned int peakToPeak = 0;          // peak-to-peak level

    unsigned int signalMax = 0;
    unsigned int signalMin = 1024;

    // collect data for 50 mS
    while (millis() - startMillis < sampleWindow)
    {
        sample = analogRead(1);
        if (sample < 1024) // toss out spurious readings
        {
            if (sample > signalMax)
            {
                signalMax = sample; // save just the max levels
            }
            else if (sample < signalMin)
            {
                signalMin = sample; // save just the min levels
            }
        }
    }
    peakToPeak = signalMax - signalMin; // max - min = peak-peak amplitude

    static uint8_t startIndex = 0;
    sensorvalue = peakToPeak;
    curshow = fscale(15, 645, 0, NUM_LEDS, sensorvalue, 2);
    startIndex = startIndex + 1; /* motion speed */
                                 //  FillLEDsFromPaletteCoitlors( startIndex,curshow);
                                 //  curshow = findLEDNum(NUM_LEDS,lastminsensorvalue,lastmaxsensorvalue,sensorvalue);
                                 //  curshow = abs(curshow);
                                 //  Serial.print(sensorvalue);
                                 //  Serial.print(" ");
    Serial.println(curshow);
    //  FillLEDsFromPaletteColors( startIndex,curshow,0);
    //  FillLEDsFromPaletteColors( startIndex,curshow,1);
    //  FillLEDsFromPaletteColors( startIndex,curshow,2);
    FillLEDsFromPaletteColors(startIndex, curshow, 4);
    //  RunBeats(peakToPeak);
    //  RunBeats2(peakToPeak);
    FastLED.show();
    loopCount++;
    //  delay(50);
}

void FillLEDsFromPaletteColors(uint8_t colorIndex, int curshow, int flag)
{
    if (flag == 0)
    {
        //    uint8_t brightness = 50;

        for (int i = 0; i < NUM_LEDS; i++)
        {
            if (i <= curshow)
            {
                leds[i] = ColorFromPalette(currentPalette, colorIndex, brightness, currentBlending);
                colorIndex += 1;
            }
            else
            {
                leds[i] = CRGB(leds[i].r / fade_scale, leds[i].g / fade_scale, leds[i].b / fade_scale);
            }
        }
    }
    else if (flag == 1)
    {
        //    uint8_t brightness = 255;

        for (int i = NUM_LEDS / 2 - 1, j = NUM_LEDS / 2; i >= 0 && j < NUM_LEDS; i--, j++)
        {

            if (NUM_LEDS / 2 - i <= curshow / 2)
            {
                leds[i] = ColorFromPalette(currentPalette, colorIndex, brightness, currentBlending);
                leds[j] = ColorFromPalette(currentPalette, colorIndex, brightness, currentBlending);
                colorIndex += 1;
            }
            else
            {
                leds[i] = CRGB(leds[i].r / fade_scale, leds[i].g / fade_scale, leds[i].b / fade_scale);
                leds[j] = CRGB(leds[j].r / fade_scale, leds[j].g / fade_scale, leds[j].b / fade_scale);
            }
        }
    }
    else if (flag == 2)
    {
        for (int i = NUM_LEDS / 2 - 1, j = NUM_LEDS / 2; i >= 0 && j < NUM_LEDS; i--, j++)
        {

            if (NUM_LEDS / 2 - i <= curshow / 2)
            {
                leds[i] = ColorFromPalette(currentPalette, colorIndex, brightness, currentBlending);
                leds[j] = ColorFromPalette(currentPalette, colorIndex, brightness, currentBlending);
                colorIndex += 1;
            }
            else if (NUM_LEDS / 2 - i <= curshow / 2 + NUM_LEDS / 4)
            {
                leds[i] = CRGB(0, 200, 100);
                leds[j] = CRGB(0, 200, 100);
            }
            else
            {
                leds[i] = CRGB(200, 200, 0);
                leds[j] = CRGB(200, 200, 0);
            }
        }
    }
    else if (flag == 3)
    {
        insertCurshow(curshow);
        int average = averageCurshow();
        Serial.print(" ");
        Serial.println(average);
        for (int i = NUM_LEDS / 2 - 1, j = NUM_LEDS / 2; i >= 0 && j < NUM_LEDS; i--, j++)
        {

            if (NUM_LEDS / 2 - i <= average / 2)
            {
                leds[i] = ColorFromPalette(currentPalette, colorIndex, brightness, currentBlending);
                leds[j] = ColorFromPalette(currentPalette, colorIndex, brightness, currentBlending);
                colorIndex += 1;
            }
            else
            {
                leds[i] = CRGB(leds[i].r / fade_scale, leds[i].g / fade_scale, leds[i].b / fade_scale);
                leds[j] = CRGB(leds[j].r / fade_scale, leds[j].g / fade_scale, leds[j].b / fade_scale);
            }
        }
    }
    else if (flag == 4)
    {
        insertCurshow(curshow);
        int average = averageCurshow();
        for (int i = NUM_LEDS / 2 - 1, j = NUM_LEDS / 2; i >= 0 && j < NUM_LEDS; i--, j++)
        {

            if (NUM_LEDS / 2 - i <= average / 2)
            {
                leds[i] = ColorFromPalette(currentPalette, colorIndex, brightness, currentBlending);
                leds[j] = ColorFromPalette(currentPalette, colorIndex, brightness, currentBlending);
                colorIndex += 1;
            }
            else if (NUM_LEDS / 2 - i <= average / 2 + NUM_LEDS / 4)
            {
                leds[i] = CRGB(0, 200, 100);
                leds[j] = CRGB(0, 200, 100);
            }
            else
            {
                leds[i] = CRGB(200, 200, 0);
                leds[j] = CRGB(200, 200, 0);
            }
        }
    }
}

void RunBeats(int x)
{

    if (x > 15)
    {
        float mappedSound = map(15, 645, 0, 15, x);
        leds[0] = ColorFromPalette(currentPalette, (int)mappedSound, brightness, currentBlending);
    }
    else
    {
        leds[0] = CRGB(0, 0, 0);
    }

    for (int z = NUM_LEDS; z > 0; z--)
    {
        leds[z] = leds[z - 1];
    }
}

void RunBeats2(int x)
{
    //  uint8_t brightness = 255;
    //  int x = analogRead(SOUND_PIN);
    if (x < 50)
    {
        leds[0] = CRGB(0, 0, 0);
    }
    else if (x > 100 && x <= 200)
    {
        leds[0] = CRGB(255, 154, 0);
    }
    else if (x > 300 && x <= 350)
    {
        leds[0] = CRGB(255, 255, 0);
    }
    else if (x > 350 && x <= 400)
    {
        leds[0] = CRGB(0, 255, 0);
    }
    else if (x > 450 && x <= 500)
    {
        leds[0] = CRGB(0, 0, 255);
    }
    else if (x > 550 && x <= 600)
    {
        leds[0] = CRGB(150, 102, 255);
    }
    else
    {
        leds[0] = CRGB(255, 0, 255);
    }
    for (int z = NUM_LEDS; z > 0; z--)
    {
        leds[z] = leds[z - 1];
    }
}

float fscale(float originalMin, float originalMax, float newBegin, float newEnd, float inputValue, float curve)
{

    float OriginalRange = 0;
    float NewRange = 0;
    float zeroRefCurVal = 0;
    float normalizedCurVal = 0;
    float rangedValue = 0;
    boolean invFlag = 0;

    // condition curve parameter
    // limit range

    if (curve > 10)
        curve = 10;
    if (curve < -10)
        curve = -10;

    curve = (curve * -.1);  // - invert and scale - this seems more intuitive - postive numbers give more weight to high end on output
    curve = pow(10, curve); // convert linear scale into lograthimic exponent for other pow function

    /*
   Serial.println(curve * 100, DEC);   // multply by 100 to preserve resolution  
   Serial.println(); 
   */

    // Check for out of range inputValues
    if (inputValue < originalMin)
    {
        inputValue = originalMin;
    }
    if (inputValue > originalMax)
    {
        inputValue = originalMax;
    }

    // Zero Refference the values
    OriginalRange = originalMax - originalMin;

    if (newEnd > newBegin)
    {
        NewRange = newEnd - newBegin;
    }
    else
    {
        NewRange = newBegin - newEnd;
        invFlag = 1;
    }

    zeroRefCurVal = inputValue - originalMin;
    normalizedCurVal = zeroRefCurVal / OriginalRange; // normalize to 0 - 1 float

    // Check for originalMin > originalMax  - the math for all other cases i.e. negative numbers seems to work out fine
    if (originalMin > originalMax)
    {
        return 0;
    }

    if (invFlag == 0)
    {
        rangedValue = (pow(normalizedCurVal, curve) * NewRange) + newBegin;
    }
    else // invert the ranges
    {
        rangedValue = newBegin - (pow(normalizedCurVal, curve) * NewRange);
    }

    return rangedValue;
}