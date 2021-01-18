import os
import shutil


def add_to_filename(directory):
    for f in os.listdir(directory):
        print("Processing file " + f + "...")
        number = f[4:-4]
        new_number = int(number) + 30000
        shutil.copy(directory + f, directory + "new_" + str(new_number) + ".xml")
        os.remove(directory + f)

if __name__ == '__main__':
    add_to_filename("/home/tamas/Jugend_Forscht_2021/labels_sample/")
