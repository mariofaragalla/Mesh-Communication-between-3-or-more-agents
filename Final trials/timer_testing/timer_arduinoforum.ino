

uint16_t divider = 0;
unsigned long time1 = 0;
unsigned long time2 = 0;
bool one_sec = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  StartFastTimer();
}

void loop() {
  // put your main code here, to run repeatedly:
    
//    divider = FastTimer()/16000000;
    time1 = FastTimer();
    Serial.println("hello world");
    Serial.println("hello world");
    Serial.println("hello world");
    Serial.println("hello world");
    Serial.println("hello world");
    Serial.println("hello world");
    Serial.println("hello world");
    time2= FastTimer() - time1;
    Serial.println(time2);
    
    
}

void StartFastTimer() {
  noInterrupts ();  // protected code
  // reset Timer 1
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;
  TIMSK1 = 0;

  TIFR1 = _BV(TOV1); // clear Overflow Flag so we don't get a bogus interrupt
  TIMSK1 = _BV(TOIE1); // Enable Timer 1 Overflow Interrupt
  TCCR1B = _BV(CS10); // start Timer 1, no prescaler

  interrupts ();
}


volatile uint16_t Overflows = 0;


ISR(TIMER1_OVF_vect){ // interrupts each 0.0625 us seconds
  Overflows++;
}


unsigned long FastTimer() 
{
  unsigned long currentTime;
  uint16_t overflows;
  
  noInterrupts();
  overflows = Overflows;  // Make a local copy


  // If an overflow happened but has not been handled yet
  // and the timer count was close to zero, count the
  // overflow as part of this time.
  if ((TIFR1 & _BV(TOV1)) && (ICR1 < 1024))
    overflows++;

  currentTime = overflows; // Upper 16 bits
  currentTime = (currentTime << 16) | ICR1;
  interrupts();
  
  return currentTime;
}
