import pyMagix3D as Mgx3D

# Création d'un pattern de blocs en forme de T
def test_issue223():
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    tm = ctx.getTopoManager()

    tm.newFreeTopoInGroup("aaa", 3)
    tm.makeBlocksByExtrude(["Fa0001"], Mgx3D.Vector(1,0,0))
    tm.splitBlocksWithOgridV2(["Bl0001"], ["Fa0010", "Fa0009", "Fa0006", "Fa0008"], .5, 10)
    tm.splitBlocksWithOgridV2(["Bl0000"], ["Fa0003", "Fa0005", "Fa0002", "Fa0000"], .5, 10)

    # Avant la correction de l'issue, l'erreur suivante se produisait :
    # erreur interne, on ne trouve pas de sommet opposé au sommet marqué à 4
    tm.splitBlocksWithOgridV2(["Bl0006", "Bl0005", "Bl0002", "Bl0003"], [], .5, 10)

    assert tm.getNbBlocks() == 24
    