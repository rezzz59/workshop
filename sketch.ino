  #include <WiFi.h>                                                                                                                                                                                                                     
    #include <HTTPClient.h>                                                                                                                                                                                                               
    #include <SPI.h>                                                                                                                                                                                                                      
    #include <MFRC522.h>                                                                                                                                                                                                                  
                                                                                                                                                                                                                                          
    // Konfigurasi pin RFID                                                                                                                                                                                                               
    #define RST_PIN 27                                                                                                                                                                                                                    
    #define SS_PIN  23                                                                                                                                                                                                                    
    #define SCK_PIN 5                                                                                                                                                                                                                     
    #define MISO_PIN 18                                                                                                                                                                                                                   
    #define MOSI_PIN 19                                                                                                                                                                                                                   
                                                                                                                                                                                                                                          
    // Konfigurasi pin LED                                                                                                                                                                                                                
    #define LED_PIN 4                                                                                                                                                                                                                     
                                                                                                                                                                                                                                          
    MFRC522 mfrc522(SS_PIN, RST_PIN);                                                                                                                                                                                                     
                                                                                                                                                                                                                                          
    // ==========================================================                                                                                                                                                                         
    // GANTI DENGAN LINK WEB ANDA (Vercel atau Hostinger/api.php)                                                                                                                                                                         
    // ==========================================================                                                                                                                                                                         
    const char* serverName = "https://rfid-web-khaki.vercel.app/api/rfid";                                                                                                                                                                      
                                                                                                                                                                                                                                          
    void setup() {                                                                                                                                                                                                                        
      Serial.begin(115200);                                                                                                                                                                                                               
                                                                                                                                                                                                                                          
      // Mengatur Pin LED sebagai Output dan pastikan dalam kondisi mati (LOW)                                                                                                                                                            
      pinMode(LED_PIN, OUTPUT);                                                                                                                                                                                                           
      digitalWrite(LED_PIN, LOW);                                                                                                                                                                                                         
                                                                                                                                                                                                                                          
      SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN, SS_PIN);                                                                                                                                                                                     
      mfrc522.PCD_Init();                                                                                                                                                                                                                 
                                                                                                                                                                                                                                          
      Serial.println("Mesin RFID Siap!");                                                                                                                                                                                                 
      Serial.print("Menghubungkan ke Wokwi-GUEST");                                                                                                                                                                                       
      WiFi.begin("Wokwi-GUEST", "");                                                                                                                                                                                                      
                                                                                                                                                                                                                                          
      while (WiFi.status() != WL_CONNECTED) {                                                                                                                                                                                             
        delay(500);                                                                                                                                                                                                                       
        Serial.print(".");                                                                                                                                                                                                                
      }                                                                                                                                                                                                                                   
      Serial.println("\nWiFi Berhasil Terhubung!");                                                                                                                                                                                       
    }                                                                                                                                                                                                                                     
                                                                                                                                                                                                                                          
    void loop() {                                                                                                                                                                                                                         
      // Jika ada kartu ditempel                                                                                                                                                                                                          
      if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {                                                                                                                                                             
                                                                                                                                                                                                                                          
        // NYALAKAN LED KETIKA KARTU TERDETEKSI                                                                                                                                                                                           
        digitalWrite(LED_PIN, HIGH);
        
        String rfidUid = "";
        // Konversi data kartu menjadi tulisan
        for (byte i = 0; i < mfrc522.uid.size; i++) {
          rfidUid += String(mfrc522.uid.uidByte[i] < 0x10 ? "0" : "");
          rfidUid += String(mfrc522.uid.uidByte[i], HEX);
        }
        rfidUid.toUpperCase();
        Serial.println("Kartu Terbaca: " + rfidUid);
  
        // Kirim data ke internet
        if(WiFi.status() == WL_CONNECTED){
          HTTPClient http;
          http.begin(serverName);
          
          http.addHeader("Content-Type", "application/json"); 
          http.setUserAgent("Mozilla/5.0 (Windows NT 10.0; Win64; x64)");
          http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
  
          String jsonPayload = "{\"uid\": \"" + rfidUid + "\"}";
          Serial.println("Mengirim data ke Server...");
          
          int httpResponseCode = http.POST(jsonPayload); 
  
          if (httpResponseCode > 0) {
            Serial.print("BERHASIL! (Kode HTTP: ");
            Serial.print(httpResponseCode);
            Serial.println(")");
          } else {
            Serial.print("GAGAL. Error: ");
            Serial.println(http.errorToString(httpResponseCode).c_str());
          }
          
          http.end(); 
        } else {
          Serial.println("Internet terputus!");
        }
  
        // MATIKAN KEMBALI LED SETELAH PROSES SELESAI
        digitalWrite(LED_PIN, LOW);
  
        // Jeda agar kartu tidak terbaca ganda
        mfrc522.PICC_HaltA(); 
        delay(1000); 
      }
    }
