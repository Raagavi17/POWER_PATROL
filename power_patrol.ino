#define BLYNK_TEMPLATE_ID "TMPL3SC9E8FOT"
#define BLYNK_TEMPLATE_NAME "Power Patrol"
#define BLYNK_AUTH_TOKEN "5rYgPDnqclkLH1gaJwdzs6aBGyBhSqkN"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

char ssid[] = "OPPO A15";
char pass[] = "12345678";

#define PIR_INPUT  2   // GPIO2 for PIR sensor
#define LED_PIN  4  // GPIO15 for LED
#define PIR_POWER   5

bool lightState = false;
unsigned long lastMotionTime = 0;
const unsigned long timeoutDuration = 2 * 60 * 1000UL;

BlynkTimer timer;

BLYNK_WRITE(V0)
{
  lightState = param.asInt();
  digitalWrite(LED_PIN, lightState);
  digitalWrite(PIR_POWER, lightState);
  Blynk.virtualWrite(V1, lightState);          // Update status
  if (lightState) {
    lastMotionTime = millis();  // Reset timer when turned ON
  }
}

void checkPIR() 
{
  Serial.println("pir called");
  Serial.println(lightState);
  if (!lightState) return;  
  int motion = digitalRead(PIR_INPUT);
  Serial.println(motion);

  if (motion == HIGH) 
  {
      lastMotionTime = millis();  // Motion detected,Updating last motion timestamp"
      Serial.println("Motion detected. ");
  }
  else
 {
    Serial.println("No motion currently.");
    if (millis() - lastMotionTime >= timeoutDuration) 
      {
        digitalWrite(LED_PIN, LOW);        // Turn OFF light
        digitalWrite(PIR_POWER, LOW);      // Turn off PIR
        lightState = false;
        Blynk.virtualWrite(V0, 0);         // Sync app switch
        Blynk.virtualWrite(V1, 0);         // Update status
        Serial.println("No motion for 2 mins. Turning OFF.");
      } 
  }
}


void setup() 
{
  Serial.begin(115200);
  pinMode(PIR_INPUT, INPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(PIR_POWER, OUTPUT);
  digitalWrite(PIR_POWER, LOW); 
  
  Serial.println("Connecting to Blynk...");
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  Serial.println("Connected to Blynk.");
  
  timer.setInterval(2000L, checkPIR); // Call checkPIR every 2 seconds
}

void loop()
{
  Blynk.run();
  timer.run();
}
