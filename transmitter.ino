#include <Ethernet.h>
#include <SPI.h>
#include <Arduino.h>
#include "WebServer.h"

unsigned long valueold;


char buf[60]; //буфер для отправки данных. 

byte ip[] = { 192, 168, 1, 10 }; // локальный адрес
byte rserver[] = { 192, 168, 1, 20 };  // ip-адрес удалённого сервера


byte mac[] = { 0xDE, 0xAD, 0xBE, 0x54, 0xDE, 0x33 }; // MAC-адрес нашего устройства
byte subnet[] = { 255, 255, 255, 0 };
byte gateway[] = { 192, 168, 1,1 };
byte dns_server[] = { 192, 168, 1, 1};

#define VERSION_STRING "Radio server"

P(Page_info) = "<html><head><title>controller " VERSION_STRING "</title></head><body>\n";
P(location_info) = "server room";
P(version_info) = VERSION_STRING ". Compile date: " __DATE__ " " __TIME__;

String url = String(25);


#define MAX_COMMAND_LEN             (10)
#define MAX_PARAMETER_LEN           (10)
#define COMMAND_TABLE_SIZE          (8)
#define PREFIX ""

WebServer webserver(PREFIX, 80);
EthernetClient client;


#define NAMELEN 32
#define VALUELEN 32

char gCommandBuffer[MAX_COMMAND_LEN + 1];
char gParamBuffer[MAX_PARAMETER_LEN + 1];
long gParamValue;

typedef struct {
  char const    *name;
  void          (*function)(WebServer &server);
} command_t;


void infoRequest(WebServer &server, WebServer::ConnectionType type, char *url_tail, bool tail_complete)
{
  server.printP(Page_info);
  server.print("IP:");
  server.print(Ethernet.localIP());
  server.print("<br>Location:");
  server.printP(location_info);
}

void setup() {
  Ethernet.begin(mac, ip, dns_server, gateway, subnet); // Инициализируем Ethernet Shield

  webserver.setDefaultCommand(&infoRequest); // дефолтная страница
  webserver.begin();
  mySwitch.enableReceive(0); // включаем прием на 2 выводе (прерывание 0)
}

bool ledA=0,ledB=0,ledC=0,ledD=0;
long statesend = 0; 

void loop() {
  
  char buff[64];
  int len = 64;
  webserver.processConnection(buff, &len);  // process incoming connections one at a time forever
  
unsigned long currentMillis = millis(); // текущий счетчик

  if (mySwitch.available()) {
unsigned long valuesend = mySwitch.getReceivedValue();

#if 0 // прием команд с радиопультов, выключено ! При использовании не забудьте про установку пина режим на выход..
if ((unsigned int)valuesend==21952){ // A

if (currentMillis-statesend>1000){
   statesend=currentMillis;
    ledA=!ledA;
    sendstate(5,ledA);
}
  
} else 
if ((unsigned int)valuesend==21808){ // B

if (currentMillis-statesend>1000){
   statesend=currentMillis;
    ledB=!ledB;
    sendstate(6,ledB);
}
  
} else 
if ((unsigned int)valuesend==21772){ // C

if (currentMillis-statesend>1000){
   statesend=currentMillis;
    ledC=!ledC;
    sendstate(7,ledC);
}
} else 
if ((unsigned int)valuesend==21763){ // D

if (currentMillis-statesend>1000){
   statesend=currentMillis;
    ledD=!ledD;
    sendstate(8,ledD);
}
  
} else
#endif
 
 // отправка принятых данных с беспроводных датчиков и отправка их на сервер.
  if (valuesend != 0 && valueold !=valuesend) 
 {
   valueold=valuesend;
        if (client.connect(rserver, 80)) {
       sprintf (buf,"GET /server.php?mode=rcskey&n=%hu HTTP/1.0",valuesend);
       
   client.println(buf);
       client.println("Host: local.ru");
    client.println();
  }  //else Serial.println("connection failed");
client.stop();
    
    
  }
  mySwitch.resetAvailable();
  }

}

void sendstate (int pin,int mode){ // управление нагрузками с пульта с отправкой статуса на сервер
  digitalWrite(pin,mode);
  
              if (client.connect(rserver, 80)) {
      sprintf(buf, "GET /server.php?mode=key&n=%i&s=%i  HTTP/1.0", pin, mode);
     client.println(buf);  
       client.println("Host: local.ru"); 
    client.println();
  } 
    client.stop();
}
