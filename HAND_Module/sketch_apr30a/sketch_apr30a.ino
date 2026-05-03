#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "Galaxy A12 F315";
const char* password = "Lequochuy";

const char* mqtt_server = "broker.emqx.io";

const int lockPin = 2; 

WiFiClient espClient;
PubSubClient client(espClient);

unsigned long openTime = 0;   
bool isAutoOpening = false;   

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Đang kết nối WiFi: ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("=> Đã kết nối WiFi!");
  Serial.print("IP của ESP32: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("\n[MQTT] Có tin nhắn từ kênh: ");
  Serial.println(topic);

  String message = "";
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.print("[COMMAND] Lệnh nhận được: ");
  Serial.println(message);

  if (String(topic) == "huy_smartlock/locks/main") {
    
    if (message == "OPEN") {
      Serial.println("=> [ACTION] AI MỞ CỬA! (Sẽ tự đóng sau 3s)");
      digitalWrite(lockPin, HIGH); 
      isAutoOpening = true;        
      openTime = millis();         
    } 
    
    else if (message == "FORCE_OPEN") {
      Serial.println("=> [ACTION] ADMIN MỞ CỬA! (Giữ nguyên mở)");
      digitalWrite(lockPin, HIGH); 
      isAutoOpening = false;       
    }
    

    else if (message == "FORCE_CLOSE") {
      Serial.println("=> [ACTION] ADMIN ĐÓNG CỬA KHẨN CẤP!");
      digitalWrite(lockPin, LOW);  
      isAutoOpening = false;      
    }
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Đang kết nối tới MQTT Broker... ");
    if (client.connect("ESP32_SmartLock_Client")) {
      Serial.println("THÀNH CÔNG!");
      client.subscribe("huy_smartlock/locks/main");
    } else {
      Serial.print("THẤT BẠI, mã lỗi = ");
      Serial.print(client.state());
      Serial.println(" -> Thử lại sau 5 giây...");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200); 
  
  pinMode(lockPin, OUTPUT);
  digitalWrite(lockPin, LOW); 

  setup_wifi();
  
  // Thiết lập MQTT
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop(); 


  if (isAutoOpening && (millis() - openTime >= 3000)) {
    Serial.println("=> [ACTION] TỰ ĐỘNG ĐÓNG CỬA (Hết 3s)!");
    digitalWrite(lockPin, LOW);  
    isAutoOpening = false;       
  }
}