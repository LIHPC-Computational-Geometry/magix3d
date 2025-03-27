import pyMagix3D as Mgx3D

def test_rotate_box():
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    tm = ctx.getTopoManager ()
    gm = ctx.getGeomManager ()
    tm.newBoxWithTopo (Mgx3D.Point(0, 0, 0), Mgx3D.Point(1, 1, 1), 10, 10, 10)
    vol = gm.getVolumeAt([Mgx3D.Point(0, 0, 1), Mgx3D.Point(0, 0, 0), Mgx3D.Point(0, 1, 1), Mgx3D.Point(0, 1, 0), Mgx3D.Point(1, 0, 1), Mgx3D.Point(1, 0, 0), Mgx3D.Point(1, 1, 1), Mgx3D.Point(1, 1, 0)])
    assert vol == 'Vol0000'
    gm.rotate (["Vol0000"], Mgx3D.RotX(90))
    img = gm.getVolumeAt([Mgx3D.Point(0, -1, 0), Mgx3D.Point(0, 0, 0), Mgx3D.Point(0, -1, 1), Mgx3D.Point(0, 0, 1), Mgx3D.Point(1, -1, 0), Mgx3D.Point(1, 0, 0), Mgx3D.Point(1, -1, 1), Mgx3D.Point(1, 0, 1)])
    assert img == 'Vol0000'
