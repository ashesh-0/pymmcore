from unittest.mock import patch

import pytest
from pymmcore import CMMCore, CMMRunner
from threading import Thread

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


@pytest.fixture(scope="function")
def runner(request):
    core = CMMCore()
    core.loadSystemConfiguration('/home/ubuntu/ashesh/software_installed/MMConfig_demo.cfg')
    runner_ins = Runner(core)
    return runner_ins