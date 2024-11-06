#include <WiFi.h>
#include <WiFiClientSecure.h>  // Ensure this library is included
#include <PubSubClient.h>

// WiFi credentials
const char* ssid = "Vandanam-2.4G";
const char* password = "VAndanam98";

// AWS IoT credentials
const char* mqtt_server = "a1l05t2okb00j5-ats.iot.eu-north-1.amazonaws.com";
const int mqtt_port = 8883;
const char* mqtt_topic = "esp32/ultrasonic";
const char* subscribe_topic = "esp32/commands";

// AWS IoT certificates
const char* ca_cert = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF
ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6
b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL
MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv
b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj
ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM
9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw
IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6
VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L
93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm
jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC
AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA
A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI
U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs
N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv
o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU
5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy
rqXRfboQnoZsG4q5WTP468SQvvG5
-----END CERTIFICATE-----
)EOF";

const char* client_cert = R"KEY(
-----BEGIN CERTIFICATE-----
MIIDWjCCAkKgAwIBAgIVAPXfo8VHU24Zr7291xdpIW/a97uMMA0GCSqGSIb3DQEB
CwUAME0xSzBJBgNVBAsMQkFtYXpvbiBXZWIgU2VydmljZXMgTz1BbWF6b24uY29t
IEluYy4gTD1TZWF0dGxlIFNUPVdhc2hpbmd0b24gQz1VUzAeFw0yNDExMDQwODA2
MjZaFw00OTEyMzEyMzU5NTlaMB4xHDAaBgNVBAMME0FXUyBJb1QgQ2VydGlmaWNh
dGUwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQC8CnqQ8dFkLYUxWVdh
Hteq4n7NCRHQXNbehOu/ntmTRrKMBaA/+WWanGFEqHSqLrNWapip9SikE4FBOEG8
ddxKHtFm5iDvPhH4xmYUnoP4Kbkr6As+Wn2qVhZcyaRr+RQvgK5wTXNFMTONOZxD
u3cuZPD5sIj85IhOhCS51q9unjqckDSHrtAahl8K00VC+20zuaBcinpzkBaROVz4
oteD3IQs4lcgjrxRjjOCSu7F4EulMEtGDZ1CAayg2TgJbsEuRblLhEAw9PpKcNTc
OdFxqHaPFhXpFKib5jbw2R3XP6Vm3ehEzm7lJsBh4YhWXb8SoCb/RemIHb016VZh
KthXAgMBAAGjYDBeMB8GA1UdIwQYMBaAFPctdYRcwLvL0q99jKcANsVVXhl3MB0G
A1UdDgQWBBR5BzlmQZQ66Dar1+PgxmK7RdaUNTAMBgNVHRMBAf8EAjAAMA4GA1Ud
DwEB/wQEAwIHgDANBgkqhkiG9w0BAQsFAAOCAQEAmh7uyUknwmZs/2wvE/v/i4Mh
FnJsoaIw+Y/9WH5MlVN6SSo3c31cdLrxhp2VEuZiN+M97RZuPAsQYjokG9qwPF67
jGH88KPLJs2A2ZFVwb9mwb2QZNgZQ/+/bTVaolSa+n8KU/15JNgXgHs80eIwoY5e
4YvnFDuUWcUeduhJCTVN8WS/a/RQJ0xLsaPgz9pXTBGXCmNBe+PklwyFEFL9YlcT
Q18iSehj+7HPz/apo0+49hd/T5y/gnefOUfoPvs/KksNBocWWNHn6ukbtptPV+6P
Dg3T8eGTMK4qtlx/MVTN6g+dRsnDz74pHldzhivcBkGIwyC+Gz07wfk6/5CKMQ==
-----END CERTIFICATE-----
)KEY";

const char* client_key = R"KEY(
-----BEGIN RSA PRIVATE KEY-----
MIIEpAIBAAKCAQEAvAp6kPHRZC2FMVlXYR7XquJ+zQkR0FzW3oTrv57Zk0ayjAWg
P/llmpxhRKh0qi6zVmqYqfUopBOBQThBvHXcSh7RZuYg7z4R+MZmFJ6D+Cm5K+gL
Plp9qlYWXMmka/kUL4CucE1zRTEzjTmcQ7t3LmTw+bCI/OSIToQkudavbp46nJA0
h67QGoZfCtNFQvttM7mgXIp6c5AWkTlc+KLXg9yELOJXII68UY4zgkruxeBLpTBL
Rg2dQgGsoNk4CW7BLkW5S4RAMPT6SnDU3DnRcah2jxYV6RSom+Y28Nkd1z+lZt3o
RM5u5SbAYeGIVl2/EqAm/0XpiB29NelWYSrYVwIDAQABAoIBAHNdunTS57SeeOkd
9w4O7iJZc5I3zkYXr3w18K1PuA0zgreFfBvD1yJUbxjAgVMn879BBwi+8IA5t0Nc
UHhflxeicLbA2dPuMPiAQIDGABFxB7p4Uu2edxraVj33OHlGB6xdNqiP2/jHhulO
PyEPEr/UDkQimukNDNOfHyklHdyvCItVTLVxjx1RCQZAafesZpSwO0LWuQSBN10t
szO0jzJvTorIAc+c6CDb3JafwFlLpE9p0F8tEISoRbpOYkqeHd9uHqQDGpsNTOqA
cp84zZCPWH0ttrFP8MY/5DSeEuol0xELxXwMhrczLmjLvDN8F8FCnYtQoPi7YrXO
eOKF4wECgYEA8tMhTzkEsyr+j1w49uzFKo5jFKY1eYKSN1OpwqU5M0tLcl1tmaCj
S6V55r3XNZRckZZzWLwFv/HnAkzwZ/LHsXBjfxT7dMnOc/7fuV7ncEKTXqeBPtIw
fs4YkqU2BoJHkWfaQw36mf/VV9gZeHzSfF1avUeYz45nzvHUV0Gpyb8CgYEAxj5k
3HpgvMb8NWdv7TPxaDqi8uMkcBy7EkmmuZdPi1HxIIRwE23/qbvYAkjOCbcXjkjq
/oCsy19uPBZ0mbv8Rdm4ROVVJ7WKToXAZLyCr8Slyx3WucHJj9apaUirF0eRgzHa
2w3CzTtV2ty9vb9PfjeZXJbitXjEHqWQeL/1J2kCgYEA3bZ3IH3gU7N7EOzUumj/
sdamkxEbnGHjHY9PXoE71CD4t2ilDPPGheiGgG9wE+TuDKSBl1C+Mz8tfUVA8LiV
KueCgWNoIX+t+O2L2xbTXQX8xx1Mfk6upumJOESPKqxDqnlXqYgW/AvDAV04DHJ0
Qcd/dXntBf5xvFvEVROIcsMCgYEAsVIF7aAY/0INPcdzufwIJm6sagz5/r30Jnty
gfbx03BL+gX8KaTuVegLYp+0i8QVcdOPPm3iIVcOxvhe+1PGZsRDqnxnGsmrGGlS
mch0LLv8gB4Efsrze5FKnz28q2n8ohjp5Z39bI1yUf24SgzhEcixiLv9tMx8v9zf
yOIKamECgYAYfS5/LqYYQcELvrN0d9NcuUqX87B/kfMEhGwvPsM5g2JdxbtPqqaW
vuj9El3ar9addCW5OppN1aqbRQYV79DUkS7eldGngAmtqjt9hhhaDlZoT5wYi8ht
I+vB8E483QeGmyaVYKirvuhxQi1Bvt5JBhsgYn6zQAGWWjalS4pGOg==
-----END RSA PRIVATE KEY-----
)KEY";

// Pin Definitions
#define TRIG_PIN 5
#define ECHO_PIN 18

WiFiClientSecure espClient;    // Secure client
PubSubClient client(espClient); // MQTT client with secure WiFi client

// Function to connect to WiFi
void setupWiFi() {
    delay(10);
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print(".");
    }
    Serial.println("Connected to WiFi");
}

// Callback function for receiving messages from AWS IoT
void mqttCallback(char* topic, byte* payload, unsigned int length) {
    Serial.print("Message received on topic: ");
    Serial.println(topic);
    Serial.print("Message: ");
    for (int i = 0; i < length; i++) {
        Serial.print((char)payload[i]);
    }
    Serial.println();
}

// Function to connect to AWS IoT
void setupAWSIoT() {
    espClient.setCACert(ca_cert);
    espClient.setCertificate(client_cert);
    espClient.setPrivateKey(client_key);
    client.setServer(mqtt_server, mqtt_port);
    client.setCallback(mqttCallback);

    while (!client.connected()) {
        Serial.println("Connecting to AWS IoT...");
        if (client.connect("ESP32Client")) {
            Serial.println("Connected to AWS IoT");
            client.subscribe(subscribe_topic);
        } else {
            Serial.print("Failed, rc=");
            Serial.print(client.state());
            Serial.println(" try again in 5 seconds");
            delay(5000);
        }
    }
}

// Function to measure distance using the ultrasonic sensor
long measureDistance() {
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);

    long duration = pulseIn(ECHO_PIN, HIGH);
    long distance = duration * 0.034 / 2; // Convert time to distance
    return distance;
}

void setup() {
    Serial.begin(115200);
    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);

    setupWiFi();
    setupAWSIoT();
}

void loop() {
    if (!client.connected()) {
        setupAWSIoT();
    }
    client.loop();

    // Measure distance
    long distance = measureDistance();
    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.println(" cm");

    // Publish distance to AWS IoT
    String distanceStr = "Distance: "+ String(distance)+ "cm";
    client.publish(mqtt_topic, distanceStr.c_str());
    
    delay(2000); // Wait 2 seconds before the next reading
}
