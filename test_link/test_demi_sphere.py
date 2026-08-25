import pyMagix3D as Mgx3D

def test_no_crossed_cells(capfd):
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    tm = ctx.getTopoManager()
    mm = ctx.getMeshManager()


    # Création d'une sphère avec une topologie
    tm.newSphereWithTopo (Mgx3D.Point(0, 0, 0), 1, Mgx3D.Portion.DEMI, True, .5, 10, 10)

    # Création du maillage pour tous les blocs
    mm.newAllBlocksMesh()


    out, err = capfd.readouterr()
    expected1 = "WARNING: 1 bloc semble avoir des mailles croisées"
    expected2 = "Il s'agit de Bl0004"
    assert expected1 not in out
    assert expected2 not in out