#include <IBMIOTF32.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_I2CDevice.h>
#include <SPI.h>

#define Red_light 21
#define Green_light 17
#define Blue_light 16

bool ledonoff = 0;

String user_html = "";

char*               ssid_pfix = (char*)"iothome";
unsigned long       lastPublishMillis = 0;

void publishData() {
    StaticJsonDocument<512> root;
    JsonObject data = root.createNestedObject("d");

    // YOUR CODE for status reporting
    char buffer[10];

    //sprintf(buffer,"%d",depth);
    data["red"];
    data["blue"]; 
    data["green"]; 
    memset(buffer, '\0', sizeof(buffer));



    serializeJson(root, msgBuffer);
    client.publish(publishTopic, msgBuffer);
}

void handleUserCommand(JsonDocument* root) {
    JsonObject d = (*root)["d"];
    if(d.containsKey("red")) {
        if (strcmp(d["red"], "off")) {
            analogWrite(Red_light, 255);
            Serial.println("red on");
        }else if (strcmp(d["red"], "on")) {   
            analogWrite(Red_light, 0);
            Serial.println("red off"); 
        }
    }
    if (d.containsKey("green")) {
        if (strcmp(d["green"], "off")) {
            analogWrite(Green_light, 255);
            Serial.println("green on");
        }else if (strcmp(d["green"], "on")) {   
            analogWrite(Green_light, 0);
            Serial.println("green off"); 
        }
    }
    if (d.containsKey("blue")) {
        if (strcmp(d["blue"], "off")) {
            analogWrite(Blue_light, 255);
            Serial.println("blue on");
        }else if (strcmp(d["blue"], "on")) {   
            analogWrite(Blue_light, 0);
            Serial.println("blue off"); 
        }  
    }
    
    // YOUR CODE for command handling
}

void message(char* topic, byte* payload, unsigned int payloadLength) {
    byte2buff(msgBuffer, payload, payloadLength);
    StaticJsonDocument<512> root;
    DeserializationError error = deserializeJson(root, String(msgBuffer));

    if (error) {
        Serial.println("handleCommand: payload parse FAILED");
        return;
    }

    handleIOTCommand(topic, &root);
    if (strstr(topic, "/device/update")) {
        JsonObject meta = cfg["meta"];

    // YOUR CODE for meta data synchronization

    } else if (strstr(topic, "/cmd/")) {            // strcmp return 0 if both string matches
        handleUserCommand(&root);
    }
}

void setup() {

    Serial.begin(115200);

    initDevice();
    pinMode(Red_light,OUTPUT);
    pinMode(Green_light,OUTPUT);
    pinMode(Blue_light,OUTPUT);
    

    JsonObject meta = cfg["meta"];
    pubInterval = meta.containsKey("pubInterval") ? atoi((const char*)meta["pubInterval"]) : 0;
    lastPublishMillis = - pubInterval;
    startIOTWatchDog((void*)&lastPublishMillis, (int)(pubInterval * 5));
    // YOUR CODE for initialization of device/environment

    WiFi.mode(WIFI_STA);
    WiFi.begin((const char*)cfg["ssid"], (const char*)cfg["w_pw"]);
    int i = 0;
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
        if(i++ > 10) reboot();
    }
    Serial.printf("\nIP address : "); Serial.println(WiFi.localIP());

    client.setCallback(message);
    set_iot_server();
}

void loop() {
    if (!client.connected()) {
        iot_connect();
    }
    client.loop();
    // YOUR CODE for routine operation in loop

    if ((pubInterval != 0) && (millis() - lastPublishMillis > pubInterval)) {
        publishData();
        lastPublishMillis = millis();
    }
    
    
}