from pymmcore import CMMRunner
# from pymmcore import *
from pymmcore import Position,AcquireImage, MDAEvent, Channel
from threading import RLock, Thread

def test_runner(runner:CMMRunner):
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
    output = runner.run_async(EventVector([mdaevent]))
    print('FrameReady:', runner.getEventState(0) == FrameReady)
    output.join()
    print('FrameReady:', runner.getEventState(0) == FrameReady)
    img = runner.getEventImage(0)

def test_wait_time(runner:CMMRunner):
    index = StrIntMap({'t':4,'c':0,'z':5})
    channel = Channel('FITC','channel')
    exposure = 50
    min_start_time = 5
    global_index = 0
    keep_shutter_open = False

    position = Position()
    position.setZ(2)
    action = AcquireImage
    mdaevent = MDAEvent(index, channel, exposure, min_start_time, position, action, global_index, keep_shutter_open)
    output = runner.run_async(EventVector([mdaevent]))
    print('FrameReady:', runner.getEventState(0) == FrameReady)
    output.join()
    print('FrameReady:', runner.getEventState(0) == FrameReady)
    img = runner.getEventImage(0)

def test_setting_position(runner:CMMRunner):
    pass