#ifndef InfluxDBModule_h
#define InfluxDBModule_h
#include <InfluxDbClient.h>
#include <InfluxDbCloud.h>

class InfluxDBModule {
    public:
    void checkInfluxDB(InfluxDBClient &client);
    void setField(String field, float value, Point& sensor);
    Point setDevice(String device, Point& sensor,  String place);
    void senDataInfluxDB(InfluxDBClient &client, Point& sensor);
   



};

#endif