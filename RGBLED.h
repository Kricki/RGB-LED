#ifndef __RGBLED_H__
#define __RGBLED_H__

/*
Zur Kommunikation mit der RGB LED nutzen wir die "FastLED" Bibliothek (https://github.com/FastLED/FastLED)
Unsere LED nutzt den IC UCS2912 als Treiber. Dieser wird jedoch nicht direkt unterstützt.
Wählt man den "WS2812B" als Chipset aus, funktioniert die Ansteuerung allerdings ohne Probleme.
Zur Kommunikation wird nur ein einziger Digital-Pin verwendet (Pin 5).

Der IC kann insgesamt 4 RGB LEDs (also je 4x R, G und B) treiben. Um die Helligkeit
zu erhöhen sind die 4 Ausgänge auf die jeweilige zusammengeschaltet. Im Code müssen
entsprechend die 4 verschiedenen Ausgänge "zusammen" geschaltet werden.
*/

#include <Arduino.h>
#include <FastLED.h>

#define NUM_LEDS 4
#define DATA_PIN 5

// gamma-correction with gamma=2.2
const uint16_t gamma_correction_2_2[] PROGMEM = {
      0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
      0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
      0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    1,
      1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
      1,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    3,    3,
      3,    3,    3,    3,    3,    3,    3,    3,    4,    4,    4,    4,    4,    4,    4,
      4,    4,    5,    5,    5,    5,    5,    5,    5,    6,    6,    6,    6,    6,    6,
      6,    6,    7,    7,    7,    7,    7,    7,    8,    8,    8,    8,    8,    8,    9,
      9,    9,    9,    9,    9,   10,   10,   10,   10,   10,   10,   11,   11,   11,   11,
     11,   12,   12,   12,   12,   12,   13,   13,   13,   13,   13,   14,   14,   14,   14,
     14,   15,   15,   15,   15,   16,   16,   16,   16,   17,   17,   17,   17,   17,   18,
     18,   18,   18,   19,   19,   19,   19,   20,   20,   20,   21,   21,   21,   21,   22,
     22,   22,   22,   23,   23,   23,   24,   24,   24,   24,   25,   25,   25,   26,   26,
     26,   26,   27,   27,   27,   28,   28,   28,   29,   29,   29,   30,   30,   30,   31,
     31,   31,   32,   32,   32,   33,   33,   33,   34,   34,   34,   35,   35,   35,   36,
     36,   36,   37,   37,   38,   38,   38,   39,   39,   39,   40,   40,   40,   41,   41,
     42,   42,   42,   43,   43,   44,   44,   44,   45,   45,   46,   46,   46,   47,   47,
     48,   48,   48,   49,   49,   50,   50,   51,   51,   51,   52,   52,   53,   53,   54,
     54,   55,   55,   55,   56,   56,   57,   57,   58,   58,   59,   59,   60,   60,   61,
     61,   61,   62,   62,   63,   63,   64,   64,   65,   65,   66,   66,   67,   67,   68,
     68,   69,   69,   70,   70,   71,   71,   72,   72,   73,   73,   74,   75,   75,   76,
     76,   77,   77,   78,   78,   79,   79,   80,   80,   81,   82,   82,   83,   83,   84,
     84,   85,   86,   86,   87,   87,   88,   88,   89,   90,   90,   91,   91,   92,   93,
     93,   94,   94,   95,   96,   96,   97,   97,   98,   99,   99,  100,  100,  101,  102,
    102,  103,  104,  104,  105,  105,  106,  107,  107,  108,  109,  109,  110,  111,  111,
    112,  113,  113,  114,  115,  115,  116,  117,  117,  118,  119,  119,  120,  121,  121,
    122,  123,  123,  124,  125,  126,  126,  127,  128,  128,  129,  130,  131,  131,  132,
    133,  133,  134,  135,  136,  136,  137,  138,  139,  139,  140,  141,  142,  142,  143,
    144,  145,  145,  146,  147,  148,  148,  149,  150,  151,  151,  152,  153,  154,  155,
    155,  156,  157,  158,  159,  159,  160,  161,  162,  163,  163,  164,  165,  166,  167,
    167,  168,  169,  170,  171,  172,  172,  173,  174,  175,  176,  177,  177,  178,  179,
    180,  181,  182,  183,  183,  184,  185,  186,  187,  188,  189,  190,  190,  191,  192,
    193,  194,  195,  196,  197,  198,  198,  199,  200,  201,  202,  203,  204,  205,  206,
    207,  208,  208,  209,  210,  211,  212,  213,  214,  215,  216,  217,  218,  219,  220,
    221,  222,  223,  224,  225,  226,  226,  227,  228,  229,  230,  231,  232,  233,  234,
    235,  236,  237,  238,  239,  240,  241,  242,  243,  244,  245,  246,  247,  248,  249,
    250,  251,  252,  253,  254,  255,  257,  258,  259,  260,  261,  262,  263,  264,  265,
    266,  267,  268,  269,  270,  271,  272,  273,  274,  276,  277,  278,  279,  280,  281,
    282,  283,  284,  285,  286,  288,  289,  290,  291,  292,  293,  294,  295,  296,  298,
    299,  300,  301,  302,  303,  304,  305,  307,  308,  309,  310,  311,  312,  313,  315,
    316,  317,  318,  319,  320,  322,  323,  324,  325,  326,  328,  329,  330,  331,  332,
    333,  335,  336,  337,  338,  339,  341,  342,  343,  344,  346,  347,  348,  349,  350,
    352,  353,  354,  355,  357,  358,  359,  360,  362,  363,  364,  365,  367,  368,  369,
    370,  372,  373,  374,  375,  377,  378,  379,  381,  382,  383,  384,  386,  387,  388,
    390,  391,  392,  393,  395,  396,  397,  399,  400,  401,  403,  404,  405,  407,  408,
    409,  411,  412,  413,  415,  416,  417,  419,  420,  421,  423,  424,  426,  427,  428,
    430,  431,  432,  434,  435,  437,  438,  439,  441,  442,  443,  445,  446,  448,  449,
    450,  452,  453,  455,  456,  458,  459,  460,  462,  463,  465,  466,  468,  469,  470,
    472,  473,  475,  476,  478,  479,  481,  482,  483,  485,  486,  488,  489,  491,  492,
    494,  495,  497,  498,  500,  501,  503,  504,  506,  507,  509,  510,  512,  513,  515,
    516,  518,  519,  521,  522,  524,  525,  527,  528,  530,  532,  533,  535,  536,  538,
    539,  541,  542,  544,  545,  547,  549,  550,  552,  553,  555,  556,  558,  560,  561,
    563,  564,  566,  568,  569,  571,  572,  574,  576,  577,  579,  580,  582,  584,  585,
    587,  589,  590,  592,  593,  595,  597,  598,  600,  602,  603,  605,  607,  608,  610,
    612,  613,  615,  617,  618,  620,  622,  623,  625,  627,  628,  630,  632,  633,  635,
    637,  639,  640,  642,  644,  645,  647,  649,  650,  652,  654,  656,  657,  659,  661,
    663,  664,  666,  668,  670,  671,  673,  675,  677,  678,  680,  682,  684,  685,  687,
    689,  691,  692,  694,  696,  698,  700,  701,  703,  705,  707,  709,  710,  712,  714,
    716,  718,  719,  721,  723,  725,  727,  729,  730,  732,  734,  736,  738,  740,  741,
    743,  745,  747,  749,  751,  753,  754,  756,  758,  760,  762,  764,  766,  767,  769,
    771,  773,  775,  777,  779,  781,  783,  785,  786,  788,  790,  792,  794,  796,  798,
    800,  802,  804,  806,  808,  809,  811,  813,  815,  817,  819,  821,  823,  825,  827,
    829,  831,  833,  835,  837,  839,  841,  843,  845,  847,  849,  851,  853,  855,  857,
    859,  861,  863,  865,  867,  869,  871,  873,  875,  877,  879,  881,  883,  885,  887,
    889,  891,  893,  895,  897,  899,  901,  903,  905,  907,  910,  912,  914,  916,  918,
    920,  922,  924,  926,  928,  930,  932,  934,  937,  939,  941,  943,  945,  947,  949,
    951,  953,  956,  958,  960,  962,  964,  966,  968,  970,  973,  975,  977,  979,  981,
    983,  985,  988,  990,  992,  994,  996,  998, 1001, 1003, 1005, 1007, 1009, 1012, 1014,
    1016, 1018, 1020, 1023
};

struct RGB { uint16_t r; uint16_t g; uint16_t b; };
struct HSV { uint16_t h; uint16_t s; uint16_t v; };

struct SweepStatus {
  uint16_t hue_start;
  uint16_t hue_stop;
  uint16_t dh;  // hue increment per step
  uint16_t dt;    // update interval in ms
  uint32_t last_update_time;  // stores the time (millis()) of the last sweep update
  uint16_t hue; // current hue setting
  bool enabled; // Sweep enabled?
};

class RGBLED {
  public:
    RGBLED(uint8_t hue, uint16_t intensity);

    struct SweepStatus sweep_status;

    void set_gamma(bool enabled);
    void show();
    void setIntensity(uint16_t intensity);
    void setHue(uint8_t hue);
    void updateHSV();

    uint16_t gamma_correction(uint16_t value);
    void all_off();
    void setRGB(uint16_t r, uint16_t g, uint16_t b);

    void setHV(uint8_t hue, uint16_t val);
    struct RGB hv2rgb(uint16_t h, uint16_t v);
    void sweep_with_delay(uint16_t hue_start, uint16_t hue_stop, uint16_t dt);
    void sweep_intensity_with_delay(uint16_t intensity_start, uint16_t intensity_stop, uint16_t di, uint16_t dt);

    void update_sweep_status(uint16_t hue_start, uint16_t hue_stop, uint16_t dh, uint16_t dt);
    void enableSweep(bool enabled);
    void resetSweep();
    uint8_t updateSweep(uint8_t hue_offset=0);
    float progress();

  private:
    CRGB _leds[NUM_LEDS];
    bool _gamma = true;

    uint16_t _intensity; // "value" of HSV [0, 1023]
    uint8_t _hue;

    //struct RGB _rgb = {0, 0, 0};  // stores the current RGB values
};

#endif
