import cv2
import os


def refine_dataset(images):
    file_count = len([name for name in os.listdir(images) if os.path.isfile(os.path.join(images, name))])
    counter = 0

    for img_file in os.listdir(images):
        print("Processing image #" + str(counter) + "... (" + str(counter / file_count) + "%)")
        img = cv2.imread(images + img_file, cv2.IMREAD_COLOR)
        cv2.imwrite(images + img_file, img)
        counter += 1

if __name__ == "__main__":
    refine_dataset("/home/elm/MobileNet-SSD/create_lmdb/Dataset/Images/")
