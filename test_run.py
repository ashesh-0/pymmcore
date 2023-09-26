from pymmcore import Position,AcquireImage, MDAEvent, Channel
from pymmcore import *
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
runner = CMMRunner()
runner.loadSystemConfiguration('/home/ubuntu/ashesh/software_installed/MMConfig_demo.cfg')
runner.prepareToRun()
print("prepared to run")
output = runner.runEvent(mdaevent)
print('FrameReady:', runner.getEventState(0) == FrameReady)
import matplotlib.pyplot as plt
plt.imshow(output)
plt.show()