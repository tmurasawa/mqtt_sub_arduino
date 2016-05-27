#include <PubSubClient.h>
#include <Ethernet.h>
#include <SPI.h>


// Enter a MAC address for your controller below.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
// if you don't want to use DNS (and reduce your sketch size)
// use the numeric IP instead of the name for the server:
//IPAddress server(74,125,232,128);  // numeric IP for Google (no DNS)

// dtostrf buffer
//char s[16];
//char t[16];

//
int ledPin = 2;

// Set the static IP address to use if the DHCP fails to assign
IPAddress ip(192, 168, 0, 9);

// Initialize the Ethernet client library
// with the IP address and port of the server
// that you want to connect to (port 80 is default for HTTP):
EthernetClient Ethclient;

// scalenics
#define SC_USER "YOUR_SCALENICS_ID"
#define DEVICE_TOKEN "YOUR_DEVICE_TOKEN"
#define DEVICE_ID "arduino01"
#define MQTT_SERVER "api.scalenics.io"
// MQTT client
//PubSubClient client2(Ethclient);
PubSubClient client2(MQTT_SERVER, 1883, callback, Ethclient);

String topic;
//String PostData;
char mqtt_topic[128];
char mqtt_payload[64];
String msg;

// callback
void callback(char* topic, byte* payload, unsigned int length) {
  // handle message arrived
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  msg = "";
  for (int i = 0; i < length; i++) {
    msg += String((char)payload[i]); 
    //Serial.print((char)payload[i]);
  }
  Serial.print("msg=");
  Serial.print(msg);
  Serial.println();
  if (msg == "led:on"){
    Serial.print("led is on!!");
    Serial.println();
    digitalWrite(ledPin, HIGH);
  }
  if (msg == "led:off"){
    Serial.print("led is off!!");
    Serial.println(); 
    digitalWrite(ledPin, LOW);
  }
}


void setup() {
  //
  pinMode(ledPin, OUTPUT);
  
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
    //
    client2.setServer(MQTT_SERVER, 1883);
    client2.setCallback(callback);

  }

  // start the Ethernet connection:
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // no point in carrying on, so do nothing forevermore:
    Ethernet.begin(mac, ip);
  }
  // print your local IP address:
  Serial.print("My IP address: ");
  for (byte thisByte = 0; thisByte < 4; thisByte++) {
    // print the value of each byte of the IP address:
    Serial.print(Ethernet.localIP()[thisByte], DEC);
    Serial.print(".");
  }
  Serial.println();
}

void reconnect() {
  while (!client2.connected()) {
    Serial.print("Attempting MQTT connection...");
    // give the Ethernet shield a second to initialize:
    // delay(1000);
    Serial.println("connecting...");

    // make topic (DEVICE_TOKEN/DEVICE_ID)
    topic = DEVICE_TOKEN;
    topic += "/";
    topic += DEVICE_ID;
    topic += "/subscribe";

    // if you get a connection, report back via serial:
    if (client2.connect(DEVICE_ID, SC_USER, DEVICE_TOKEN)) {
      topic.toCharArray(mqtt_topic, topic.length() + 1);
      //PostData.toCharArray(mqtt_payload, PostData.length() + 1);
      Serial.println("mqtt_topic=");
      Serial.println(mqtt_topic);
      //Serial.println("mqtt_payload=");
      //Serial.println(mqtt_payload);
      Serial.println("Connect to MQTT server..");
      Serial.println("DEVICE_TOKEN=");
      Serial.println(DEVICE_TOKEN);
      Serial.println("DEVICE_ID=");
      Serial.println(DEVICE_ID);

      //client2.publish(mqtt_topic, mqtt_payload);
      client2.subscribe(mqtt_topic);
    } else {
      Serial.print("Connect failed..RC=");
      Serial.print(client2.state());
      Serial.println("Retry in 5sec..");
      delay(5000);
    }
  }
}

void loop()
{
  if (!client2.connected()) {
    reconnect();
  }
  client2.loop();

}
