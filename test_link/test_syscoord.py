import math
import pyMagix3D as Mgx3D

def test_sys_coord_1(capfd):
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    scm = ctx.getSysCoordManager()
    tm = ctx.getTopoManager()
    gm = ctx.getGroupManager()

    tm.newBoxWithTopo (Mgx3D.Point(0, 0, 0), Mgx3D.Point(1, 1, 1), 10, 10, 10, "CUBE")
    scm.newSysCoord (Mgx3D.Point(0, 0, 0), "Original")
    gm.getInfos("Original", 3) # check that the group exists (exception otherwise)
    scm.newSysCoord (Mgx3D.Point(1, 2, 3), "Decale")
    gm.getInfos("Decale", 3) # check that the group exists (exception otherwise)
    scm.newSysCoord (Mgx3D.Point(1, 2, 3), Mgx3D.Point(2, 2, 3), Mgx3D.Point(1, 3, 3.5), "LocalDecale")
    gm.getInfos("LocalDecale", 3) # check that the group exists (exception otherwise)
    scm.newSysCoord (Mgx3D.Point(0, 0, 0), Mgx3D.Vector(1, 0, 0), Mgx3D.Vector(0, 1, 0.5), "LocalOrigine")
    gm.getInfos("LocalOrigine", 3) # check that the group exists (exception otherwise)

    tm.setVertexLocation(["Som0006"], True, 0, True, 0, True, 0, "Rep0001")
    assertPoint(tm, "Som0006", 1, 2, 3)
    tm.setVertexLocation(["Som0006"], True, 0, True, 1, True, 0, "Rep0002")
    assertClosePoint(tm, "Som0006", 1, 2.89442719099992, 3.44721359549996)

def test_sys_coord_2(capfd):
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    scm = ctx.getSysCoordManager()
    tm = ctx.getTopoManager()
    gm = ctx.getGroupManager()

    scm.newSysCoord (Mgx3D.Point(0, 0, 0), "Original")
    tm.newCylinderWithTopo (Mgx3D.Point(2, 3, 4), 1, Mgx3D.Vector(5, 6, 7), 360, True, 0.5, 10, 10, 10, "CYL")
    assertClosePoint(tm, "Som0002", 1.15465695980145, 3.09012445348047, 4.52656692572998)
    assertClosePoint(tm, "Som0008", 1.57732847990073, 3.04506222674024, 4.26328346286499)
    scm.newSysCoord (Mgx3D.Point(2, 3, 4), Mgx3D.Point(2, 3.7592566023653, 3.34920862654403), Mgx3D.Point(1.15465695980145, 3.09012445348047, 4.52656692572998), "RepCylindre")
    tm.setVertexCylindricalLocation(["Som0002", "Som0008"], False, 0, True, 90, False, 0, "Rep0001")
    assertClosePoint(tm, "Som0002", 1.12095092700847, 3.31025261399701, 4.36196138299651)
    assertClosePoint(tm, "Som0008", 1.56047546350423, 3.15512630699851, 4.18098069149826)
    tm.setVertexCylindricalLocation(["Som0026", "Som0027", "Som0018", "Som0008"], True, 0.2, False, 0, False, 0, "Rep0001")
    assertClosePoint(tm, "Som0008", 1.82419018540169, 3.0620505227994, 4.0723922765993)

def assertPoint(m, point, x, y, z):
    p = m.getCoord(point)
    assert p.getX() == x
    assert p.getY() == y
    assert p.getZ() == z

def assertClosePoint(m, point, x, y, z):
    p = m.getCoord(point)
    assert math.isclose(p.getX(), x, abs_tol=1e-15)
    assert math.isclose(p.getY(), y, abs_tol=1e-15)
    assert math.isclose(p.getZ(), z, abs_tol=1e-15)
