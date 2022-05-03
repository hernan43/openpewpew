// OpenPewPew, active autofire using Android Nano
// buffi - 2019
//

// Button configs below.
// Four autofire buttons with different frequencies are allowed.
// When "input_pin" of a button is connected to GND, "output_pin" will start auto-firing.
// Each pulse is "frames_active" frames long, with "frames_inactive" frames between each pulse.
int LED = 3;

int SWITCH_PIN = 2;

int ON_INTERVAL = 1;
int OFF_INTERVAL = 0;

struct AutofireConfig {
  int frames_active, frames_inactive, input_pin, output_pin, autofire_state;
};

AutofireConfig BUTTON_1 = {
  ON_INTERVAL,
  OFF_INTERVAL,
  21,//A3
  15,//15
  0  
};

AutofireConfig BUTTON_2 = {
  ON_INTERVAL,
  OFF_INTERVAL,
  20,//A2
  14,//14
  0  
};

AutofireConfig BUTTON_3 = {
  ON_INTERVAL,
  OFF_INTERVAL,
  19,//A1
  16,//16
  0  
};

AutofireConfig BUTTON_4 = {
  ON_INTERVAL,
  OFF_INTERVAL,
  18,//A0
  310,//10
  0  
};

AutofireConfig *BUTTONS[] = {&BUTTON_1, &BUTTON_2, &BUTTON_3, &BUTTON_4};

// Code below.
boolean handle_button(AutofireConfig* button) {
  boolean should_fire = !digitalRead(button->input_pin);
  boolean afon = !digitalRead(SWITCH_PIN);
  boolean firing = false;
 
  if (should_fire) {
    if (
      (afon && (button->autofire_state++ < button->frames_active))
      || !afon
      ) {
        digitalWrite(button->output_pin, LOW);
        pinMode(button->output_pin, OUTPUT);
        digitalWrite(button->output_pin, LOW);
        firing = true;
        Serial.println("firing");
    } else {
        digitalWrite (button->output_pin, LOW) ;
        pinMode(button->output_pin, INPUT);
        Serial.println("off");
    }
    if (
      (button->autofire_state > (button->frames_active + button->frames_inactive))
      || !afon
    ) {
      button->autofire_state = 0;
    }
  } else {
    button->autofire_state = 0;
    digitalWrite (button->output_pin, LOW) ;
    pinMode(button->output_pin, INPUT);
  }
  return firing;
}

ISR(TIMER1_COMPA_vect) {
  boolean firing = false;
  for (int i=0; i<4; i++)
    firing |= handle_button(BUTTONS[i]);
  if (firing)
    digitalWrite (LED, HIGH);
  else
    digitalWrite (LED, LOW);
}

void setup() {
  Serial.begin(9600);
  Serial.println("Starting setup...");
  
  for (int i=0; i<4; i++) {
    pinMode(BUTTONS[i]->input_pin, INPUT_PULLUP);
    digitalWrite (BUTTONS[i]->output_pin, LOW) ;
    pinMode(BUTTONS[i]->output_pin, INPUT);
  }

  pinMode(SWITCH_PIN, INPUT_PULLUP);

  pinMode(LED, OUTPUT);
  digitalWrite (LED, LOW);

  // Setup timer.
  cli();
  TCCR1A = 0x00; // Timer 1 in normal mode
  TCCR1B = 0x02; // 16MHz clock with prescaler, TCNT1 increments every .5 uS
  TCNT1  = 0;    // Initialize counter to 0
  OCR1A = 33333; // = 16666 microseconds (each count is .5 us)
  TIMSK1 |= (1 << OCIE1A); // Enable compare interrupt
  sei();

  Serial.println("Completed setup.");
}

void loop() { delay(1); }
