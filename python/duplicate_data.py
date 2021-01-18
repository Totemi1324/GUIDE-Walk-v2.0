import os
import cv2
import xml.etree.ElementTree as ET


def duplicate_images_of_class(images, labels, cls):
    save_dir = "../Dataset/Labels_temp"
    for label_file in os.listdir(labels):
        real_file_name = label_file[:-4]
        full_path = os.path.join(labels, label_file)

        tree = ET.parse(full_path)
        root = tree.getroot()

        match = 0
        mismatch = 0
        for _o in root.findall("object"):
            if _o.find("name").text in cls:
                match += 1
            else:
                mismatch += 1

        if match >= 1 and mismatch == 0:
            #img = cv2.imread(os.path.join(images, real_file_name) + ".jpg", cv2.IMREAD_COLOR)
            #img_flip = cv2.flip(img, 1)
            #cv2.imwrite(os.path.join(save_dir, real_file_name) + "_f.jpg", img_flip)
            for _o in root.findall("object"):
                x_min = int(_o[4][0].text)
                x_max = int(_o[4][2].text)
                _o[4][0].text = str(x_max - ((x_max - 640) * 2))
                _o[4][2].text = str(x_min - ((x_min - 640) * 2))
            root.find("filename").text = real_file_name + "_f.jpg"
            root.find("path").text = "/home/tamas/Jugend_Forscht_2021/pics/" + real_file_name + "_f.jpg"
            tree.write(os.path.join(save_dir, real_file_name) + "_f.xml")

if __name__ == '__main__':
    images = "../Dataset/Images_2"
    labels = "../Dataset/Labels_2"
    #duplicate_images_of_class(images, labels, ['trafficlight_red', 'trafficlight_green'])
    img = cv2.imread("../Dataset/Images_2/new_368_f.jpg", cv2.IMREAD_COLOR)
    cv2.rectangle(img, (519, 128), (565, 215), (0, 0, 255), 2)
    cv2.rectangle(img, (505, 312), (520, 350), (0, 0, 255), 2)
    cv2.imshow("IMAGE", img)
    cv2.waitKey(0)
    cv2.destroyAllWindows()
