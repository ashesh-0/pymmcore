#ifndef _MDAEvent_H_
#define _MDAEvent_H_

#ifdef _MSC_VER
// We use exception specifications to instruct SWIG to generate the correct
// exception specifications for Java. Turn off the warnings that VC++ issues by
// the mere use of exception specifications (which VC++ does not implement).
#pragma warning(disable : 4290)
#endif
#include <map>
#include "MDAPosition.h"

enum Action {AcquireImage}; 
enum EventState {DoesNotExist, Registered, Start, Paused, Canceled, Finished, FrameReady};

class Position; 

class Channel{
    public:
        Channel();
        Channel(std::string config, std::string group);
        std::string getConfig(){return config_;};
        std::string getGroup(){return group_;};
    private:
        std::string config_;
        std::string group_;
};

class PropertyTuple{
public:
    PropertyTuple(std::string device_name, std::string property_name, float value);

private:
    std::string deviceName_;
    std::string propertyName_;
    float value_;
};


class MDAEvent{
public:
    MDAEvent();
    MDAEvent(std::map<std::string, int> index, Channel channel, float exposure,
    float min_start_time, Position position, Action action, int global_index, bool keep_shutter_open);
    float getMinStartTime();
    float getExposure();
    double getZ();
    double getX();
    double getY();
    void setZ(double z);
    void setX(double x);
    void setY(double y);
    bool isXSet();
    bool isYSet();
    bool isZSet();
    bool isExposureSet();
    bool isChannelSet();
    bool keepShutterOpen();
    int getGlobalIndex() {return globalIndex_;};
    Action getAction() {return action_;};
    std::string getChannelGroup(){return channel_.getGroup();};
    std::string getChannelConfig(){return channel_.getConfig();};
private:
    std::map<std::string, int> index_;
    Channel channel_;
    float exposure_;
    float minStartTime_;
    Position position_;
    int globalIndex_;
    bool keepShutterOpen_;
    Action action_;
    std::map<std::string, float> metadata_;


};

#endif // _MDAEvent_H_