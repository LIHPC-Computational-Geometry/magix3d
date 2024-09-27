import pyMagix3D as Mgx3D

# Issue2 : l'opérateur == de la classe Point utilisait la
# méthode length ce qui n'est pas assez précis sur les
# grands nombres => le 2e print ne fonctionnait pas :
# getVertexAt impossible, on trouve 0 sommets à la position  [ 3757, 0, 0]
def test_issue2():
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    gm = ctx.getGeomManager()

    # Création du sommet Pt0000
    gm.newVertex (Mgx3D.Point(0.3756999999999998, 0, 0))
    # Création du sommet Pt0001
    gm.newVertex (Mgx3D.Point(0.3306999999999998, 0, 0))
    # Création du segment Crb0000
    gm.newSegment("Pt0000", "Pt0001")

    # recherche d'un point par ses coordonnées
    print(gm.getVertexAt(Mgx3D.Point(0.3757,0,0)))

    # Homothétie de tout
    scale = 1.000000e+04
    gm.scaleAll(scale)

    # recherche d'un point par ses coordonnées
    print(gm.getVertexAt(Mgx3D.Point(scale*0.3757,0,0)))
