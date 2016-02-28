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
#define AUTO_REPEAT_RAMP_DELAY 1
#define AUTO_REPEAT_RAMP_STEP  50
#define USER_INPUT_TIMEOUT     8000

typedef struct
{
    time_t buttonPressTime;
    time_t mostRecentAutoRepeatTime;
    short autoRepeatCount;
    short autoRepeatInterval;
} SharedEventState;
