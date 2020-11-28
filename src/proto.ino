#include <PublishQueueAsyncRK.h>
#include <ECOTRONS-RS232-telemetry.h>

SYSTEM_THREAD(ENABLED);

String createData(String dataType, float data);
void onSerialData();
int cloudReset(String command);
int cloudChangeRate(String command);

bool resetSystem = false;
unsigned long resetRequest;
unsigned long RESET_TIMEOUT = 3000;

unsigned long RATE = 2000;
unsigned long time_ms;
time_t lastTime;

uint8_t publishQueueRetainedBuffer[16384];
PublishQueueAsync publishQueue(publishQueueRetainedBuffer, sizeof(publishQueueRetainedBuffer));
ECOTRONSRS232telemetry ecu(&Serial1);

Timer timer(1, onSerialData);

void setup() {
  Particle.function("reset", cloudReset);
  Particle.function("changeRate", cloudChangeRate);
  ecu.begin();
  timer.start();
  publishQueue.setup();
  time_ms = millis();
}

float rpm;
float o2s;
float ect;
float iat;
float spark_val;
float ubadc;

void loop() {
  if (resetSystem && millis() - resetRequest >= RESET_TIMEOUT) {
    System.reset();
  }
  unsigned long curr_time_ms = millis();
  if (curr_time_ms - time_ms >= RATE) {
    time_ms = millis();
    time_t timestamp = ecu.getTimestamp();
    rpm = ecu.getRPM();
    o2s = ecu.getO2S();
    ect = ecu.getECT();
    iat = ecu.getIAT();
    spark_val = ecu.getSpark();
    ubadc = ecu.getUbAdc();
    
    if (Particle.connected() && lastTime != timestamp) {
      lastTime = timestamp;
      String rpmS = createData("PROTO-RPM", rpm);
      String o2sS = createData("PROTO-O2S", o2s);
      String ectS = createData("PROTO-ECT", ect);
      String iatS = createData("PROTO-IAT", iat);
      String sparkS = createData("PROTO-SPARK", spark_val);
      String ubadcS = createData("PROTO-UBADC", ubadc);
      String payloadTime = String(lastTime);
      String payload = "{\"time\":" + payloadTime + ",\"d\":[" + rpmS + "," + o2sS + "," + ectS + "," + iatS + "," + sparkS + "," + ubadcS + "]}";
      
      publishQueue.publish("PROTO", payload, PUBLIC, WITH_ACK);
    }
  }
}

String createData(String dataType, float data)
{
  return "{\"t\":\"" + dataType + "\",\"d\":" + String(data, 3) + "}";
}

void onSerialData()
{
  ecu.onSerialData();
}

int cloudReset(String command)
{
  resetSystem = true;
  resetRequest = millis();
  return 0;
}

int cloudChangeRate(String command)
{
  unsigned long number = command.toInt();
  if (number) {
    RATE = number;
    return 0;
  } else {
    return -1;
  }
}