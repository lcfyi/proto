#include <PublishQueueAsyncRK.h>

SYSTEM_THREAD(ENABLED);

void resetValues();
String createPayload(String t, String data);
float getVal(uint8_t high, uint8_t low, float factor, float offset);
void readSerialData();

String payloadTime;
unsigned long RATE = 3000;
unsigned long time_ms;
// As listed on the EcoEFI documentation
float rpm;
float map_val;
float tps;
float ect;
float iat;
float o2s;
float spark_val;
float fuelpw1;
float fuelpw2;
float ubadc;
float fuellvl;
float baro;
float fuel_consumption;

uint8_t publishQueueRetainedBuffer[8192];
PublishQueueAsync publishQueue(publishQueueRetainedBuffer, sizeof(publishQueueRetainedBuffer));

void setup() {
  Serial1.begin(115200, SERIAL_8N1);
  Serial.begin(9600);
  time_ms = millis();
  resetValues();
}

void loop() {
  readSerialData();
  unsigned long curr_time_ms = millis();
  if (curr_time_ms - time_ms >= RATE) {
    time_ms = millis();
    if (Particle.connected()) {
      publishQueue.publish("PROTO-RPM", createPayload(payloadTime, String(rpm)), PUBLIC, WITH_ACK);
      publishQueue.publish("PROTO-O2S", createPayload(payloadTime, String(o2s)), PUBLIC, WITH_ACK);
      publishQueue.publish("PROTO-ECT", createPayload(payloadTime, String(ect)), PUBLIC, WITH_ACK);
      publishQueue.publish("PROTO-IAT", createPayload(payloadTime, String(iat)), PUBLIC, WITH_ACK);
      publishQueue.publish("PROTO-SPARK", createPayload(payloadTime, String(spark_val)), PUBLIC, WITH_ACK);
      publishQueue.publish("PROTO-UBADC", createPayload(payloadTime, String(ubadc)), PUBLIC, WITH_ACK);
    }
  }
}

void resetValues() {
  rpm = 0;
  map_val = 0.0;
  tps = 0.0;
  ect = 0.0;
  iat = 0.0;
  o2s = 0.0;
  spark_val = 0.0;
  fuelpw1 = 0.0;
  fuelpw2 = 0.0;
  ubadc = 0.0;
  fuellvl = 0.0;
  baro = 0.0;
  fuel_consumption = 0.0;
  payloadTime = String(Time.now());
}

String createPayload(String t, String data) {
  t.concat("||");
  t.concat(data);
  return t;
}

float getVal(uint8_t high, uint8_t low, float factor, float offset) {
  return (float)((int)high * 256 + (int)low) * factor + offset;
}

void readSerialData() {
  // Clear any old data from the buffer
  // while (Serial1.available()) {
  //   Serial1.read();
  // }
  while (Serial1.available() < 27) {
    continue;
  }
  payloadTime = String(Time.now());
  char buffer[27];
  Serial1.readBytes(buffer, 27);
  Serial.println("------------------");
  char header1 = buffer[0] & 0xFF;
  char header2 = buffer[1] & 0xFF;
  char header3 = buffer[2] & 0xFF;
  char dataFieldLength = buffer[3] & 0xFF;
  char serviceId = buffer[4] & 0xFF;
  if (header1 == 0x80 && header2 == 0x8F && header3 == 0xEA && dataFieldLength == 0x16 && serviceId == 0x50) {
    Serial.println("Header verified");
  }
  rpm = getVal(buffer[6], buffer[7], 0.25, 0.0);
  map_val = getVal(buffer[8], buffer[9], 0.0039, 0.0);
  tps = getVal(buffer[10], buffer[11], 0.0015, 0.0);
  ect = getVal(buffer[12], buffer[13], 1, -40.0);
  iat = getVal(buffer[14], buffer[15], 1, -40.0);
  o2s = getVal(buffer[16], buffer[17], 0.0048, 0.0);
  spark_val = getVal(buffer[18], buffer[19], 0.5, 0.0);
  fuelpw1 = getVal(buffer[20], buffer[21], 0.001, 0.0);
  fuelpw2 = getVal(buffer[22], buffer[23], 0.001, 0.0);
  ubadc = getVal(buffer[24], buffer[25], 0.00625, 0.0);
}