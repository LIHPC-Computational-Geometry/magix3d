import pyMagix3D as Mgx3D
import os
import re

def test_geom_description():
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    gm = ctx.getGeomManager()

    gm.newBox (Mgx3D.Point(0, 0, 0), Mgx3D.Point(1, 1, 1), "")
    compare(gm.getTextualDescription("Pt0000", 0), "test_description_pt0000.txt")
    compare(gm.getTextualDescription("Crb0000", 1), "test_description_crb0000.txt")
    compare(gm.getTextualDescription("Surf0000", 2), "test_description_surf0000.txt")
    compare(gm.getTextualDescription("Vol0000", 3), "test_description_vol0000.txt")

# Filtrage du bloc propriétés OCC qui n'est présent qu'en mode DEBUG
def load_and_filter(filename):
    with open(filename, encoding="utf-8") as f:
        lines = f.readlines()

    result = []
    skip_block = False
    skip_words = ["Précision", "Param first", "Param last", "Nb noeuds", "Nb poles"]
    for line in lines:
        # Début du bloc à ignorer
        if "SerializedRepresentation Propriétés OCC" in line:
            skip_block = True
            continue
        if skip_block:
            # fin du bloc : quand on a sauté "Properties sets", on arrête d’ignorer
            # (on avait laissé le "Properties sets" précédent le bloc)
            if "Properties sets" in line:
                skip_block = False
            continue

        # Mots qui ne sont qu'en mode DEBUG
        if not any(w in line for w in skip_words):
            result.append(line)

    return result

def compare(text, model_file_name):
    test_folder = os.path.dirname(__file__)

    # Ecriture d'un fichier avec le texte pour avoir le même formatage
    file_path = os.path.join(test_folder, model_file_name + ".tmp")
    with open(file_path, "w", encoding="utf-8") as f:
        f.write(text)

    # Lecture du fichier qui vient d'être écrit
    text = load_and_filter(file_path)
    os.remove(file_path)

    # Lecture du fichier modèle
    file_path = os.path.join(test_folder, model_file_name)
    model = load_and_filter(file_path)

    # Comparaison en ignorant le hashcode
    assert text == model
