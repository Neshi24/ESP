void WiFiConnection()
{
  const char* ssid = "Neshi";
  const char* password =  "walsiena";
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(2500);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");
    lcd.backlight();
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Wifi Connected");
}