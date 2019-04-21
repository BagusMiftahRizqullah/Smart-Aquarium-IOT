/*******************************************************************
*  Aldi Pratama                *
*  Miftah Bagus                                                               
*  Putra Eryawan
*  Gemara Adhiyasa                                                                                                                                                                      
 *******************************************************************/
 
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>  
#include <Servo.h> // menyertakan library servo ke dalam program 
#include <NewPing.h>

int numNewMessages;
// Initialize Wifi connection to the router
char ssid[] = "senyumceria";     // your network SSID (name)
char password[] = "gustiallah"; // your network key

// Initialize Telegram BOT
#define BOTtoken "737904081:AAGQzlh9DTPCsPQJJGRWp1s52CBmeWx2ViQ"  // your Bot Token (Get from Botfather)

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

int Bot_mtbs = 1000; //mean time between scan messages
long Bot_lasttime;   //last time messages' scan has been done
bool Start = false;

//--------------------------------------------------
bool kuras_status=false;
bool isi_status=false;
float jarak;
float batas_atas=6;
float batas_bawah=15;
String chat_id1;
//--------------------------------------------------


//------------------ULTRASONIK----------------------
const int PIN_TRIG =14;
const int PIN_ECHO=12;
NewPing sonar(PIN_TRIG, PIN_ECHO, 200);
//------------------ULTRASONIK----------------------

//----------------------RELAY----------------------
int relay1 = 5; //D1
int relay2 = 4; //D2
//----------------------RELAY---------------------

//----------------------SERVO----------------------
Servo myservo;     // variable untuk menyimpan posisi data
//----------------------SERVO----------------------

void handleNewMessages(int numNewMessages) {
  Serial.println("handleNewMessages");
  Serial.println(String(numNewMessages));

  for (int i=0; i<numNewMessages; i++) {
    String chat_id = String(bot.messages[i].chat_id);
    String text = bot.messages[i].text;

    String from_name = bot.messages[i].from_name;
    
    if (text == "/send_test_action") {
        bot.sendChatAction(chat_id, "typing");
        delay(4000);
        bot.sendMessage(chat_id, "Did you see the action message?");
      
    }
    if (text == "/start") {
      String welcome = "Selamat Datang di Alat ARUM , " + from_name + ".\n";
      welcome += "ARUM dapat melakukan sesuatu.\n\n";
      welcome += "/isi_air : Mengisi Air Akuarium\n";
      welcome += "/kuras_air : Menguras Air Akuarium\n";
      welcome += "/makan_ikan : Memberi Pakan Ikan\n";
      welcome += "/get_ping : Ukur Jarak Ultrasonik\n";
      bot.sendMessage(chat_id, welcome);
    }

    if (text == "/isi_air"){
      String welcome = "Sedang Mengisi Air... \n";
      welcome += "Harap tunggu Sebentar";
      bot.sendMessage(chat_id, welcome);
      isi_air(chat_id);
    }

    if (text == "/kuras_air"){
      String welcome = "Sedang Menguras Air...\n";
      welcome += "Harap tunggu Sebentar";
      bot.sendMessage(chat_id, welcome);
      kuras_air(chat_id);
    }

    if (text == "/makan_ikan"){
      String welcome = "Memberi Makan Ikan...\n";
      bot.sendMessage(chat_id, welcome);
      beri_makan(chat_id);
      
    }

    if (text == "/get_ping"){
      get_ping(chat_id);
      
    }

    
  }
}

void kuras_air(String chat_id){
  chat_id1=chat_id;
  kuras_status=true;
}

void isi_air(String chat_id){
  chat_id1=chat_id;
  isi_status=true;
}

void beri_makan(String chat_id){
   myservo.write(0);
   myservo.write(30);
   delay(100);
   myservo.write(0);
   delay(1000);
   myservo.write(30);
   delay(100);
   myservo.write(0);
   bot.sendMessage(chat_id, "Selesai Memberikan Makan..");
}

void get_ping(String chat_id){
  jarak = sonar.ping_cm();
  Serial.print(jarak);
  Serial.println(" cm");
  String pesan = "Ping Sensor : " + String(jarak) + " cm";
  bot.sendMessage(chat_id, pesan);
}

void setup() {
  //-------------------------------------------------
    kuras_status = false;
    isi_status = false;
  //-------------------------------------------------
    Serial.begin(9600);
  //----------------------RELAY----------------------
   pinMode(relay1, OUTPUT);     //Set Pin12 as output 
   pinMode(relay2, OUTPUT);
   digitalWrite(relay1, HIGH);   //Turn off relay
   digitalWrite(relay2, HIGH);   //Turn off relay
  //----------------------RELAY----------------------
  //----------------------SERVO----------------------
    myservo.attach(16); //sinyal data kabel motor servo dikonekan di pin 3 Arduino
    myservo.write(0);
  //----------------------SERVO----------------------
  

  // Set WiFi to station mode and disconnect from an AP if it was Previously
  // connected
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  // attempt to connect to Wifi network:
  Serial.print("Connecting Wifi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  if (millis() > Bot_lasttime + Bot_mtbs)  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    Serial.println(numNewMessages);
    while(numNewMessages) {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    Bot_lasttime = millis();
  }
  //------------coding logika---------------
  if(isi_status){
    jarak = sonar.ping_cm();
    if(jarak <= batas_atas)
    {
      digitalWrite(relay2, HIGH);   //Turn off relay
      delay(50);      
      bot.sendMessage(chat_id1, "Selesai Mengisi Air");
      isi_status=false;
    }
    else if(jarak >= batas_atas)
      digitalWrite(relay2, LOW);   //Turn on relay
      delay(50);
    }
  

  if(kuras_status){
    jarak = sonar.ping_cm();
    if(jarak >= batas_bawah)
    {
      digitalWrite(relay1, HIGH);   //Turn off relay
      delay(50);
      bot.sendMessage(chat_id1, "Selesai Menguras Air");
      kuras_status=false;
    }
    else if(jarak <= batas_bawah) 
    {
      digitalWrite(relay1, LOW);   //Turn on relay
      delay(50);
    }
  }


  //------------coding logika---------------
    
  delay(500);
}
