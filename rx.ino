// connect ch 1-4 to digital 2-5

void setup()
{
  DDRC = 0; // pins as input
  
  // enable PCINT 18 to 23
  PCICR |= (1 << PCIE2);
  PCMSK2 = 0x3C;
  
  Serial.begin(115200);
}

typedef struct {
  byte edge;
  unsigned long riseTime;
  unsigned long fallTime;
  unsigned int  lastGoodWidth;
} tPinTimingData;

volatile static tPinTimingData pinData[6 + 1];
volatile static uint8_t PCintLast;
uint32_t temp = 0;

ISR(PCINT2_vect)
{
  uint8_t bit;
  uint8_t curr;
  uint8_t mask;
  uint32_t currentTime;
  uint32_t time;
  
  // get the pin states for the indicated port.
  curr = PIND & 0x3C;
  mask = curr ^ PCintLast;
  PCintLast = curr;
  
  currentTime = micros();
  
  // mask is pcint pins that have changed.
  for (uint8_t i=0; i < 4; i++) {
    bit = 0x04 << i;
    if (bit & mask) {
      temp++;
      // for each pin changed, record time of change
      if (bit & PCintLast) {
        time = currentTime - pinData[i].fallTime;
        pinData[i].riseTime = currentTime;
        if ((time >= 10000) && (time <= 26000))
          pinData[i].edge = 1;
        else
          pinData[i].edge = 0; // invalid rising edge detected
      }
      else {
        time = currentTime - pinData[i].riseTime;
        pinData[i].fallTime = currentTime;
        if ((time >= 800) && (time <= 2200) && (pinData[i].edge == 1)) {
          pinData[i].lastGoodWidth = time;
          pinData[i].edge = 0;
        }
      }
    }
  }
}

void loop()
{
  Serial.println();
  for (byte i = 0; i < 4; i++) {
    if( i == 0 )
    {
      Serial.print("ail");
    }
    else if( i == 1 )
    {
      Serial.print("ele");
    }
    else if( i == 2 )
    {
      Serial.print("thr");
    }
    else if( i == 3 )
    {
      Serial.print("rudo");
    }
    Serial.print(": ");
    Serial.print(pinData[i].lastGoodWidth);
    Serial.print(", ");
  }
  
  delay(500);
}

