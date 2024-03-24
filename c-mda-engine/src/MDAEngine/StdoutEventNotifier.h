#ifndef _stdout_event_notifier_H_
#define _stdout_event_notifier_H_

#ifdef _MSC_VER
// We use exception specifications to instruct SWIG to generate the correct
// exception specifications for Java. Turn off the warnings that VC++ issues by
// the mere use of exception specifications (which VC++ does not implement).
#pragma warning(disable : 4290)
#endif
#include <map>
#include "MDAPosition.h"
#include "BaseEventNotifier.h"
#include "MDAEvent.h"

class MDAEvent;
/*
There are 5 types of signals which needs to be handled by this class:
    Start event 
    PauseToggle event
    Canceled event
    Finished event
    frameReady event
*/
class StdoutEventNotifier : public BaseEventNotifier{
    public:
        bool notifyRegistered(MDAEvent& event);
        bool notifyStart(MDAEvent& event);
        bool notifyPauseToggled(MDAEvent& event, bool paused);
        bool notifyCanceled(MDAEvent& event);
        bool notifyFinished(MDAEvent& event);
        bool notifyFrameReady(MDAEvent& event, std::map<std::string, std::string> &metadata, void* image, 
                        unsigned imageWidth, unsigned imageHeight, unsigned bytesPerPixel, unsigned imageBitDepth);
};


#endif // _stdout_event_notifier_H_