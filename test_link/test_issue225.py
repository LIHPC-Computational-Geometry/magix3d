import math
import os
import pyMagix3D as Mgx3D
import pytest

step_file_name = "mambo/Basic/B18.step"

def test_issue225(capfd):
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    gm = ctx.getGeomManager()
    tm = ctx.getTopoManager ()

    magix3d_test_data_dir = os.environ['MAGIX3D_TEST_DATA_DIR']
    full_path = os.path.join(magix3d_test_data_dir, step_file_name)

    ctx.setLengthUnit(Mgx3D.Unit.centimeter)
    gm.importSTEP(full_path)
    assert gm.getNbVolumes()==1
    tm.newFreeTopoOnGeometry("Vol0000")
    assert math.isclose(tm.getEdgeLength("Ar0007"), 0.1, abs_tol=1e-6)