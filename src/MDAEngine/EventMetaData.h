#ifndef _event_metadata_H
#define _event_metadata_H
#ifdef _MSC_VER
// We use exception specifications to instruct SWIG to generate the correct
// exception specifications for Java. Turn off the warnings that VC++ issues by
// the mere use of exception specifications (which VC++ does not implement).
#pragma warning(disable : 4290)
#endif
#include "MDAEvent.h"
#include <map>


class EventMetaData{
    public:
        EventMetaData();
        EventMetaData(int event_id, EventState state);
        void updateState(EventState state);
        void updateImage(void* image, unsigned imageWidth, unsigned imageHeight, unsigned bytesPerPixel, unsigned imageBitDepth);
        EventState getState(){return state_;};
        void* getImage(){return image_;};
        // to get the image, we need few things:
        unsigned getImageWidth() {return imageWidth_;};
        unsigned getImageHeight() {return imageHeight_;};
        unsigned getBytesPerPixel() {return bytesPerPixel_;};
        unsigned getImageBitDepth() {return imageBitDepth_;};

    private:
        int eventId_;
        EventState state_;
        void* image_;
        unsigned imageWidth_;
        unsigned imageHeight_;
        unsigned bytesPerPixel_;
        unsigned imageBitDepth_;

};

#endif // _event_metadata_H