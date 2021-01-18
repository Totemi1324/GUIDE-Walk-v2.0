import os
import shutil


def delete_unused_images(pics, labels, target):
    for label_file in os.listdir(labels):
        real_file_name = label_file[:-4]
        if os.path.isfile(pics + real_file_name + ".jpg"):
            print("Copying file " + real_file_name + ".jpg...")
            shutil.copy(pics + real_file_name + ".jpg", target + real_file_name + ".jpg")
def join_directories(source, target):
    for f in os.listdir(source):
        print("Copying file " + f + "...")
        shutil.copy(source + f, target + f)
def compare_directories(d1, d2):
    for f in os.listdir(d1):
        real_file_name = f[:-4]
        if not os.path.isfile(d2 + real_file_name + ".jpg"):
            print("Mismatch detected! File: " + f)

if __name__ == '__main__':
    pictures = "/home/tamas/Jugend_Forscht_2021/pics3/"
    labels = "/home/tamas/Jugend_Forscht_2021/labels_sample/"
    target = "/home/tamas/Jugend_Forscht_2021/pics2_labeled/"

    # delete_unused_images(pictures, labels, target)
    join_directories("/home/tamas/Jugend_Forscht_2021/labels_sample/", "/home/tamas/Jugend_Forscht_2021/labels/")
    # compare_directories(labels, target)
