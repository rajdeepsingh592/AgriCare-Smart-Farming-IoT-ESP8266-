#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>
#include <ESP8266WiFi.h>
#include <ESP_Mail_Client.h>

// ---------------- WIFI ----------------
#define WIFI_SSID "IOT"
#define WIFI_PASSWORD "12345678"

// ---------------- EMAIL ----------------
#define SMTP_HOST "smtp.gmail.com"
#define SMTP_PORT 465

#define AUTHOR_EMAIL "rajdeepsingh5272@gmail.com"
#define AUTHOR_PASSWORD "ylhh cisx rxed rkgo"
#define RECIPIENT_EMAIL "projecthub5272@gmail.com"

// ---------------- OLED ----------------
Adafruit_SSD1306 display(128, 64, &Wire, -1);

// ---------------- DHT ----------------
#define DHTPIN D8
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// ---------------- PINS ----------------
#define SOIL_PIN A0
#define GAS_PIN D1        // Methane / Hydrogen
#define SMOKE_PIN D2
#define WIFI_LED D0

SMTPSession smtp;

// ---------- SMTP CALLBACK ----------
void smtpCallback(SMTP_Status status) {
  Serial.println(status.info());
}

// ---------------- SEND EMAIL ----------------
void sendEmail(String subject, String body) {

  SMTP_Message message;
  message.sender.name = "AgriCare System";
  message.sender.email = AUTHOR_EMAIL;
  message.subject = subject;
  message.addRecipient("Farmer", RECIPIENT_EMAIL);
  message.text.content = body.c_str();
  message.text.charSet = "utf-8";

  ESP_Mail_Session session;
  session.server.host_name = SMTP_HOST;
  session.server.port = SMTP_PORT;
  session.login.email = AUTHOR_EMAIL;
  session.login.password = AUTHOR_PASSWORD;

  smtp.callback(smtpCallback);
  digitalWrite(WIFI_LED, HIGH);

  if (!smtp.connect(&session)) return;
  MailClient.sendMail(&smtp, &message);

  // ESP LED blink
  for (int i = 0; i < 6; i++) {
    digitalWrite(LED_BUILTIN, LOW);
    delay(200);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(200);
  }

  digitalWrite(WIFI_LED, LOW);
}

// ---------------- SETUP ----------------
void setup() {
  Serial.begin(115200);

  pinMode(GAS_PIN, INPUT);
  pinMode(SMOKE_PIN, INPUT);
  pinMode(WIFI_LED, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  Wire.begin(D3, D4);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  dht.begin();
  randomSeed(analogRead(A0));

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) delay(500);
}

// ---------------- LOOP ----------------
void loop() {

  float temp = dht.readTemperature();
  float hum  = dht.readHumidity();
  int soil = map(analogRead(SOIL_PIN), 1024, 300, 0, 100);
  soil = constrain(soil, 0, 100);

  bool gas   = digitalRead(GAS_PIN) == LOW;
  bool smoke = digitalRead(SMOKE_PIN) == LOW;

  // -------- OLED (NO EMOJI) --------
  display.clearDisplay();
  display.setCursor(0,0);
  display.setTextSize(1);
  display.println("AgriCare Monitor");
  display.println("Temp : " + String(temp) + " C");
  display.println("Hum  : " + String(hum) + " %");
  display.println("Soil : " + String(soil) + " %");
  display.println((gas || smoke) ? "Air : DANGER" : "Air : SAFE");
  display.display();

  String alerts = "";
  String actions = "";

  // 🌱 SOIL MOISTURE (WITH QUESTION)
  if (soil < 30) {
    alerts += "💧 Soil Moisture LOW\n";
    actions +=
      "❓ Is irrigation system currently ON?\n\n"
      "• 🚿 Start irrigation immediately\n"
      "• 🌾 Use flood or drip irrigation\n"
      "• 🕓 Water early morning or evening\n"
      "• 🌱 Avoid water loss in noon sun\n"
      "• 🧪 Check soil dryness manually\n\n";
  }

  // 🌡 TEMPERATURE (NO QUESTION)
  if (temp > 40) {
    alerts += "🔥 High Temperature Detected\n";
    actions +=
      "• 🌤 Use temporary shade nets\n"
      "• 💦 Increase irrigation gap\n"
      "• 🌬 Allow natural airflow\n"
      "• 🌾 Protect young crops\n"
      "• 🕓 Avoid fertilizer spray now\n\n";
  }

  // 💦 HUMIDITY (NO QUESTION)
  if (hum < 30) {
    alerts += "💦 Low Humidity Detected\n";
    actions +=
      "• 💧 Light irrigation recommended\n"
      "• 🌾 Apply organic mulch\n"
      "• 🌬 Reduce dry wind exposure\n"
      "• 🌱 Protect seedlings\n"
      "• 🕓 Monitor evening conditions\n\n";
  }

  // 🚨 GAS (OPEN FIELD)
  if (gas) {
    alerts += "🚨 Methane / Hydrogen Gas Detected\n";
    actions +=
      "• 🔥 Avoid open flames nearby\n"
      "• 🚜 Stop machinery temporarily\n"
      "• 🌬 Allow gas to disperse naturally\n"
      "• 🧯 Keep fire safety ready\n"
      "• 📞 Inform field supervisor\n\n";
  }

  // 💨 SMOKE
  if (smoke) {
    alerts += "💨 Smoke Detected in Field\n";
    actions +=
      "• 🔥 Check for crop burning nearby\n"
      "• 🚜 Stop diesel engines\n"
      "• 🌬 Wind direction observation\n"
      "• 🚒 Prepare fire control\n"
      "• 📞 Alert nearby farmers\n\n";
  }

  // -------- SEND EMAIL --------
  if (alerts != "") {
    String emailBody =
      "📊 Open Field Farm Status\n\n"
      "🌡 Temperature : " + String(temp) + " C\n"
      "💦 Humidity    : " + String(hum) + " %\n"
      "🌱 Moisture    : " + String(soil) + " %\n\n"
      "⚠️ ALERTS\n"
      + alerts +
      "\n✅ Recommended Actions\n"
      + actions +
      "— AgriCare Open Field System 🌾";

    sendEmail("🚜 Open Field Farming Alert", emailBody);
  }

  delay(60000); // spam control
}
