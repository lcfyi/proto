SYSTEM_THREAD(ENABLED);

void resetValues();
String createPayload(String t, String data);
String payloadTime;
void readSerialData();

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

void setup() {
  Serial1.begin(115200, SERIAL_8N1);
  Serial.begin(9600);
  time_ms = millis();
  resetValues();
}

void loop() {
  readSerialData();
  unsigned long curr_time_ms = millis();
  if (curr_time_ms - time_ms >= 2000) {
    time_ms = millis();
    if (Particle.connected()) {
      Particle.publish("PROTO-RPM", createPayload(payloadTime, String(rpm)), PUBLIC, WITH_ACK);
      Particle.publish("PROTO-IAT", createPayload(payloadTime, String(iat)), PUBLIC, WITH_ACK);
      Particle.publish("PROTO-ECT", createPayload(payloadTime, String(ect)), PUBLIC, WITH_ACK);
      Particle.publish("PROTO-UBADC", createPayload(payloadTime, String(ubadc)), PUBLIC, WITH_ACK);
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

void readSerialData() {
  // Clear any old data from the buffer
  while (Serial1.available()) {
    Serial1.read();
  }
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
  rpm = (float)((int)buffer[6] * 256 + (int)buffer[7]) * 0.25;
  ect = (float)((int)buffer[12] * 256 + (int)buffer[13]) - 40;
  iat = (float)((int)buffer[14] * 256 + (int)buffer[15]) - 40;
  ubadc = (float)((int)buffer[24] * 256 + (int)buffer[25]) * 0.00625;
  fuelpw1 = (float)((int)buffer[20] * 256 + (int)buffer[21]) * 0.001;
  // Serial.println(rpm);
  // Serial.println(ect);
  // Serial.println(iat);
  // Serial.println(ubadc);
  // Serial.println(fuelpw1);
  // // map_val = (float)(buffer[2] * 256 + buffer[3]) * 0.0039;
  // tps = (float)(buffer[4] * 256 + buffer[5]) * 0.0015;
  // ect = (float)(buffer[6] * 256 + buffer[7]) * 1.25 - 40;
  // iat = (float)(buffer[8] * 256 + buffer[9]) * 1.25 - 40;
  // o2s = (float)(buffer[10] * 256 + buffer[11]) * 0.0012;
  // spark_val = (float)(buffer[12] * 256 + buffer[13]) * 0.75;
  // fuelpw1 = (float)(buffer[14] * 256 + buffer[15]) * 0.01;
  // ubadc = (float)(buffer[18] * 256 + buffer[19]) * 0.00625;
  // Serial1.read();
  // Serial.println(timestamp)

}