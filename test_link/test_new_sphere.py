import pyMagix3D as Mgx3D
import pytest
import math

def test_new_sphere(capfd):
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    gm = ctx.getGeomManager()
    tm = ctx.getTopoManager()
    mm = ctx.getMeshManager()

    # Création du volume Vol0000
    ctx.getGeomManager().newSphere (Mgx3D.Point(0, 0, 0), 1, Mgx3D.Portion.ENTIER, "AAA")
    assert gm.getNbVolumes()==1
    assert gm.getInfos("Vol0000", 3).area==pytest.approx(4/3 * math.pi, abs=1e-7)

    # Création du volume Vol0001
    ctx.getGeomManager().newSphere (Mgx3D.Point(0, 0, 0), 1, Mgx3D.Portion.TROIS_QUARTS, "AAA")
    assert gm.getNbVolumes()==2
    assert gm.getInfos("Vol0001", 3).area==pytest.approx(math.pi, abs=1e-7)

    # Création du volume Vol0002
    ctx.getGeomManager().newSphere (Mgx3D.Point(0, 0, 0), 1, Mgx3D.Portion.DEMI, "AAA")
    assert gm.getNbVolumes()==3
    assert gm.getInfos("Vol0002", 3).area==pytest.approx(2/3 * math.pi, abs=1e-7)

    # Création du volume Vol0003
    ctx.getGeomManager().newSphere (Mgx3D.Point(0, 0, 0), 1, Mgx3D.Portion.TIERS)
    assert gm.getNbVolumes()==4
    assert gm.getInfos("Vol0003", 3).area==pytest.approx(4/9 * math.pi, abs=1e-7)

    # Création du volume Vol0004
    ctx.getGeomManager().newSphere (Mgx3D.Point(0, 0, 0), 1, Mgx3D.Portion.QUART)
    assert gm.getNbVolumes()==5
    assert gm.getInfos("Vol0004", 3).area==pytest.approx(1/3 * math.pi, abs=1e-7)

    # Création du volume Vol0005
    ctx.getGeomManager().newSphere (Mgx3D.Point(0, 0, 0), 1, Mgx3D.Portion.CINQUIEME)
    assert gm.getNbVolumes()==6
    assert gm.getInfos("Vol0005", 3).area==pytest.approx(4/15 * math.pi, abs=1e-7)

    # Création du volume Vol0006
    ctx.getGeomManager().newSphere (Mgx3D.Point(0, 0, 0), 1, Mgx3D.Portion.SIXIEME)
    assert gm.getNbVolumes()==7
    assert gm.getInfos("Vol0006", 3).area==pytest.approx(2/9 * math.pi, abs=1e-7)

    # Création du volume Vol0007
    ctx.getGeomManager().newSphere (Mgx3D.Point(0, 0, 0), 1, Mgx3D.Portion.SIXIEME)
    assert gm.getNbVolumes()==8
    assert gm.getInfos("Vol0007", 3).area==pytest.approx(2/9 * math.pi, abs=1e-7)

    # Création du volume Vol0008
    ctx.getGeomManager().newSphere (Mgx3D.Point(0, 0, 0), 1, 360)
    assert gm.getNbVolumes()==9
    assert gm.getInfos("Vol0008", 3).area==pytest.approx(4/3 * math.pi, abs=1e-7)

    # Création du volume Vol0009
    ctx.getGeomManager().newSphere (Mgx3D.Point(0, 0, 0), 1, 180)
    assert gm.getNbVolumes()==10
    assert gm.getInfos("Vol0009", 3).area==pytest.approx(2/3 * math.pi, abs=1e-7)

    # Création du volume Vol0010
    ctx.getGeomManager().newSphere (Mgx3D.Point(0, 0, 0), 1, 90)
    assert gm.getNbVolumes()==11
    assert gm.getInfos("Vol0010", 3).area==pytest.approx(1/3 * math.pi, abs=1e-7)

    # Création du volume Vol0011
    ctx.getGeomManager().newSphere (Mgx3D.Point(0, 0, 0), 1, 100)
    assert gm.getNbVolumes()==12
    assert gm.getInfos("Vol0011", 3).area==pytest.approx((100/360) * (4/3) * math.pi, abs=1e-7)