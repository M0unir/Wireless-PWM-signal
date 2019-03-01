#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

/*** BEGIN CONFIGURATION ***/

    /* Choose which device you're compiling for */
    #define MASTER 
    // #define SLAVE 
    // #define ESPMini // Uncomment if this is an ESPMini (ESP-01) device. Use TX || RX Pin.

    /* Setup Wifi Credentials*/
    const char* const ssid = "--------------"; // Your network SSID (name)
    const char* const pass = "--------------"; // Your network password

    /* Setup Servo */
    #ifdef SLAVE
        byte SERVO_PIN = 4;   // (D2) Set PWM Pin to use on ESP8266 
        byte SERVO_CHANNEL_AUX = 1;   // Set Channel to Output to Servo {1:AILERON , 2: ELEVATOR, 3:RUDDER or 4:THROTTLE}
    #endif

    /* Setup Master Input Pins */
    #define CHANNEL_1_PIN 4  // D2 PIN on ESP8266
    #define CHANNEL_2_PIN 12 // D6 PIN on ESP8266
    #define CHANNEL_3_PIN 14 // D5 PIN on ESP8266
    #define CHANNEL_4_PIN 15 // D8 PIN on ESP8266

/*** END CONFIGURATION ***/

// Led pin used for Wifi Status: Flashing: Trying to connect. OFF: Connected.
const int8_t ledPin = LED_BUILTIN;

WiFiUDP udp;
char incomingPacket[255];  // buffer for incoming packets
const uint16_t localPort = 54321; // Local port to listen for UDP packets

#ifdef SLAVE
    #include <Servo.h>
    Servo myservo;     
    int ServoPWM; 
    int pVal[4]; // PWM Data Array
#endif

#ifdef MASTER

IPAddress broadcastAddress; 

int pwm_value;
int pwm_value2;
int pwm_value3;
int pwm_value4;

//micros when the pin goes HIGH
volatile unsigned long timer_start1;
volatile unsigned long timer_start2;
volatile unsigned long timer_start3;
volatile unsigned long timer_start4;

//difference between timer_start and micros() is the length of time that the pin 
//was HIGH - the PWM pulse length. 
volatile int pulse_time; 
volatile int pulse_time2; 
volatile int pulse_time3; 
volatile int pulse_time4; 

//this is the time that the last interrupt occurred. 
//you can use this to determine if your receiver has a signal or not. 
volatile int last_interrupt_time; //calcSignal is the interrupt handler 
volatile int last_interrupt_time2; //calcSignal is the interrupt handler 
volatile int last_interrupt_time3; //calcSignal is the interrupt handler 
volatile int last_interrupt_time4; //calcSignal is the interrupt handler 

void calcSignal() 
{
    //record the interrupt time so that we can tell if the receiver has a signal from the transmitter 
    last_interrupt_time = micros(); 
    //if the pin has gone HIGH, record the microseconds since the Arduino started up 
    if(digitalRead(CHANNEL_1_PIN) == HIGH) 
    { 
        timer_start1 = micros();
    } 
    //otherwise, the pin has gone LOW 
    else
    { 
        //only worry about this if the timer has actually started
        if(timer_start1 != 0)
        { 
            //record the pulse time
            pulse_time = ((volatile int)micros() - timer_start1);
            //restart the timer
            timer_start1 = 0;
            if ( pulse_time >= 2000  ) {
              pulse_time = 2000;
            }
            
            if ( pulse_time <= 1000  ) {
              pulse_time = 1000;
            }            
        }
    } 
} 

void calcSignal2() 
{
    last_interrupt_time2 = micros(); 
    if(digitalRead(CHANNEL_2_PIN) == HIGH) 
    { 
        timer_start2 = micros();
    } 
    else
    { 
        if(timer_start2 != 0)
        { 
            pulse_time2 = ((volatile int)micros() - timer_start2);
            timer_start2 = 0;
            if ( pulse_time2 >= 2000  ) {
              pulse_time2 = 2000;
            }
            
            if ( pulse_time2 <= 1000  ) {
              pulse_time2 = 1000;
            }
        }
    } 
} 

void calcSignal3() 
{
    last_interrupt_time3 = micros(); 
    if(digitalRead(CHANNEL_3_PIN) == HIGH) 
    { 
        timer_start3 = micros();
    } 
    else
    { 
        if(timer_start3 != 0)
        { 
            pulse_time3 = ((volatile int)micros() - timer_start3);
            timer_start3 = 0;
            if ( pulse_time3 >= 2000  ) {
              pulse_time3 = 2000;
            }
            
            if ( pulse_time3 <= 1000  ) {
              pulse_time3 = 1000;
            }            
        }
    } 
} 

void calcSignal4() 
{
    last_interrupt_time4 = micros(); 
    if(digitalRead(CHANNEL_4_PIN) == HIGH) 
    { 
        timer_start4 = micros();
    } 
    else
    { 
        if(timer_start4 != 0)
        { 
            pulse_time4 = ((volatile int)micros() - timer_start4);
            timer_start4 = 0;
            if ( pulse_time4 >= 2000  ) {
              pulse_time4 = 2000;
            }
            
            if ( pulse_time4 <= 1000  ) {
              pulse_time4 = 1000;
            }
        }
    } 
} 

#endif

void setup() {
  Serial.begin(115200);
  Serial.println();

  #ifdef ESPMini

    //********** CHANGE PIN FUNCTION  TO GPIO **********
        //GPIO 1 (TX) swap the pin to a GPIO.
        pinMode(1, FUNCTION_3); 
        //GPIO 3 (RX) swap the pin to a GPIO.
        pinMode(3, FUNCTION_3); 
    //**************************************************

  #endif

  pinMode(ledPin, OUTPUT);

  Serial.print(F("Connecting to "));
  Serial.println(ssid);
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(ledPin, ! digitalRead(ledPin));
    delay(500);
    Serial.print('.');
  }
  digitalWrite(ledPin, HIGH);
  Serial.println();

  Serial.println(F("WiFi connected"));
  Serial.println(F("IP address: "));
  Serial.println(WiFi.localIP());

#ifdef MASTER
  broadcastAddress = (uint32_t)WiFi.localIP() | ~((uint32_t)WiFi.subnetMask());
  Serial.print(F("Broadcast address: "));
  Serial.println(broadcastAddress);
#endif

#ifdef SLAVE
  myservo.attach(SERVO_PIN); 
#endif

  Serial.println(F("Starting UDP"));
  udp.begin(localPort);
  Serial.print(F("Local port: "));
  Serial.println(udp.localPort());

#ifdef MASTER

  timer_start1 = 0; 
  timer_start2 = 0; 
  timer_start3 = 0; 
  timer_start4 = 0; 
  
  attachInterrupt(CHANNEL_1_PIN, calcSignal, CHANGE);
  attachInterrupt(CHANNEL_2_PIN, calcSignal2, CHANGE);
  attachInterrupt(CHANNEL_3_PIN, calcSignal3, CHANGE);
  attachInterrupt(CHANNEL_4_PIN, calcSignal4, CHANGE);
#endif

}

void loop() {

#ifdef MASTER
  static uint32_t nextTime = 0;
  
  String Channel1 = "1:";
  String Channel1Value = Channel1 + pulse_time + ",";
  String Channel2 = "2:";
  String Channel2Value = Channel1Value + Channel2 + pulse_time2 + ",";
  String Channel3 = "3:";
  String Channel3Value = Channel2Value + Channel3 + pulse_time3 + ",";
  String Channel4 = "4:";
  String AllChannels = Channel3Value + Channel4 + pulse_time4;
  
  Serial.println(AllChannels);

  udp.beginPacket(broadcastAddress, localPort);
  char Buffer[33];
  AllChannels.toCharArray(Buffer, 33);
  udp.write(Buffer);
  udp.endPacket();
  
  delay(60);

#endif

#ifdef SLAVE
  // Handle Packet and Write to Servo  
  if (udp.parsePacket()){

    int len = udp.read(incomingPacket, 255);
    if (len > 0)
    {
      incomingPacket[len] = 0;
    }
    Serial.printf("UDP packet contents: %s\n", incomingPacket);

    // Read and Store data into array then check if we're really getting 4 channels 
    if ( sscanf(incomingPacket,"1:%d,2:%d,3:%d,4:%d", &pVal[0],&pVal[1],&pVal[2],&pVal[3]) == 4 ) {
      
      ServoPWM = pVal[SERVO_CHANNEL_AUX];
      myservo.write(ServoPWM); 
      
      delay(15);

      } else {
        Serial.println("Count bad");
      };  

     udp.flush();
     }  
#endif

} 
