// This is the set of things we keep track of in eeprom.  This is mostly 
// so we can record peruser info, and only have to compute the various
// holidays once a year.
typedef struct {
  unsigned char version;  // incremented if the info struct changes, also used to check
                          // never having been written state.
  
  unsigned short year;    // Most recent handled year, year all the holidays are computed for.
  unsigned char month;    // Most recently handled month.
  unsigned char day;      // Most recently handled day.

  int gmtOffsetInSeconds; 
} EEPROMInfo;

// This is used to keep track of individual button state during button input/auto repeat, etc.

#define BUTTON_PRESSED     0
#define BUTTON_RELEASED    1
#define BUTTON_AUTO_REPEAT 2
#define BUTTON_NO_EVENT    3

#define BUTTON_STATE_MOST_RECENT_STATE   (1 << 0)
#define BUTTON_STATE_OWNS_START_TIME     (1 << 1)
#define BUTTON_STATE_CAN_AUTO_REPEAT     (1 << 2)
#define BUTTON_STATE_OFFICALLY_PRESSED   (1 << 3)

typedef struct
{
    uint8_t state;
    uint8_t pin;
} ButtonInfo;

// Because we do some tricky things with color/indicating AM PM, forcing leading
// zeros, etc. It is useful for us to have a digit type during display.
#define DIGIT_TYPE_HOURS      0
#define DIGIT_TYPE_MINUTES    1
#define DIGIT_TYPE_GMT_OFFSET 2
#define DIGIT_TYPE_YEAR       3
#define DIGIT_TYPE_MONTH      4
#define DIGIT_TYPE_DAY        5

#define BUTTON_DEBOUNCE_INTERVAL 50
#define MAX_AUTO_REPEAT_INTERVAL 350
#define MIN_AUTO_REPEAT_INTERVAL 200
#define AUTO_REPEAT_RAMP_DELAY 2
#define AUTO_REPEAT_RAMP_STEP  50
#define USER_INPUT_TIMEOUT     10000

typedef struct
{
    time_t buttonPressTime;
    time_t mostRecentAutoRepeatTime;
    short autoRepeatCount;
    short autoRepeatInterval;
} SharedEventState;

#define SPECIAL_CHAR_TYPE_SPRING_EQUINOX    0
#define SPECIAL_CHAR_TYPE_SUMMER_SOLSTICE   1
#define SPECIAL_CHAR_TYPE_FALL_EQUINOX      2
#define SPECIAL_CHAR_TYPE_WINTER_SOLSTICE   3
#define SPECIAL_CHAR_TYPE_CURRENT_MOON      4
#define SPECIAL_CHAR_TYPE_GOLDEN_RING       5
#define SPECIAL_CHAR_TYPE_SPINNING_GLOBE    6
#define SPECIAL_CHAR_TYPE_MONSTER_EYE       7
#define SPECIAL_CHAR_TYPE_SAURON_EYE        8
#define SPECIAL_CHAR_TYPE_BEATING_HEART     9 // No Implemented yet.
#define SPECIAL_CHAR_TYPE_SPINNING_ORNAMENT 10

#define DAY_TYPE_NORMAL_DAY             0
#define DAY_TYPE_BIRTHDAY               1
#define DAY_TYPE_FLAG_DAY               2
#define DAY_TYPE_MIDNIGHT_COUNTDOWN_DAY 3
#define DAY_TYPE_VALENTINES_DAY         4
#define DAY_TYPE_COLOR_HOLIDAY          5 // These are defined by a color + special char.
#define DAY_TYPE_EASTER                 6
#define DAY_TYPE_TOLKIENS_BIRTHDAY      7
#define DAY_TYPE_HALLOWEEN              8

#define FIREWORKS_STATE_WHITE_FLASH 0
#define FIREWORKS_STATE_SOLID_COLOR_FADE 1
#define FIREWORKS_STATE_BETWEEN_FLASHES 2

#define COLOR_DEFAULT_DIGITS 0xff0000
#define COLOR_GOLD 0xfe7000
#define COLOR_COPPER 0xff3000
#define COLOR_PINK 0xfe0032
#define COLOR_GREEN 0x00ff00
#define COLOR_BLUE  0x0000ff
#define COLOR_RED 0xff0000
#define COLOR_YELLOW 0xfea100
#define COLOR_TURQUOISE 0x00ffff
#define COLOR_ORANGE 0xfe1a00
#define COLOR_PURPLE 0xff00ff
#define COLOR_MOON_BRIGHT 0xfff8d1
#define COLOR_MOON_DIM 0x0f120d
#define COLOR_EYE_GREEN 0x00c800
#define COLOR_WHITE 0xffffff
#define COLOR_ALMOST_WHITE 0xfffffe


#define MIN_DIMMING_SCALE 64
