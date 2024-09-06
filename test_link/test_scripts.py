import pathlib
import runpy
import pytest

scripts = pathlib.Path(__file__, '..', 'scripts').resolve().glob('*.py')

@pytest.mark.parametrize('script', scripts)
def test_script_execution(script):
    runpy.run_path(script)
