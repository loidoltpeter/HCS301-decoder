//simple keeloq decoder without interrupts (works with every pin)

#define HCS_RX_PIN  11

uint32_t  rollingCode;      
uint32_t  serialNr;
uint8_t   message;

void setup()
{
  pinMode(HCS_RX_PIN, INPUT);
  
  Serial.begin(9600);
  Serial.println("Setup OK");
}

void loop()
{
  HCS_301(); //run this in a loop when you want to receive
}

void HCS_301(){

  uint32_t  HCS_last_change;
  uint8_t   HCS_preamble_counter;
  uint8_t   HCS_bit_counter;
  uint8_t   HCS_bit_array[66];
  
  //looking for preamble
  HCS_preamble_counter = 0;
  while (HCS_preamble_counter < 12) {
    HCS_last_change = micros();
    while (digitalRead(HCS_RX_PIN) == LOW) {
    }
   if (HCS_last_change + 500  < micros()) {
     goto exit;                          //exit when pulses are too long
   }
   HCS_last_change = micros();
    while (digitalRead(HCS_RX_PIN) == HIGH) {
    }
   if (HCS_last_change + 300 > micros()) {
     goto exit;                          //exit when pulses are too short
  }
    HCS_preamble_counter ++;
  }
  //looking for header
  HCS_last_change = micros();
  while (HCS_last_change + 3800 > micros()) {
    if (digitalRead(HCS_RX_PIN) == HIGH) {
      goto exit;
    }
  }

  //read data
  HCS_bit_counter = 0;
  while (HCS_bit_counter < 66) {
    HCS_last_change = micros();
    while (digitalRead(HCS_RX_PIN) == LOW) {    
    }
    if (HCS_last_change + 1000 < micros()) {
      goto exit;                          //exit when pulses are too long
    }
    HCS_last_change = micros();
    while (digitalRead(HCS_RX_PIN) == HIGH) {    
    }
    if (HCS_last_change + 300 > micros()) {
      goto exit;                          //exit when pulses are too short
    }
    if (HCS_last_change + 600 > micros()) {
      HCS_bit_array[HCS_bit_counter] = 1; //write 1 to bitarray on short puls
    }
    else {
      HCS_bit_array[HCS_bit_counter] = 0; //write 0 to bitarray on long puls
    }
    HCS_bit_counter ++;
  }

  //format data
  rollingCode = 0;
  serialNr = 0;
  message = 0;
  
  for (int i = 34; i<66; i++) {
    rollingCode = (rollingCode << 1) + HCS_bit_array[65-i];
  }
  for (int i = 6; i<34; i++) {
    serialNr = (serialNr << 1) + HCS_bit_array[65-i];
  }
  for (int i = 0; i<6; i++) {
    message = (message << 1) + HCS_bit_array[65-i];
  }

  Serial.println(serialNr,DEC);
  //Serial.println(rollingCode,DEC);
  Serial.println(message,BIN);
  
  exit:;
}
