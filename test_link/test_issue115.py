import pyMagix3D as Mgx3D

# Vérification que le maillage Tetra fonctionne
def test_issue115():
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    tm = ctx.getTopoManager()
    mm = ctx.getMeshManager()
    # Création d'un cylindre avec une topologie
    tm.newCylinderWithTopo (Mgx3D.Point(0, 0, 0), 1, Mgx3D.Vector(10, 0, 0), 360, False, 1, 10, 10, 10)
    # Création du maillage pour tous les blocs
    mm.newAllBlocksMesh()
    assert mm.getInfos("Hors_Groupe_3D", 3)=="Hors_Groupe_3D (uniqueId 33, Name Hors_Groupe_3D)"
