#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "Galaxy A12 F315";
const char* password = "Lequochuy";


const char* mqtt_server = "broker.emqx.io";

const int lockPin = 2; 

WiFiClient espClient;
PubSubClient client(espClient);

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
      Serial.println("=> [ACTION] MỞ CỬA! (Bật Relay)");
      digitalWrite(lockPin, HIGH);
      
      // Giữ cửa mở trong 3 giây
      delay(3000); 
      
      Serial.println("=> [ACTION] ĐÓNG CỬA! (Tắt Relay)");
      digitalWrite(lockPin, LOW); 
    }
  }
}

// Hàm tự động kết nối lại nếu rớt mạng MQTT
void reconnect() {
  while (!client.connected()) {
    Serial.print("Đang kết nối tới MQTT Broker... ");
    // Tạo một cái tên định danh cho ESP32
    if (client.connect("ESP32_SmartLock_Client")) {
      Serial.println("THÀNH CÔNG!");
      // Đăng ký nhận tin nhắn từ kênh của cửa chính
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
  Serial.begin(115200); // Khởi tạo Serial Monitor với tốc độ 115200
  
  pinMode(lockPin, OUTPUT);
  digitalWrite(lockPin, LOW); // Mặc định vừa bật lên là khóa cửa

  setup_wifi();
  
  // Thiết lập MQTT
  client.setServer(mqtt_server, 1883); // Cổng mặc định của MQTT là 1883
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop(); // Lệnh này giúp ESP32 luôn lắng nghe tín hiệu
}