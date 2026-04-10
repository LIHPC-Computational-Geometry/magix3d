import os
import pyMagix3D as Mgx3D

def test_aero():
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    tm = ctx.getTopoManager()

    magix3d_test_data_dir = os.environ['MAGIX3D_TEST_DATA_DIR']
    full_path = os.path.join(magix3d_test_data_dir, "aero/param_RAMCII_3D.ini")

    tm.computeAero(full_path)

    assert tm.getNbBlocks() == 208
    assert tm.getNbFaces() == 688
    assert tm.getNbEdges() == 758

