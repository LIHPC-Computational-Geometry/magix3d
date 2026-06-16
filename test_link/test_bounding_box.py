import math
import os
import pyMagix3D as Mgx3D
import pytest

step_file_name = "mambo/Basic/B18.step"
tol = 1e-7

def assertPoint(tm, name, x, y, z):
    p = tm.getCoord(name)
    assert math.isclose(p.getX(), x, rel_tol=tol)
    assert math.isclose(p.getY(), y, rel_tol=tol)
    assert math.isclose(p.getZ(), z, rel_tol=tol)

# Issue 225 : Le bloc créé par boite englobante d'un volume peut avoir de mauvaises dimensions
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
    assert math.isclose(tm.getEdgeLength("Ar0002"), 0.1, abs_tol=tol) #0.1
    assert math.isclose(tm.getEdgeLength("Ar0007"), 0.1, abs_tol=1e-4) #0.10003610119179525
    assert math.isclose(tm.getEdgeLength("Ar0009"), 0.05, abs_tol=tol) #0.05000000000000623

# Issue 262 : Le bloc créé par boite englobante d'un volume peut avoir de mauvaises dimensions
# la création d'un bloc sur une sphère crée un bloc plat
def test_issue262():
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    gm = ctx.getGeomManager()
    tm = ctx.getTopoManager()
    # Création du volume Vol0000
    gm.newSphere (Mgx3D.Point(0, 0, 0), 1, Mgx3D.Portion.ENTIER)
    # Création d'un bloc topologique structuré sans projection (Vol0000)
    tm.newFreeTopoOnGeometry ("Vol0000")

    assertPoint(tm, "Som0000", -1, -1, -1)
    assertPoint(tm, "Som0007", 1, 1, 1)

    # Annulation de : Création d'un bloc topologique structuré sans projection (Vol0000)
    ctx.undo()
    # Création d'un bloc unitaire mis dans le groupe AAA
    ctx.getTopoManager().newFreeBoundedTopoInGroup ("AAA", 3, ["Vol0000"])

    assertPoint(tm, "Som0000", -1, -1, -1)
    assertPoint(tm, "Som0007", 1, 1, 1)

def test_freeboundedtopoForPoints():
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    gm = ctx.getGeomManager ()
    tm = ctx.getTopoManager ()

    gm.newVertex(Mgx3D.Point(-1.8, -.7, 1.2))
    gm.newVertex(Mgx3D.Point(-.7, .45, -.1))
    tm.newFreeBoundedTopoInGroup ("aaa", 3, ["Pt0000","Pt0001"])
    assertPoint(tm, "Som0007", -0.7, 0.45, 1.2)

def test_freeboundedtopoForSphere():
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    gm = ctx.getGeomManager ()
    tm = ctx.getTopoManager ()

    gm.newSphere (Mgx3D.Point(0, 0, 0), 1, 150)
    tm.newFreeTopoOnGeometry ("Vol0000")
    assertPoint(tm, "Som0007", 1.0, 1.0, 1.0)

def test_freeboundedtopoForCylinder():
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    gm = ctx.getGeomManager ()
    tm = ctx.getTopoManager ()

    gm.newCylinder (Mgx3D.Point(0, 0, 0), 1, Mgx3D.Vector(10, 0, 0), 360)
    tm.newFreeTopoOnGeometry ("Vol0000")
    assertPoint(tm, "Som0007", 10, 1.0, 1.0)

def test_bounding_box_with_scale():
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    gm = ctx.getGeomManager ()
    tm = ctx.getTopoManager ()

    gm.newSphere (Mgx3D.Point(0, 0, 0), 1, 125)
    gm.scaleAll(10, 20, 10)
    tm.newFreeTopoOnGeometry ("Vol0000")
    assertPoint(tm, "Som0007", 10.0, 20.0, 10.0)
