volatile uint32_t micro_sec = 0;
uint32_t micro_sec_cpy = 0;
uint32_t time1 = 0;

void setup() {
  Serial.begin(115200);
  timer();
}

void loop() {
  noInterrupts();
  micro_sec_cpy = micro_sec; 
  if(micro_sec >= 1000000){ 
    micro_sec = 0;
  }
  sei();
  
  if(micro_sec_cpy >= 1000000){   //now it is working with 1 second period -> 1/250000 = 4us but this num should be 1000000 for 1us
    Serial.println(micro_sec_cpy);
  }
}

void timer(){
  cli();
  TCCR5A = 0;     // set entire TCCR5A register to 0
  TCCR5B = 0;     // same for TCCR5B
  TCNT5 = 0;

  OCR5A = 15;            // compare match register (16MHz/(1 prescaler * 1000000(interrupt frequency))- 1) // 1 microsec = 1000000 Hz // https://timer-interrupt-calculator.simsso.de/

  TCCR5B |= (1 << WGM52);   // CTC mode

  TCCR5B |= (1 << CS50);
  TCCR5B &= ~(1 << CS51);    // clkI/O /8 prescaler 
  TCCR5B &= ~(1 << CS52);

  TIMSK5 |= (1 << OCIE5A);  // enable timer compare interrupt

  sei();             // enable all interrupts
  micro_sec = 0;
}

 ISR(TIMER5_COMPA_vect){          // timer compare interrupt service routine
   micro_sec++;  // one increment = 1 microsec 
}
