// ============================================================
//  Digital Weather Station
//  Hardware : Arduino Uno + DHT11 + 1602 LCD (I2C adapter)
//  Wiring   : DHT11 Signal → D2 | LCD SDA → A4 | LCD SCL → A5
// ============================================================

#include <DHT.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// ── Pin & sensor config ─────────────────────────────────────
#define DHT_PIN   2
#define DHT_TYPE  DHT11

// ── I2C address: try 0x27 first; change to 0x3F if blank screen
#define LCD_ADDR  0x27
#define LCD_COLS  16
#define LCD_ROWS  2

// ── Refresh interval (ms) ───────────────────────────────────
#define REFRESH_MS 2000

// ── Custom degree symbol (°) — 5×8 pixel bitmap ────────────
byte degreeSymbol[8] = {
  0b00110,  //  ●●
  0b01001,  // ●  ●
  0b01001,  // ●  ●
  0b00110,  //  ●●
  0b00000,
  0b00000,
  0b00000,
  0b00000
};

// ── Object instantiation ────────────────────────────────────
DHT             dht(DHT_PIN, DHT_TYPE);
LiquidCrystal_I2C lcd(LCD_ADDR, LCD_COLS, LCD_ROWS);

// ── State tracking for flicker-free updates ─────────────────
float lastTemp = -999.0;
float lastHum  = -999.0;
unsigned long lastRefresh = 0;

// ── Helper: write a float right-padded to fill 5 chars ──────
//   Prevents stale digits when value shrinks (e.g. 100 → 99)
void printPadded(float value, int totalWidth) {
  char buf[8];
  dtostrf(value, totalWidth, 1, buf);   // width.1 decimal
  lcd.print(buf);
}

// ── Setup ────────────────────────────────────────────────────
void setup() {
  Serial.begin(9600);

  // Initialise LCD
  lcd.init();
  lcd.backlight();
  lcd.createChar(0, degreeSymbol);   // store ° at slot 0

  // Splash screen
  lcd.setCursor(0, 0);
  lcd.print("  Weather Stn   ");
  lcd.setCursor(0, 1);
  lcd.print("  Initialising  ");

  // Initialise sensor
  dht.begin();

  // DHT11 needs ~1 s after power-on before first valid reading
  delay(1500);
  lcd.clear();
}

// ── Main loop ────────────────────────────────────────────────
void loop() {
  unsigned long now = millis();

  // Only refresh every REFRESH_MS milliseconds
  if (now - lastRefresh < REFRESH_MS) return;
  lastRefresh = now;

  float temp = dht.readTemperature();   // Celsius
  float hum  = dht.readHumidity();

  // Validate — DHT11 occasionally returns NaN on a bad read
  if (isnan(temp) || isnan(hum)) {
    lcd.setCursor(0, 0);
    lcd.print("Sensor Error!   ");
    lcd.setCursor(0, 1);
    lcd.print("Retrying...     ");
    Serial.println("[WARN] DHT11 read failed — retrying");
    return;
  }

  // ── Update Line 1 only if temperature changed ────────────
  if (temp != lastTemp) {
    lcd.setCursor(0, 0);
    lcd.print("Temp: ");
    printPadded(temp, 5);         // e.g. " 27.0"
    lcd.write(byte(0));           // ° custom char
    lcd.print("C  ");             // trailing spaces erase old chars
    lastTemp = temp;
  }

  // ── Update Line 2 only if humidity changed ───────────────
  if (hum != lastHum) {
    lcd.setCursor(0, 1);
    lcd.print("Hum:  ");
    printPadded(hum, 5);          // e.g. " 65.0"
    lcd.print("%     ");
    lastHum = hum;
  }

  // Mirror to Serial Monitor for debugging
  Serial.print("Temp: ");
  Serial.print(temp, 1);
  Serial.print(" C  |  Hum: ");
  Serial.print(hum, 1);
  Serial.println(" %");
}
