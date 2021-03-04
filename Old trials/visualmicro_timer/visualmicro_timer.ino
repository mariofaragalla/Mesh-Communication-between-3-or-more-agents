int compareMatchReg;
volatile int interrupts;

void setup()
{
  Serial.begin(115200);
  // initialize timer1 
  noInterrupts();           // disable all interrupts
  TCCR1A = 0;
  TCCR1B = 0;

  // Set compareMatchReg to the correct value for our interrupt interval
  // compareMatchReg = [16, 000, 000Hz / (prescaler * desired interrupt frequency)] - 1
  
  /* E.g. 1Hz with 1024 Pre-Scaler:
    compareMatchReg = [16, 000, 000 / (prescaler * 1)] - 1
    compareMatchReg = [16, 000, 000 / (1024 * 1)] - 1 = 15624
  
      As this is > 256 Timer 1 Must be used for this..
  */
  compareMatchReg = 15;   // preload timer from calc above
  TCNT1 = compareMatchReg;   // preload timer
  

  /*
  Prescaler:
    (timer speed(Hz)) = (Arduino clock speed(16MHz)) / prescaler
      So 1Hz = 16000000 / 1 --> Prescaler: 1
      Prescaler can equal the below values and needs the relevant bits setting
      1    [CS10]
      8    [CS11]
      64   [CS11 + CS10]
      256  [CS12]
      1024 [CS12 + CS10]
  */
  TCCR1B |= (1 << CS10);    // 1 prescaler 

  TIMSK1 |= (1 << TOIE1);   // enable timer overflow interrupt
  interrupts();             // enable all interrupts
}

ISR(TIMER1_OVF_vect)        // interrupt service routine 
{
  TCNT1 = compareMatchReg;   // preload timer
  interrupts++;
  Serial.print("Total Ticks:");
  Serial.println(interrupts);
}

void loop()
{
  // your program here...
}
