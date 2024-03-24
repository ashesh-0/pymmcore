#include "MMRunner.h"

#include <algorithm>
#include <cassert>
#include <chrono>
#include <cstring>
#include <fstream>
#include <set>
#include <sstream>
#include <vector>
#include <iostream>
#include <thread>


#ifndef _WINDOWS
// Needed on Unix for getcwd() and gethostname()
#include <pwd.h>
#include <sys/types.h>
#include <unistd.h>
#else
// for _getcwd
#include <direct.h>
#endif

#include "StdoutEventNotifier.h"
#include "EventDataManager.h"

using namespace std;

CMMRunner::CMMRunner(CMMCore* core, EventDataManager& notifier):
core_(core),
running_(false),
paused_(false),
pausedTime_(0),
pausedInterval_(0.1),
cancelled_(false)
{
    notifier_ = &notifier;
    // get current time in seconds
    resetTimer();
    autoshutterWasSet_ = core_->getAutoShutter();
}

EventState CMMRunner::getEventState(int event_id)
{
    return ((EventDataManager*)notifier_)->getState(event_id);
}
void* CMMRunner::getEventImage(int event_id)
{
    return ((EventDataManager*)notifier_)->getImage(event_id);
}

double CMMRunner::getCurrentTime()
{
    auto now = std::chrono::system_clock::now().time_since_epoch();
    auto millisecs = std::chrono::duration_cast<std::chrono::milliseconds>(now).count();
    double secs = millisecs / 1000.0;
    return secs;
    
}
void sleep_for(double milliseconds)
{
    long long int microsec = (long long int)(milliseconds*1000.0);
    std::this_thread::sleep_for(std::chrono::microseconds(microsec));
}

void CMMRunner::resetTimer()
{
    startTime_ = getCurrentTime();
}

CMMRunner::~CMMRunner()
{
    try
    {
        // reset();
    }
    catch (...)
    {
        // LOG_ERROR(coreLogger_) << "Exception caught in CMMRunner desructor.";
    }
    // Not sure if we should delete the sequence_ or not. It is so because it is getting passed from 
    // outside and so should be deleted outside as well ?
    // delete sequence_;
}

double CMMRunner::timeElapsed()
{
    double curtime = getCurrentTime();
    double elapsedtime = curtime -startTime_;
    return elapsedtime;
}
bool CMMRunner::isRunning()
{
    return running_;
}

bool CMMRunner::isPaused()
{
    return paused_;
}

void CMMRunner::cancel()
{
    cancelled_ = true;
    pausedTime_ = 0;
}
bool CMMRunner::togglePause(MDAEvent& event)
{
    if (isRunning()){
        paused_ = !paused_;
        return notifier_->notifyPauseToggled(event, paused_);
    }
    return false;
}


bool CMMRunner::checkCanceled()
{
    // TODO: more logic. For now, just check the flag. in pymcoreplus, it is reset to false. 
    auto cancelled = cancelled_;
    cancelled_ = false;
    return cancelled;
}

/**
Wait until the event's min start time, checking for pauses cancellations.
Parameters
----------
event : MDAEvent The event to wait for.

Returns
-------
bool
    Whether the MDA was cancelled while waiting.
*/
bool CMMRunner::waitUntilEvent(MDAEvent& event)
{
    if (!isRunning())
        return false;
    else if (checkCanceled())
        return true;
    
    while (isPaused() && (!cancelled_))
    {
        pausedTime_ += pausedInterval_;
        // now, sleep for pausedInterval_ seconds
        sleep_for(pausedInterval_*1000);
        if (checkCanceled())
        {
            return true;
        }

    }

    if (event.getMinStartTime())
    {
        auto go_at = event.getMinStartTime() + pausedTime_;
    // We need to enter a loop here checking paused and canceled.
    // otherwise you'll potentially wait a long time to cancel
        auto to_go = go_at - timeElapsed();
        while (to_go > 0)
        {
            while (paused_ &&  !cancelled_)
            {
                pausedTime_ += pausedInterval_;  // fixme: be more precise
                to_go += pausedInterval_;
                // now, sleep for pausedInterval_ seconds
                sleep_for(pausedInterval_*1000);
            }
            if (cancelled_)
                break;
            
            // sleep for minimum of 0.5 seconds and to_go
            // TODO: why  are we sleeping for 0.5 seconds here?
            auto sleepdur = min(to_go*1000, (double)(500.0));
            sleep_for(sleepdur);

            to_go = go_at - timeElapsed();

            
        }

    }
    
// check canceled again in case it was canceled
// during the waiting loop
return checkCanceled();
}
void CMMRunner::prepareToRun()
{
    running_ = true;
    paused_ = false;
    pausedTime_ = 0;
    resetTimer();
}

void CMMRunner::run(std::vector<MDAEvent>& events)
{
    prepareToRun();
    for (int i = 0; i < events.size(); i++)
    {
        runEvent(events[i]);
    }
   
    finishRun();
}

/**
* Run a single event.
* Parameters
* ----------
* event : MDAEvent
* The event to run.
* Returns
* -------
* bool
* Returns the acquisiton output.
*/
void CMMRunner::runEvent(MDAEvent& event)
{
    // notifier_->notifyRegistered(event);
    if (waitUntilEvent(event)||  !running_)
    {
        notifier_->notifyCanceled(event);
        return;
    }
    notifier_->notifyStart(event);
    setupEvent(event);
    execEvent(event);
    teardownEvent(event);
}
void CMMRunner::execEvent(MDAEvent& event)
{
    core_->snapImage();
    if (!event.keepShutterOpen())
    {
        core_->setShutterOpen(false);
    }
    void* output = core_->getImage();
    auto metadata = std::map<std::string, std::string>();

    notifier_->notifyFrameReady(event,metadata, output, 
                core_->getImageWidth(), 
                core_->getImageHeight(), 
                core_->getBytesPerPixel(), 
                core_->getImageBitDepth());

}


void CMMRunner::setupEvent(MDAEvent& event)
{
        // if event.keep_shutter_open:
        //         ...
    auto x_pos = event.getX();
    auto y_pos = event.getY();
    if (event.isXSet() || event.isYSet())
    {
        setEventPosition(event);
    }
    if (event.isZSet())
    {
        setEventZ(event);
    }
    if (event.isChannelSet())
    {
        setEventChannel(event);
    }
    if (event.isExposureSet())
    {
        setEventExposure(event);
    }
    if (autoshutterWasSet_ && event.keepShutterOpen())
    {
        // autoshutterWasSet_: (if autoshutter wasn't set at the beginning of the sequence
        // then it never matters...)
        // keepShutterOpen: if we want to leave the shutter open after this event, and autoshutter
        // is currently enabled...

        core_->setShutterOpen(true);
        core_->setAutoShutter(false);
    }

}

void CMMRunner::finishRun()
{
    running_ = false;
    cancelled_ = false;
}

void CMMRunner::teardownEvent(MDAEvent& event)
{
    if (!event.keepShutterOpen() && autoshutterWasSet_)
    {
        core_->setAutoShutter(true);
    }
}

void CMMRunner::setEventChannel(MDAEvent& event)
{
    std::string channel_group = event.getChannelGroup();
    std::string channel_config = event.getChannelConfig();
    // TODO: for some reason, setConfig is failing. 
    // "ValueError: Preset "DAPI" of configuration group "channel" does not exist"
    // However, in the config file, I see the entry for it. 
    // std::cout << "Setting channel group to " << channel_group << " and channel config to " << channel_config << std::endl;
    // core_->setConfig(channel_group.c_str(), channel_config.c_str());
}
void CMMRunner::setEventExposure(MDAEvent& event)
{
    double exposure = event.getExposure();
    core_->setExposure(exposure);
}


// engine related
void CMMRunner::setEventZ(MDAEvent& event)
{
    std::string focus_device = core_->getFocusDevice();
    if (focus_device.empty())
    {
        std::cout << "No focus device set. Cannot set Z position." << std::endl;
        return;
    }
    auto z_pos = event.getZ();
    core_->setPosition(focus_device.c_str(), z_pos);

}

void CMMRunner::setEventPosition(MDAEvent& event)
{
    double x_pos;
    double y_pos;
    if (event.isXSet())
    {
        x_pos = event.getX();
    }
    else{
        // TODO: call mmcore to get current position
        x_pos = core_->getXPosition();
    }

    if (event.isYSet())
    {
        y_pos = event.getY();
    }
    else{
        // TODO: call mmcore to get current position
        y_pos = core_->getYPosition();
    
    }
    
    core_->setXYPosition(x_pos, y_pos);
}


// int main(int argc, char **argv)
// {
// //  auto var = CMMRunner();
// }
