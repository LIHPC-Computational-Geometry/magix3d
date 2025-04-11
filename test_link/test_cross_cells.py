import pyMagix3D as Mgx3D

# Test que le découpage en o-grid n'est PAS fait quand la contrainte d'Euler n'est pas vérifiée 
def test_cross_cells(capfd):
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    tm = ctx.getTopoManager()
    mm = ctx.getMeshManager()

    ctx.clearSession() # Clean the session after the previous test
    # Création d'un cylindre avec une topologie
    tm.newCylinderWithTopo (Mgx3D.Point(0, 0, 0), 1, Mgx3D.Vector(10, 0, 0), 360, True, .5, 10, 10, 10, "A")
    # Modification de la position des sommets topologiques Som0018 en coordonnées cartésiennes
    tm.setVertexLocation (["Som0018"], True, 0, True, -.1, True, .1)
    # Création du maillage pour tous les blocs
    mm.newAllBlocksMesh()

    out, err = capfd.readouterr()
    expected1 = "WARNING: 3 blocs semblent avoir des mailles croisées"
    expected2 = "Il s'agit de Bl0001 Bl0002 Bl0004"
    assert expected1 in out
    assert expected2 in out
