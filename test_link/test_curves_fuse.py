import os
import pyMagix3D as Mgx3D

step_file_name = "mambo/Simple/S45.step"

def test_curves_fuse_s45():
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    gm = ctx.getGeomManager ()
    tm = ctx.getTopoManager ()

    ctx.setLengthUnit(Mgx3D.Unit.meter)
    gm.importSTEP(step_file_name)
    assert gm.getNbVolumes()==1
