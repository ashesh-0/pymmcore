#include <iostream>
#include <vector>

#include "MDAEvent.h"
#include "StdoutEventNotifier.h"
using namespace std;

bool StdoutEventNotifier::notifyStart(MDAEvent& event){
    std::cout << "Start event" << std::endl;
    return true;
}

bool StdoutEventNotifier::notifyPauseToggled(MDAEvent& event, bool paused){
    std::cout << "PauseToggle event" << std::endl;
    return true;
}

bool StdoutEventNotifier::notifyCanceled(MDAEvent& event){
    std::cout << "Canceled event" << std::endl;
    return true;
}

bool StdoutEventNotifier::notifyFinished(MDAEvent& event){
    std::cout << "Finished event" << std::endl;
    return true;
}

bool StdoutEventNotifier::notifyFrameReady(MDAEvent& event, std::map<std::string, std::string> &metadata, void* image, 
                        unsigned imageWidth, unsigned imageHeight, unsigned bytesPerPixel, unsigned imageBitDepth){
    std::cout << "frameReady event" << std::endl;
    return true;
}

bool StdoutEventNotifier::notifyRegistered(MDAEvent& event){
    std::cout << "Registered event" << std::endl;
    return true;
}

// int main()
// {
//     auto notifier = StdoutEventNotifier();
// // create an event. 
//     auto index = std::map<std::string, int>();
//     index["t"] = 4;
//     index["c"] = 0;
//     index["z"] = 5;
//     Channel channel = Channel("FITC", "channel");
//     float exposure = 50;
//     float min_start_time = 8;
//     Position position = Position();
//     position.setZ(2);
//     position.setName("dummyPos");
//     int global_index = 0;
//     bool keep_shutter_open = false;
//     Action action = AcquireImage;
//     auto event = MDAEvent(index, channel, exposure, min_start_time, position, action, global_index, keep_shutter_open);


// // 
//     notifier.notifyStart(event);
//     notifier.notifyPauseToggled(true);
//     notifier.notifyCanceled(event);
//     notifier.notifyFinished(event);
//     std::map<std::string, std::string> metadata;
//     void* pixels = nullptr;
//     notifier.notifyFrameReady(event, metadata, pixels);


//     auto timesince_epoch =  std::chrono::system_clock::now().time_since_epoch();
//     auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(timesince_epoch).count();
//     auto milis2 = timesince_epoch/std::chrono::milliseconds(1);
//     auto milis3 = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count()/1000.0;

//     std::cout << "millis: " << millis/1000.0 << std::endl;
//     std::cout << "millis2: " << milis2 << std::endl;
//     std::cout << "millis3: " << milis3 << std::endl;
//     std::cout << "one milli second" << std::chrono::milliseconds(1).count() << std::endl;

//     std::this_thread::sleep_for(std::chrono::microseconds(4*1000 * 1000));

//     timesince_epoch =  std::chrono::system_clock::now().time_since_epoch();
//     millis = std::chrono::duration_cast<std::chrono::milliseconds>(timesince_epoch).count();
//     milis2 = timesince_epoch/std::chrono::milliseconds(1);
//     milis3 = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count()/1000.0 - milis3;

//     std::cout << "millis1: " << millis/1000.0 << std::endl;
//     std::cout << "millis2: " << milis2 << std::endl;
//     std::cout << "millis3: " << milis3 << std::endl;
// }