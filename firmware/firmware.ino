#define PIN_READY 2
#define PIN_CLOCK 3
#define PIN_D0 4
#define PIN_D1 5
#define PIN_D2 6
#define PIN_D3 7
#define PIN_D4 8
#define PIN_D5 9
#define PIN_D6 10
#define PIN_D7 11
#define PIN_WE 12
#define PIN_CE 13

// #define LED_PIN 13

#define DELAY 50 // 50

void setup()
{
  pinMode(PIN_READY, INPUT);
  pinMode(PIN_CLOCK, OUTPUT);
/*
  TCCR2A = 0x23;
  TCCR2B = 0x09;
  OCR2A = 3;
  OCR2B = 1;
*/
  pinMode(PIN_D0, OUTPUT);
  pinMode(PIN_D1, OUTPUT);
  pinMode(PIN_D2, OUTPUT);
  pinMode(PIN_D3, OUTPUT);
  pinMode(PIN_D4, OUTPUT);
  pinMode(PIN_D5, OUTPUT);
  pinMode(PIN_D6, OUTPUT);
  pinMode(PIN_D7, OUTPUT);
  pinMode(PIN_WE, OUTPUT);
  pinMode(PIN_CE, OUTPUT);

  digitalWrite(PIN_CE, HIGH);
  digitalWrite(PIN_WE, HIGH);

  writeByte(0b10011111);
  writeByte(0b10111111);
  writeByte(0b11011111);
  writeByte(0b11111111);
  
  // initialize serial communication
  Serial.begin(9600);
  
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB
  }
}

// Possible state-machine states
typedef enum {
  NONE,
  PSG_WRITE
} states;

// Current state-machine state
states state = NONE;

// Number of command argument bytes
int commandLength = 0;

// Command argument bytes storage
byte commandBuffer[8];

int commandBufferPointer;

void processIncomingByte(const byte c)
{
  // Looking for next command to execute
  if(commandLength == 0) {
    commandBufferPointer = 0;
    switch(c) {
      case 0x50: {
        state = PSG_WRITE;
        commandLength = 1;
        break;
      }
    }
  } else if(commandBufferPointer < commandLength) {
    // Write current byte in command buffer
    commandBuffer[commandBufferPointer++] = c;
    if(commandBufferPointer >= commandLength) {
      processCurrentCommand();
      commandBufferPointer = 0;
      commandLength = 0;
    }
  }

//  Serial.write(c);
}

void processCurrentCommand()
{
  switch(state) {
    case PSG_WRITE: {
      writeByte(commandBuffer[0]);
      break;
    }
  }
}

void writeByte(const byte b)
{
//  delay(DELAY);
  
  digitalWrite(PIN_CE, LOW);
  
//  digitalWrite(PIN_D0, (b&1)?HIGH:LOW);
//  digitalWrite(PIN_D1, (b&2)?HIGH:LOW);
//  digitalWrite(PIN_D2, (b&4)?HIGH:LOW);
//  digitalWrite(PIN_D3, (b&8)?HIGH:LOW);
//  digitalWrite(PIN_D4, (b&16)?HIGH:LOW);
//  digitalWrite(PIN_D5, (b&32)?HIGH:LOW);
//  digitalWrite(PIN_D6, (b&64)?HIGH:LOW);
//  digitalWrite(PIN_D7, (b&128)?HIGH:LOW);
  
//  digitalWrite(PIN_D0, (b >> 0) & 1); // 1
//  digitalWrite(PIN_D1, (b >> 1) & 1); // R0
//  digitalWrite(PIN_D2, (b >> 2) & 1); // R1
//  digitalWrite(PIN_D3, (b >> 3) & 1); // R2
//  digitalWrite(PIN_D4, (b >> 4) & 1);  // A0
//  digitalWrite(PIN_D5, (b >> 5) & 1);  // A1
//  digitalWrite(PIN_D6, (b >> 6) & 1);  // A2
//  digitalWrite(PIN_D7, (b >> 7) & 1);  // A3
  
  digitalWrite(PIN_D0, (b >> 7) & 1); // 1
  digitalWrite(PIN_D1, (b >> 6) & 1); // R0
  digitalWrite(PIN_D2, (b >> 5) & 1); // R1
  digitalWrite(PIN_D3, (b >> 4) & 1); // R2
  digitalWrite(PIN_D4, (b >> 3) & 1);  // A0
  digitalWrite(PIN_D5, (b >> 2) & 1);  // A1
  digitalWrite(PIN_D6, (b >> 1) & 1);  // A2
  digitalWrite(PIN_D7, (b >> 0) & 1);  // A3
  
//  delay(DELAY);
  
  digitalWrite(PIN_WE, LOW);
  
  delay(1);
  
  digitalWrite(PIN_CE, HIGH);
  digitalWrite(PIN_WE, HIGH);
}

void loop()
{
//  Serial.println(sizeof(buf));
  // see if there's incoming serial data:
  if(Serial.available()) {
    processIncomingByte(Serial.read());
//    readFileStream(Serial.read())
//    Serial.write(readyBytes(4));
  }
}

void chipSetup() {
  pinMode(PIN_READY, INPUT);
  pinMode(PIN_CLOCK, OUTPUT);
/*
  TCCR2A = 0x23;
  TCCR2B = 0x09;
  OCR2A = 3;
  OCR2B = 1;
*/
  pinMode(PIN_D0, OUTPUT);
  pinMode(PIN_D1, OUTPUT);
  pinMode(PIN_D2, OUTPUT);
  pinMode(PIN_D3, OUTPUT);
  pinMode(PIN_D4, OUTPUT);
  pinMode(PIN_D5, OUTPUT);
  pinMode(PIN_D6, OUTPUT);
  pinMode(PIN_D7, OUTPUT);
  pinMode(PIN_WE, OUTPUT);
  pinMode(PIN_CE, OUTPUT);

  digitalWrite(PIN_CE, HIGH);
  digitalWrite(PIN_WE, HIGH);

  /////////////////////
  
  delay(DELAY);

  digitalWrite(PIN_CE, LOW);
  
  digitalWrite(PIN_D0, HIGH); // 1
  digitalWrite(PIN_D1, LOW); // R0
  digitalWrite(PIN_D2, LOW); // R1
  digitalWrite(PIN_D3, HIGH); // R2
  digitalWrite(PIN_D4, HIGH);  // A0
  digitalWrite(PIN_D5, HIGH);  // A1
  digitalWrite(PIN_D6, HIGH);  // A2
  digitalWrite(PIN_D7, HIGH);  // A3
  
  delay(DELAY);
  
  digitalWrite(PIN_WE, LOW);
  
  delay(DELAY);

  digitalWrite(PIN_CE, HIGH);
  digitalWrite(PIN_WE, HIGH);

  /////////////////////
  
  delay(DELAY);

  digitalWrite(PIN_CE, LOW);
  
  digitalWrite(PIN_D0, HIGH); // 1
  digitalWrite(PIN_D1, LOW); // R0
  digitalWrite(PIN_D2, HIGH); // R1
  digitalWrite(PIN_D3, HIGH); // R2
  digitalWrite(PIN_D4, HIGH);  // A0
  digitalWrite(PIN_D5, HIGH);  // A1
  digitalWrite(PIN_D6, HIGH);  // A2
  digitalWrite(PIN_D7, HIGH);  // A3
  
  delay(DELAY);
  
  digitalWrite(PIN_WE, LOW);
  
  delay(DELAY);

  digitalWrite(PIN_CE, HIGH);
  digitalWrite(PIN_WE, HIGH);

  /////////////////////
  
  delay(DELAY);

  digitalWrite(PIN_CE, LOW);
  
  digitalWrite(PIN_D0, HIGH); // 1
  digitalWrite(PIN_D1, HIGH); // R0
  digitalWrite(PIN_D2, LOW); // R1
  digitalWrite(PIN_D3, HIGH); // R2
  digitalWrite(PIN_D4, HIGH);  // A0
  digitalWrite(PIN_D5, HIGH);  // A1
  digitalWrite(PIN_D6, HIGH);  // A2
  digitalWrite(PIN_D7, HIGH);  // A3
  
  delay(DELAY);
  
  digitalWrite(PIN_WE, LOW);
  
  delay(DELAY);

  digitalWrite(PIN_CE, HIGH);
  digitalWrite(PIN_WE, HIGH);

  /////////////////////
  
  delay(DELAY);

  digitalWrite(PIN_CE, LOW);
  
  digitalWrite(PIN_D0, HIGH); // 1
  digitalWrite(PIN_D1, HIGH); // R0
  digitalWrite(PIN_D2, HIGH); // R1
  digitalWrite(PIN_D3, HIGH); // R2
  digitalWrite(PIN_D4, LOW);  // A0
  digitalWrite(PIN_D5, LOW);  // A1
  digitalWrite(PIN_D6, LOW);  // A2
  digitalWrite(PIN_D7, LOW);  // A3
  
  delay(DELAY);
  
  digitalWrite(PIN_WE, LOW);
  
  delay(DELAY);

  digitalWrite(PIN_CE, HIGH);
  digitalWrite(PIN_WE, HIGH);

  /////////////////////
  
  delay(DELAY);

  digitalWrite(PIN_CE, LOW);
  
  digitalWrite(PIN_D0, HIGH); // 1
  digitalWrite(PIN_D1, HIGH); // R0
  digitalWrite(PIN_D2, HIGH); // R1
  digitalWrite(PIN_D3, LOW);  // R2
  digitalWrite(PIN_D4, HIGH); // UNUSED
  digitalWrite(PIN_D5, HIGH); // FB
  digitalWrite(PIN_D6, LOW);  // NF0
  digitalWrite(PIN_D7, LOW);  // NF1
  
  delay(DELAY);
  
  digitalWrite(PIN_WE, LOW);
  
  delay(DELAY);

  digitalWrite(PIN_CE, HIGH);
  digitalWrite(PIN_WE, HIGH);
  
  //write_byte(0b10011111); // Tone 1 Attenuation
  //write_byte(0b10111111); // Tone 2 Attenuation
  //write_byte(0b11011111); // Tone 3 Attenuation
  /*
  digitalWrite(PIN_WE, HIGH);
  
  digitalWrite(PIN_D0, HIGH); // 1
  digitalWrite(PIN_D1, LOW);  // R0
  digitalWrite(PIN_D2, LOW);  // R1
  digitalWrite(PIN_D3, HIGH); // R2
  digitalWrite(PIN_D4, LOW); // A0
  digitalWrite(PIN_D5, LOW); // A1
  digitalWrite(PIN_D6, LOW); // A2
  digitalWrite(PIN_D7, LOW); // A3
  
  digitalWrite(PIN_WE, LOW);
  delay(1000);
  digitalWrite(PIN_WE, HIGH);
  
  digitalWrite(PIN_D0, HIGH); // 1
  digitalWrite(PIN_D1, LOW);  // R0
  digitalWrite(PIN_D2, HIGH); // R1
  digitalWrite(PIN_D3, HIGH); // R2
  digitalWrite(PIN_D4, LOW); // A0
  digitalWrite(PIN_D5, LOW); // A1
  digitalWrite(PIN_D6, LOW); // A2
  digitalWrite(PIN_D7, LOW); // A3
  
  digitalWrite(PIN_WE, LOW);
  delay(1000);
  digitalWrite(PIN_WE, HIGH);
  
  digitalWrite(PIN_D0, HIGH); // 1
  digitalWrite(PIN_D1, HIGH); // R0
  digitalWrite(PIN_D2, LOW);  // R1
  digitalWrite(PIN_D3, HIGH); // R2
  digitalWrite(PIN_D4, LOW); // A0
  digitalWrite(PIN_D5, LOW); // A1
  digitalWrite(PIN_D6, LOW); // A2
  digitalWrite(PIN_D7, LOW); // A3
  
  digitalWrite(PIN_WE, LOW);
  delay(1000);
  digitalWrite(PIN_WE, HIGH);
  */
  //write_byte(0b11100100);
  //write_byte(0b11110000);
}

void write_byte(byte b)
{
  digitalWrite(PIN_WE, HIGH);
  
  digitalWrite(PIN_D0, (b&1)?HIGH:LOW);
  digitalWrite(PIN_D1, (b&2)?HIGH:LOW);
  digitalWrite(PIN_D2, (b&4)?HIGH:LOW);
  digitalWrite(PIN_D3, (b&8)?HIGH:LOW);
  digitalWrite(PIN_D4, (b&16)?HIGH:LOW);
  digitalWrite(PIN_D5, (b&32)?HIGH:LOW);
  digitalWrite(PIN_D6, (b&64)?HIGH:LOW);
  digitalWrite(PIN_D7, (b&128)?HIGH:LOW);
  
  digitalWrite(PIN_WE, LOW);
  delay(1000);
  digitalWrite(PIN_WE, HIGH);
}

// the loop function runs over and over again forever
//void loop() {
//  /*
//  digitalWrite(LED_PIN, digitalRead(PIN_READY));
//  
//  digitalWrite(PIN_CLK, HIGH);
//  delay(DELAY);
//  digitalWrite(PIN_CLK, LOW);
//  delay(DELAY);
//  */
//}
