from pymmcore import Position,AcquireImage, MDAEvent, Channel
from pymmcore import *
from threading import RLock, Thread


index = StrIntMap({'t':4,'c':0,'z':5})
channel = Channel('FITC','channel')
exposure = 50
min_start_time = 1
global_index = 0
keep_shutter_open = False

position = Position()
position.setZ(2)
action = AcquireImage
mdaevent = MDAEvent(index, channel, exposure, min_start_time, position, action, global_index, keep_shutter_open)
core = CMMCore()
core.loadSystemConfiguration('/home/ubuntu/ashesh/software_installed/MMConfig_demo.cfg')

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

runner = Runner(core)
output = runner.run_async(EventVector([mdaevent]))
print('FrameReady:', runner.getEventState(0) == FrameReady)
output.join()
print('FrameReady:', runner.getEventState(0) == FrameReady)
img = runner.getEventImage(0)
# import matplotlib.pyplot as plt
# plt.imshow(output)
# plt.show()