from pymmcore import Position,AcquireImage, MDAEvent, Channel
from pymmcore import *
from threading import RLock, Thread
from time import sleep

class Runner(CMMRunner):
    def run_async(self, events):
        if self.isRunning():
            raise ValueError(
                "Cannot start an MDA while the previous MDA is still running."
            )
        self.prepareToRun()
        th = Thread(target=self.run, args=(events,))
        th.start()
        return th

class Notifier(EventDataManager):
    def notifyRegistered(self, event, packet):
        print('Notify registered')
        return super().notifyRegistered(event, packet)
    
    
    def notifyStart(self, event):
        print('Notify start')
        return super().notifyStart(event)

    def notifyPauseToggled(self, event, paused):
        print('Notify pause toggled')
        return super().notifyPauseToggled(event, paused)
    def notifyCanceled(self, event):
        print('Notify canceled')
        return super().notifyCanceled(event)
    
    def notifyFinished(self, event):
        print('Notify finished')
        return super().notifyFinished(event)
    
    def notifyFrameReady(self, event, metadata, image, imageWidth, imageHeight, bytesPerPixel, imageBitDepth):
        print('Notify frame ready', image)
        return super().notifyFrameReady(event, metadata, image, imageWidth, imageHeight, bytesPerPixel, imageBitDepth)


core = CMMCore()
core.loadSystemConfiguration('/home/ubuntu/ashesh/software_installed/MMConfig_demo.cfg')
notifier = Notifier()
runner = Runner(core, notifier)
for global_index in range(5):
    print('')
    print('')
    index = StrIntMap({'t':4,'c':0,'z':5})
    channel = Channel('FITC','channel')
    exposure = 50
    min_start_time = 1
    keep_shutter_open = False

    position = Position()
    position.setZ(2)
    action = AcquireImage
    mdaevent = MDAEvent(index, channel, exposure, min_start_time, position, action, global_index, keep_shutter_open)
    packet = EventMetaData(global_index, Registered)
    notifier.notifyRegistered(mdaevent, packet)

    output = runner.run_async(EventVector([mdaevent]))
    print('Is FrameReady for ', global_index, ": ", runner.getEventState(global_index) == FrameReady)

    if global_index == 3:
        runner.togglePause(mdaevent)
        sleep(5)
        print('Is FrameReady for ', global_index, ": ", runner.getEventState(global_index) == FrameReady)
        runner.togglePause(mdaevent)

    elif global_index == 4:
        runner.cancel()
    
    output.join()
    print('Is FrameReady for ', global_index, ": ", runner.getEventState(global_index) == FrameReady, end=' ')
    if runner.getEventState(global_index) == FrameReady:
        img = runner.getEventImage(global_index)
        print(img.shape)
    else:
        print('')