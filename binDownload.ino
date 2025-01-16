#include <WiFi.h>
#include <HTTPClient.h>
#include <Update.h>

const char* ssid = "HELDER  OI FIBRA";
const char* password = "25670980";
const String firmwareUrl = "https://raw.githubusercontent.com/F1reFinger/quick_bin/main/blink.ino.bin";  // Direct download link

void setup() {
  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Start OTA process
  if (beginOTA()) {
    Serial.println("OTA update successful!");
  } else {
    Serial.println("OTA update failed.");
  }
}

bool beginOTA() {
  HTTPClient http;
  http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);  // Handle redirects automatically
  http.begin(firmwareUrl);  // Start HTTP request

  int httpCode = http.GET();  // Make GET request
  if (httpCode == HTTP_CODE_OK) {
    long contentLength = http.getSize();
    WiFiClient* stream = http.getStreamPtr();

    // Start the update process
    if (!Update.begin(contentLength)) {
      Serial.println("Failed to begin OTA update");
      http.end();
      return false;
    }

    // Write data to flash memory
    size_t written = Update.writeStream(*stream);
    if (written == contentLength) {
      Serial.println("Written: " + String(written) + " successfully.");
    } else {
      Serial.println("Written only: " + String(written) + " bytes.");
    }

    // End update and check if it was successful
    if (Update.end()) {
      if (Update.isFinished()) {
        Serial.println("Update successfully completed. Rebooting...");
        ESP.restart();  // Reboot ESP after successful OTA update
      } else {
        Serial.println("Update failed. Not enough space?");
      }
    } else {
      Serial.println("Update failed");
    }
  } else {
    Serial.printf("Failed to download update. HTTP Error: %d\n", httpCode);
  }

  http.end();  // End the HTTP connection
  return false;
}

void loop() {
  // Nothing to do in loop for OTA
}
