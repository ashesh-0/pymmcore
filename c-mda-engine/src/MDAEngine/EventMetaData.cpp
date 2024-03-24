#include "EventMetaData.h"
#include <stdexcept>
#include <iostream>
using namespace std;

EventMetaData::EventMetaData(){
    eventId_ = -1;
    state_ = DoesNotExist;
}
EventMetaData::EventMetaData(int event_id, EventState state){
    eventId_ = event_id;
    state_ = state;
}

void EventMetaData::updateState(EventState state){
    state_ = state;
}

void EventMetaData::updateImage(void* image,unsigned imageWidth, unsigned imageHeight, unsigned bytesPerPixel, unsigned imageBitDepth){
    image_ = image;
    imageWidth_ = imageWidth;
    imageHeight_ = imageHeight;
    bytesPerPixel_ = bytesPerPixel;
    imageBitDepth_ = imageBitDepth;
}
