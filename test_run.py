from pymmcore import Position,AcquireImage, MDAEvent, Channel
from pymmcore import *
from threading import RLock, Thread

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
    def notifyRegistered(self, event):
        print('Notify registered')
        return super().notifyRegistered(event)
    
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

for global_index in range(1):
    index = StrIntMap({'t':4,'c':0,'z':5})
    channel = Channel('FITC','channel')
    exposure = 50
    min_start_time = 1
    keep_shutter_open = False

    position = Position()
    position.setZ(2)
    action = AcquireImage
    mdaevent = MDAEvent(index, channel, exposure, min_start_time, position, action, global_index, keep_shutter_open)


    output = runner.run_async(EventVector([mdaevent]))
    print('FrameReady:', runner.getEventState(0) == FrameReady)
    output.join()
    print('FrameReady:', runner.getEventState(0) == FrameReady)
    # img = runner.getEventImage(0)