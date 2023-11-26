from pymmcore import CMMRunner, CMMCore, EventDataManager, EventMetaData
# from pymmcore import *
from pymmcore import Position,AcquireImage, MDAEvent, Channel, StrIntMap, EventVector, FrameReady, Paused,Registered
from threading import RLock, Thread

from pymmcore import CMMCore, CMMRunner
from threading import Thread
from time import sleep, perf_counter

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


def test_runner_executes_multiple_events():
    core = CMMCore()
    core.loadSystemConfiguration('/home/ubuntu/ashesh/software_installed/MMConfig_demo.cfg')
    notifier = EventDataManager()
    runner = Runner(core, notifier)   
    for global_index in range(10):
        index = StrIntMap({'t':4,'c':0,'z':5})
        channel = Channel('FITC','channel')
        exposure = 50
        min_start_time = 0.1
        keep_shutter_open = False

        position = Position()
        position.setZ(2)
        action = AcquireImage
        mdaevent = MDAEvent(index, channel, exposure, min_start_time, position, action, global_index, keep_shutter_open)

        packet = EventMetaData(global_index, Registered)
        notifier.notifyRegistered(mdaevent, packet)
        output = runner.run_async(EventVector([mdaevent]))
        print('FrameReady:', runner.getEventState(global_index) == FrameReady)
        output.join()
        print('FrameReady:', runner.getEventState(global_index) == FrameReady)
        img = runner.getEventImage(global_index)
    del core 
    del runner

def test_wait_times_for_events():
    core = CMMCore()
    core.loadSystemConfiguration('/home/ubuntu/ashesh/software_installed/MMConfig_demo.cfg')
    notifier = EventDataManager()
    runner = Runner(core, notifier)   
    t0 = perf_counter()
    timestamps = []
    for global_index in range(10):
        index = StrIntMap({'t':4,'c':0,'z':5})
        channel = Channel('FITC','channel')
        exposure = 50
        min_start_time =  0.5
        keep_shutter_open = False

        position = Position()
        position.setZ(2)
        action = AcquireImage
        mdaevent = MDAEvent(index, channel, exposure, min_start_time, position, action, global_index, keep_shutter_open)
        packet = EventMetaData(global_index, Registered)
        notifier.notifyRegistered(mdaevent, packet)
        output = runner.run_async(EventVector([mdaevent]))
        print('FrameReady:', runner.getEventState(global_index) == FrameReady)
        output.join()
        print('FrameReady:', runner.getEventState(global_index) == FrameReady)
        img = runner.getEventImage(global_index)
        timestamps.append(perf_counter() - t0)
        assert timestamps[-1] >= min_start_time*(1+global_index)
    del core 
    del runner

def test_setting_positions():
    core = CMMCore()
    core.loadSystemConfiguration('/home/ubuntu/ashesh/software_installed/MMConfig_demo.cfg')
    notifier = EventDataManager()
    runner = Runner(core, notifier)   
    index = StrIntMap({'t':4,'c':0,'z':5})
    channel = Channel('FITC','channel')
    exposure = 50
    min_start_time = 0.1
    keep_shutter_open = False
    global_index = 0
    position = Position()
    
    
    position.setZ(2)
    position.setX(955)
    position.setY(123)

    action = AcquireImage
    mdaevent = MDAEvent(index, channel, exposure, min_start_time, position, action, global_index, keep_shutter_open)
    packet = EventMetaData(global_index, Registered)
    notifier.notifyRegistered(mdaevent, packet)
    runner.setEventPosition(mdaevent)
    assert tuple([int(x) for x in core.getXYPosition()]) == (955, 123)
    runner.setEventZ(mdaevent)

    assert core.getPosition() == 2
    del core 
    del runner

def test_setting_exposure():
    core = CMMCore()
    core.loadSystemConfiguration('/home/ubuntu/ashesh/software_installed/MMConfig_demo.cfg')
    notifier = EventDataManager()
    runner = Runner(core, notifier)   
    index = StrIntMap({'t':4,'c':0,'z':5})
    channel = Channel('FITC','channel')
    min_start_time = 0.1
    keep_shutter_open = False
    global_index = 0
    position = Position()
    position.setZ(2)
    position.setX(955)
    position.setY(123)
    
    exposure = 50

    action = AcquireImage
    mdaevent = MDAEvent(index, channel, exposure, min_start_time, position, action, global_index, keep_shutter_open)
    packet = EventMetaData(global_index, Registered)
    notifier.notifyRegistered(mdaevent, packet)
    runner.setEventExposure(mdaevent)
    assert core.getExposure() == exposure
    del core 
    del runner


def test_pause_logic():
    """
    Test that we can pause the runner and that it will wait for the pause to be released before continuing.
    """
    core = CMMCore()
    core.loadSystemConfiguration('/home/ubuntu/ashesh/software_installed/MMConfig_demo.cfg')
    notifier = EventDataManager()
    runner = Runner(core, notifier)   
    index = StrIntMap({'t':4,'c':0,'z':5})
    channel = Channel('FITC','channel')
    min_start_time = 0.2
    keep_shutter_open = False
    global_index = 0
    position = Position()
    position.setZ(2)
    position.setX(955)
    position.setY(123)
    
    exposure = 50

    action = AcquireImage
    mdaevent = MDAEvent(index, channel, exposure, min_start_time, position, action, global_index, keep_shutter_open)
    packet = EventMetaData(global_index, Registered)
    notifier.notifyRegistered(mdaevent, packet)
    output = runner.run_async(EventVector([mdaevent]))
    sleep(0.1)
    print(runner.togglePause(mdaevent))
    assert runner.getEventState(0) == Paused
    sleep(min_start_time + 1)
    runner.togglePause(mdaevent)
    output.join()

    assert runner.getEventState(0) == FrameReady
    del core 
    del runner
    # img = runner.getEventImage(0)

# def test_wait_time():
#     runner = get_runner()
#     index = StrIntMap({'t':4,'c':0,'z':5})
#     channel = Channel('FITC','channel')
#     exposure = 50
#     min_start_time = 5
#     global_index = 0
#     keep_shutter_open = False

#     position = Position()
#     position.setZ(2)
#     action = AcquireImage
#     mdaevent = MDAEvent(index, channel, exposure, min_start_time, position, action, global_index, keep_shutter_open)
#     output = runner.run_async(EventVector([mdaevent]))
#     print('FrameReady:', runner.getEventState(0) == FrameReady)
#     output.join()
#     print('FrameReady:', runner.getEventState(0) == FrameReady)
#     img = runner.getEventImage(0)

# def test_setting_position(runner:CMMRunner):
#     pass