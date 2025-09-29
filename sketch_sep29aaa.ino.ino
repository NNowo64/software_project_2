#include <Arduino.h>


#define PIN_LED  9
#define PIN_TRIG 12
#define PIN_ECHO 13


#define SND_VEL 346.0
#define INTERVAL 25
#define PULSE_DURATION 10
#define _DIST_MIN 100
#define _DIST_MAX 300
#define _EMA_ALPHA 0.9 

#define TIMEOUT ((INTERVAL / 2) * 1000.0)
#define SCALE (0.001 * 0.5 * SND_VEL)

const int N = 30; 


unsigned long last_sampling_time;
float dist_ema = _DIST_MAX;
float dist_raw;
float dist_median;
float samples[N];
int sample_index = 0;

void setup() {
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
  digitalWrite(PIN_TRIG, LOW);

  Serial.begin(57600);
}

void loop() {
  if (millis() < last_sampling_time + INTERVAL)
    return;

  dist_raw = USS_measure(PIN_TRIG, PIN_ECHO);
  updateSamples(dist_raw);

  dist_median = computeMedian(samples, N);


  dist_ema = _EMA_ALPHA * dist_median + (1.0 - _EMA_ALPHA) * dist_ema;


  Serial.print("Min:"); Serial.print(_DIST_MIN);
  Serial.print(",raw:"); Serial.print(dist_raw);
  Serial.print(",ema:"); Serial.print(dist_ema);
  Serial.print(",median:"); Serial.print(dist_median);
  Serial.print(",Max:"); Serial.print(_DIST_MAX);
  Serial.println();


  if ((dist_raw < _DIST_MIN) || (dist_raw > _DIST_MAX))
    digitalWrite(PIN_LED, HIGH);
  else
    digitalWrite(PIN_LED, LOW);

  last_sampling_time += INTERVAL;
}

float USS_measure(int TRIG, int ECHO) {
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(PULSE_DURATION);
  digitalWrite(TRIG, LOW);

  return pulseIn(ECHO, HIGH, TIMEOUT) * SCALE;
}

void updateSamples(float new_sample) {
  samples[sample_index] = new_sample;
  sample_index = (sample_index + 1) % N;
}

float computeMedian(float* array, int size) {
  float temp[size];
  memcpy(temp, array, sizeof(float) * size);
  for (int i = 0; i < size - 1; i++) {
    for (int j = i + 1; j < size; j++) {
      if (temp[i] > temp[j]) {
        float t = temp[i];
        temp[i] = temp[j];
        temp[j] = t;
      }
    }
  }
  if (size % 2 == 0)
    return (temp[size / 2 - 1] + temp[size / 2]) / 2.0;
  else
    return temp[size / 2];
}
