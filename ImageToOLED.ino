#include "esp_camera.h"
#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

#define i2c_add 0x3C // OLED address

#define I2C_SDA 15  // Connect GPIO 15 to SDA of OLED
#define I2C_SCK 14 // Connect GPIO 14 to SCK of OLED

// Initializing I2C BUS as ESP32 Cam doesn't have default I2C BUS
TwoWire I2C_BUS = TwoWire(0);

// Camera pins for AI Thinker ESP32-CAM
#define PWDN_GPIO_NUM 32
#define RESET_GPIO_NUM -1
#define XCLK_GPIO_NUM 0
#define SIOD_GPIO_NUM 26
#define SIOC_GPIO_NUM 27
#define Y9_GPIO_NUM 35
#define Y8_GPIO_NUM 34
#define Y7_GPIO_NUM 39
#define Y6_GPIO_NUM 36
#define Y5_GPIO_NUM 21
#define Y4_GPIO_NUM 19
#define Y3_GPIO_NUM 18
#define Y2_GPIO_NUM 5
#define VSYNC_GPIO_NUM 25
#define HREF_GPIO_NUM 23
#define PCLK_GPIO_NUM 22

// Define click button
#define click 12

// OLED SCREEN WIDTH AND HEIGHT
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// DEFINE AN INSTANCE OF WEBSERVER
WebServer server(80);

// DEFINE AN INSTANCE OF DISPLAY
Adafruit_SH1106G display(128, 64, &I2C_BUS, -1);

void setup() {
  // Initialize serial communications
  Serial.begin(115200);

  pinMode(click, INPUT); // CLICK BUTTON

  WiFi.mode(WIFI_STA); // WIFI STATION MODE
  WiFi.begin("WIFI_SSID", "WIFI_PASSWORD"); // CONNECT TO YOUR WIFI

  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("\nWiFi Connected!");
  Serial.print("ESP32-CAM IP Address: ");
  Serial.println(WiFi.localIP());

  I2C_BUS.begin(I2C_SDA, I2C_SCK, 100000); // CONFIGURE I2C BUS

  display.begin(i2c_add, true); // INITIALIZING DISPLAY
  display.display(); // ADAFRUIT SPLASH SCREEN FOR 2 MINUTES
  delay(2000);
  display.clearDisplay(); // CLEAR DISPLAY
  // display.println("Click!");
  // display.display();

  server.on("/capture", HTTP_GET, handleCapture); // FOR CHECKING OUTPUT IMAGE IN WEB-BROWSER 
  server.begin();

  // Camera configuration
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_GRAYSCALE;  //PIXFORMAT_JPEG; // PIXFORMAT_GRAYSCALE;
  config.frame_size = FRAMESIZE_QQVGA;        // 160x120 resolution
  config.jpeg_quality = 10;
  config.fb_count = 1;

  // Initialize camera
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }
}

// FUNCTION FOR DRAWING IMAGE IN OLED
void drawToOLED() {
  camera_fb_t *fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Camera capture failed");
    display.println("Camera capture failed!");
    display.display();
    return;
  }

  display.clearDisplay();

  // Calculate scaling factors
  float scale_x = (float)SCREEN_WIDTH / 160;
  float scale_y = (float)SCREEN_HEIGHT / 120;

  // Draw scaled image
  for (int y = 0; y < SCREEN_HEIGHT; y++) {
    for (int x = 0; x < SCREEN_WIDTH; x++) {
      int src_x = (int)(x / scale_x);
      int src_y = (int)(y / scale_y);
      int src_pos = src_y * 160 + src_x;

      if (src_pos < fb->len) {
        uint8_t pixel = fb->buf[src_pos];
        // Convert grayscale to binary for OLED
        if (pixel > 128) {
          display.drawPixel(x, y, SH110X_WHITE);
        }
      }
    }
  }

  display.display();

  esp_camera_fb_return(fb);
}

// WHEN REQUESTING FOR ACTUAL IMAGE THROUGH WEBSERVER
void handleCapture() {
  Serial.println("GET: Capture");
  camera_fb_t *fb = esp_camera_fb_get();
  if (!fb) {
    server.send(500, "text/plain", "Camera capture failed");
    return;
  }

  Serial.printf("Captured image size: %d bytes\n", fb->len);
  // SENDING RAW GRAYSCALE BINARY DATA
  // FOR FINE OUTPUT YOU NEED TO PROCESS BINARY TO GRAYSCALE DATA AT RECEIVING SIDE
  server.send_P(200, "application/octet-stream", (const char *)fb->buf, fb->len);
  esp_camera_fb_return(fb);
}


void loop() {
  int isClicked = digitalRead(click);
  if (isClicked) {
    drawToOLED();
  }
  server.handleClient();
}