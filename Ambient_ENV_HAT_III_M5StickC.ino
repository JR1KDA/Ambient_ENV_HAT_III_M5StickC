
/*
   M5StickC IoT send and display temp, humdi, presu

   by Shoji Iwasaki
   Sep 15, 2021

*/


#include <M5StickC.h>
#include <WiFi.h>
#include <Ambient.h>  // Ambientのヘッダファイルをインクルード
#include <Wire.h>
#include "UNIT_ENV.h"

SHT3X sht30;  // SHT30 object
QMP6988 qmp6988;  // QMP6988 object

char draw_string[1024];  // buffer for Temperature, Humidity, Barometric Pressure

const char* ssid = "AG20R-9C41F8";
const char* password = "j2nrc5xnkc";

WiFiClient client;
Ambient ambient;  // Ambientオブジェクトを作る

unsigned int channelId = 41449; // AmbientのチャネルID
const char* writeKey = "0522b33a0ff129fe"; // ライトキー

void setup(void) {
  M5.begin();

  WiFi.begin(ssid, password);  // Wi-Fi APに接続する
  while (WiFi.status() != WL_CONNECTED) {  //  Wi-Fi AP接続待ち
    delay(500);
    M5.Lcd.print(".");
  }
  M5.Lcd.print("\r\nWiFi connected\r\nIP address: ");
  M5.Lcd.println(WiFi.localIP());

  ambient.begin(channelId, writeKey, &client); // チャネルIDとライトキーを指定してAmbientを初期化

  Wire.begin(0, 26); // For pin socket
  qmp6988.init();

  if (sht30.get() != 0) {
    M5.Lcd.println("Wire read error!");
    while (true) {
      delay(0);
    }
  }
  M5.Lcd.setRotation(3);  //
  M5.Lcd.fillScreen(BLACK);  //

  M5.Lcd.setTextSize(1);  //
  M5.Lcd.setCursor(40, 0, 2);  //
  M5.Lcd.println("ENV HAT");  //
}

void loop(void) {
  if (sht30.get() != 0) {
    return;
  }

  float temp = sht30.cTemp;  // get temp
  float humid = sht30.humidity;  // get humidity
  float presu = qmp6988.calcPressure() / 100;  // get pressure

  M5.Lcd.setCursor(0, 15);
  M5.Lcd.printf("Temp: %2.2f *C\r\n", temp);
  M5.Lcd.printf("Humi: %0.2f %%\r\n", humid);
  M5.Lcd.printf("Pres: %0.2f Pa\r\n", presu);

  ambient.set(1, temp);  // Ambientのデータ1に温度をセットする
  ambient.set(2, humid);  // データ2に湿度をセットする
  ambient.set(3, presu);  // set presu to data 3
  ambient.send();  // Ambientに送信する

  delay(60 * 1000);
}
