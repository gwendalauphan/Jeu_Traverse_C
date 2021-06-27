from os import listdir
from os.path import isfile, join, isdir
root_path = "./"
root = listdir(root_path)

total = 0
def counter(fichiers, path):
    global total
    for file in fichiers:
        lignes = 0
        if isfile(join(path, file)) and file != "makefile":
            if file.split(".")[1] == "c" or file.split(".")[1] == "h":
                with open(join(path,file), "r") as fichier:
                    temp = fichier.readlines()
                    for _ in temp:
                        lignes += 1
                    print(file,  lignes)
                    total += lignes
                    lignes = 0
        elif isdir(join(path, file)):
            temp = join(path, file)
            counter(listdir(temp), temp)
counter(root, root_path)
print("total de lignes = ", total)
input()
