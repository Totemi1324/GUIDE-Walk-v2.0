import os
import check_dataset
import cv2
import shutil

folder = "coco_train_images"
image_count = len([name for name in os.listdir(folder) if os.path.isfile(os.path.join(folder, name))])

#it = 0
#for img in os.listdir(folder):
#    it += 1
#    if it < image_count * 0.2:
#        shutil.copy(os.path.join(folder, img), os.path.join("coco_test_images", img))
#        os.remove(os.path.join(folder, img))

#for img in os.listdir(folder):
#    shutil.copy(os.path.join(folder, img), os.path.join("coco_train_images", img))
#    os.remove(os.path.join(folder, img))

for img in os.listdir(folder):
    shutil.copy(os.path.join("coco_data", str(img[:-4] + ".xml")), os.path.join("coco_train_data", str(img[:-4] + ".xml")))
