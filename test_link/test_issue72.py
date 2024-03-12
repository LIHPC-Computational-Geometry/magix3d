import sys
import pyMagix3D as Mgx3D
import subprocess

SEGFAULT_PROCESS_RETURNCODE = -11

def test_issue72():
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    try:
        subprocess.run(["python3", "-m", "issue72"], check=True)
    except subprocess.CalledProcessError as err:
        if err.returncode == SEGFAULT_PROCESS_RETURNCODE:
            print("probably segfaulted")
            assert False
        else:
            print(f"crashed for other reasons: {err.returncode}")
            assert False
    else:
            assert True