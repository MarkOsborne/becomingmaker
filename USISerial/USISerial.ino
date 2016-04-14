/*  
 Author: Mark Osborne, BecomingMaker.com
 See accompanying post http://becomingmaker.com/usi-serial-uart-attiny85/ 
 
 An example of USI Serial for ATtiny25/45/85.
 A byte of serial input sets the PWM duty cycle of PB4.
   
  ATTiny85 Hookup

  RESET -|1 v 8|- Vcc
    PB3 -|2   7|- PB2/SCK
    PB4 -|3   6|- PB1/MISO
    GND -|4 _ 5|- PB0/MOSI/SDA

ATTiny85 PB0/MOSI/SDA -> Serial UART Rx, connect to Tx of serial input device
ATTiny85 PB4 -> PWM output, connect to LED
*/

/* Supported combinations:
 *   F_CPU 1000000   BAUDRATE 1200, 2400 
 *   F_CPU 8000000   BAUDRATE 9600, 19200
 *   F_CPU 16000000  BAUDRATE 9600, 19200, 28800, 38400
 */

// Set your baud rate and here
#define BAUDRATE            9600
// F_CPU defined by Arduino, e.g. 1000000, 8000000, 16000000

// If bit width in cpu cycles is greater than 255 then  divide by 8 to fit in timer
// Calculate prescaler setting
#define CYCLES_PER_BIT       ( F_CPU / BAUDRATE )
#if (CYCLES_PER_BIT > 255)
#define DIVISOR             8
#define CLOCKSELECT         2
#else
#define DIVISOR             1
#define CLOCKSELECT         1
#endif
#define FULL_BIT_TICKS      ( CYCLES_PER_BIT / DIVISOR )
#define HALF_BIT_TICKS      ( FULL_BIT_TICKS / 2)

// Number of code CPU cycles from from pin change to starting USI timer
#define START_DELAY         ( 99 )

// Number of CPU cycles delay after setting COMPA timer until global interrupt is enabled
#define COMPA_DELAY         42
#define TIMER_MIN           ( COMPA_DELAY / DIVISOR )
  
#define TIMER_START_DELAY   ( START_DELAY  / DIVISOR )
#if (HALF_BIT_TICKS - TIMER_START_DELAY)>0
#define TIMER_TICKS         ( HALF_BIT_TICKS - TIMER_START_DELAY )
#if (TIMER_TICKS < TIMER_MIN)
#warning TIMER_TICKS too low, USI bit sample will after center of bit
#endif
#else
#error "TIMER_TICKS invalid, choose different values for F_CPU, BAUDRATE and START_DELAY"
#define TIMER_TICKS         1
#endif

// Old timer values
volatile byte oldTCCR0A;
volatile byte oldTCCR0B;
volatile byte oldTCNT0;

// Serial
volatile bool serialDataReady = false;
volatile uint8_t serialInput;

// Call from main loop to read from serial
// returns true if data was read
// data placed in variable at pData address
bool readSerialData(uint8_t* pData)
{
    if (serialDataReady)
    {
        *pData = serialInput;
        serialDataReady = false;
        return true;
    }
    return false;
}

// Initialize USI for UART reception.
void initializeUSI () {
    oldTCCR0B = TCCR0B;
    oldTCCR0A = TCCR0A;
    DDRB &= ~(1 << DDB0);           // Set pin 0 to input
    PORTB |= 1 << PB0;              // Enable internal pull-up on pin PB0
    USICR = 0;                      // Disable USI.
    GIFR = 1<<PCIF;                 // Clear pin change interrupt flag.
    GIMSK |= 1<<PCIE;               // Enable pin change interrupts
    PCMSK |= 1<<PCINT0;             // Enable pin change on pin PB0
}

// Will fire for all enabled pin change interrupt pins
ISR (PCINT0_vect) {
  uint8_t pinbVal = PINB;           // Read directly as Arduino digitalRead is too slow
  if (!(pinbVal & 1<<PINB0))        // Trigger only if DI is Low
  {
    onSerialPinChange();
  }
}

void onSerialPinChange()
{
    oldTCNT0 = TCNT0;               // Save old timer counter
    GIMSK &= ~(1<<PCIE);            // Disable pin change interrupts
    TCCR0A = 2<<WGM00;              // CTC mode
    TCCR0B = CLOCKSELECT;           // Set prescaler to clk or clk /8
    GTCCR |= 1 << PSR0;             // Reset prescaler
    OCR0A = TIMER_TICKS;            // Delay to the middle of start bit accounting for interrupt startup and code execution delay before timer start
    TCNT0 = 0;                      // Count up from 0
    TIFR = 1 << OCF0A;              // Clear output compare interrupt flag
    TIMSK |= 1<<OCIE0A;             // Enable output compare interrupt
}

ISR (TIMER0_COMPA_vect) {
    // COMPA interrupt indicates middle of bit 0
    TIMSK &= ~(1<<OCIE0A);          // Disable COMPA interrupt
    TCNT0 = 0;                      // Count up from 0
    OCR0A = FULL_BIT_TICKS;         // Shift every bit width
    // Enable USI OVF interrupt, and select Timer0 compare match as USI Clock source:
    USICR = 1<<USIOIE | 0<<USIWM0 | 1<<USICS0;
    // Clear Start condition interrupt flag, USI OVF flag, and set counter
    USISR = 1<<USIOIF | /*1<<USISIF |*/ 8;
}

void serialReceived(uint8_t data)
{
    serialDataReady = true;
    serialInput = data;
}

// Reverse USI byte
uint8_t ReverseByte (uint8_t x) {
    x = ((x >> 1) & 0x55) | ((x << 1) & 0xaa);
    x = ((x >> 2) & 0x33) | ((x << 2) & 0xcc);
    x = ((x >> 4) & 0x0f) | ((x << 4) & 0xf0);
    return x;
}

// USI overflow interrupt indicates we've received a byte
ISR (USI_OVF_vect) {
    uint8_t temp = USIBR;
    USICR  =  0;                    // Disable USI

    //Restore old timer values
    TCCR0A = oldTCCR0A;
    TCCR0B = oldTCCR0B;
    // Note Arduino millis() and micros() will loose the time it took us to receive a byte
    // Approximately 1ms at 9600 baud
    TCNT0 = oldTCNT0;
    
    serialReceived(ReverseByte(temp));

    GIFR = 1<<PCIF;                 // Clear pin change interrupt flag.
    GIMSK |= 1<<PCIE;               // Enable pin change interrupts again
    // We are still in the middle of bit 7 and if it is low we will get a pin change event
    // for the stop bit, but we will ignore it because it is high
}

const uint8_t ledPin = 4;

void setup() {
  // Tweak clock speed for 5V, comment out if running ATtiny at 3V
  OSCCAL += 3;
  
  initializeUSI();
  
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin,0);
}

void loop() {
  uint8_t serialInput;
  if (readSerialData(&serialInput))
  {
    analogWrite(ledPin, serialInput);
  }
}
