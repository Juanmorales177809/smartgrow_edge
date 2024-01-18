#include <InfluxDBModule.h>
#include <Arduino.h>
#include <InfluxDbClient.h>
#include <InfluxDbCloud.h>

void InfluxDBModule::checkInfluxDB(InfluxDBClient &client)
{
    if (client.validateConnection()) {
      Serial.print("Connected to InfluxDB: ");
      Serial.println(client.getServerUrl());
    } else {
      Serial.print("InfluxDB connection failed: ");
      Serial.println(client.getLastErrorMessage());
    }
}
void InfluxDBModule::setField(String field, float value, Point& sensor){
    sensor.clearFields();
    sensor.addField(field, value);
}
Point InfluxDBModule::setDevice(String device, Point& sensor, String place){
    sensor.addTag("device", device);
    sensor.addTag("SSID", place);
    return sensor;
}
void InfluxDBModule::senDataInfluxDB(InfluxDBClient &client, Point& sensor){
    if (!client.writePoint(sensor)) {
      Serial.print("InfluxDB write failed: ");
      Serial.println(client.getLastErrorMessage());
    }
    else{
        Serial.println("InfluxDB write success");
    }

}