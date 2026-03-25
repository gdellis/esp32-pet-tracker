import pytest
from pytest_embedded import Dut


@pytest.mark.generic
@pytest.mark.parametrize("target", ["esp32c6"])
def test_driver_test(dut):
    dut.run_all_single_board_cases()
