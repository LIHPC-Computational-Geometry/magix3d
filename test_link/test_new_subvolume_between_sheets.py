import pyMagix3D as Mgx3D

def test_new_subvolume_between_sheets(capfd):
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    tm = ctx.getTopoManager()
    mm = ctx.getMeshManager()

    # Création d'un cylindre avec une topologie
    tm.newCylinderWithTopo (Mgx3D.Point(0, 0, 0), 1, Mgx3D.Vector(10, 0, 0), 360, True, .5, 10, 10, 10, "CYL")
    # Création du maillage pour tous les blocs
    mm.newAllBlocksMesh()
    # Création volume entre 2 feuillets
    mm.newSubVolumeBetweenSheets ( ["Bl0000", "Bl0001", "Bl0002", "Bl0003", "Bl0004"], "Ar0010", 3, 5, "INT")

    assert mm.getNbVolumes() == 2
    out, err = capfd.readouterr()
    assert len(err) == 0
