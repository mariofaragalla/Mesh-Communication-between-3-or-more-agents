uint32_t micro_sec = 0;
uint32_t time1 = 0;

extern uint32_t MicroSecClock = 0;

void setup() {
  Serial.begin(115200);
//  timer();
  TCCR2B = 0x00;
  TCNT2  = 253;
  TIFR2  = 0x00;        //Timer2 INT Flag Reg: Clear Timer Overflow Flag
  TIMSK2 = 0x01;        //Timer2 INT Reg: Timer2 Overflow Interrupt Enable
  TCCR2A = 0x00;        //Timer2 Control Reg A: Wave Gen Mode normal
  TCCR2B = 0x01;        // 16Mhz
}

void loop() {
//  Serial.println(micro_sec);
if(MicroSecClock >= 250000){
  Serial.println(MicroSecClock);
  MicroSecClock = 0;
}
  /*
  if(MicroSecClock >= 250000){
//    Serial.println(micro_sec);
//    Serial.println(TCNT4);
//    micro_sec = 0;
  }*/

//  Serial.println(MicroSecClock);
}

void timer(){
  cli();
  TCCR4A = 0;     // set entire TCCR5A register to 0
  TCCR4B = 0;     // same for TCCR5B
  TCNT4 = 0;

  OCR4A = 15;            // compare match register (16MHz/(1 prescaler * 1000000(interrupt frequency))- 1) // 1 microsec = 1000000 Hz // https://timer-interrupt-calculator.simsso.de/
   

  TCCR4A &= ~(1 << WGM40);   // CTC mode
  TCCR4A &= ~(1 << WGM41);   // CTC mode
  TCCR4B |= (1 << WGM42);   // CTC mode
  TCCR4B &= ~(1 << WGM43);   // CTC mode

  TCCR4B |= (1 << CS40);
  TCCR4B &= ~(1 << CS41);    // clkI/O /1 prescaler 
  TCCR4B &= ~(1 << CS42);
  
  TIMSK4 |= (1 << OCIE4A);
  
 
  
/*
  // using 8 prescaling (some errors: works with 4 microseconds instead of 1 microsecond)
  OCR5A = 1;            // compare match register (16MHz/(8 prescaler * 1000000(interrupt frequency))- 1) // 1 microsec = 1000000 Hz // https://timer-interrupt-calculator.simsso.de/

  TCCR5B |= (1 << WGM52);   // CTC mode

  TCCR5B &= ~(1 << CS50);
  TCCR5B |= (1 << CS51);    // clkI/O /8 prescaler 
  TCCR5B &= ~(1 << CS52);
  
  TIMSK5 |= (1 << OCIE5A);  // enable timer compare interrupt
*/
  sei();             // enable all interrupts
  micro_sec = 0;
}

 ISR(TIMER4_COMPA_vect){          // timer compare interrupt service routine
   micro_sec++;  // one increment = 1 microsec
}


ISR(TIMER2_OVF_vect) {// this is a built in function that gets called when the timer gets to the overflow counter number
//  static uint8_t count;            // interrupt counter
//
//  if( (++count & 0x01) == 0 )     // bump the interrupt counter
//    ++MicroSecClock;              // & count uSec every other time.
//
  TCNT2 = 253;                    // this tells the timer when to trigger the interrupt. when the counter gets to 253 out of 255(because the timer is 8 bit) the timmer will trigger an interrupt
  TIFR2 = 0x00;                   // clear timer overflow flag
  MicroSecClock++;
};
/* not working
unsigned long count() { // from: https://ucexperiment.wordpress.com/2012/03/17/examination-of-the-arduino-micros-function/
    unsigned long m;
    uint8_t oldSREG = SREG, t;
     
    cli();
    m = timer5_overflow_count;
#if defined(TCNT5)
    t = TCNT5;
#elif defined(TCNT5L)
    t = TCNT5L;
#else
    #error TIMER 5 not defined
#endif
 
   
#ifdef TIFR5
    if ((TIFR5 & _BV(TOV5)) && (t & 255))
        m++;
#else
    if ((TIFR & _BV(TOV5)) && (t & 255))
        m++;
#endif
 
    SREG = oldSREG;
     
    return ((m << 8) + t) * (64 / 16);
}
*/
