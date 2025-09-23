import pyMagix3D as Mgx3D
import os
import re

def test_geom_description():
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    gm = ctx.getGeomManager()

    gm.newBox (Mgx3D.Point(0, 0, 0), Mgx3D.Point(1, 1, 1), "")
    compare(gm.getTextualDescription("Pt0000", 0, False), "test_description_pt0000.txt")
    compare(gm.getTextualDescription("Pt0000", 0, True), "test_description_pt0000.txt")

    compare(gm.getTextualDescription("Crb0000", 1, False), "test_description_crb0000.txt")
    compare(gm.getTextualDescription("Crb0000", 1, True), "test_description_crb0000.txt")

    compare(gm.getTextualDescription("Surf0000", 2, False), "test_description_surf0000.txt")
    compare(gm.getTextualDescription("Surf0000", 2, True), "test_description_surf0000.txt")

    compare(gm.getTextualDescription("Vol0000", 3, False), "test_description_vol0000.txt")
    compare(gm.getTextualDescription("Vol0000", 3, True), "test_description_vol0000.txt")

def normaliser_contenu(texte):
    return re.sub(r"HashCode			: [0-9a-fA-F]+", "hash=<IGNORED>", texte)

def compare(text, model_file_name):
    # Ecriture d'un fichier avec le texte pour avoir le même formatage
    with open(model_file_name + ".tmp", "w", encoding="utf-8") as f:
        f.write(text)

    # Lecture du fichier qui vient d'être écrit
    with open(f.name, "r", encoding="utf-8") as f1:
        text = f1.read()
    os.remove(f.name)

    # Lecture du fichier modèle
    with open(model_file_name, "r", encoding="utf-8") as f2:
        model = f2.read()

    # Comparaison en ignorant le hashcode
    assert normaliser_contenu(text) == normaliser_contenu(model)
