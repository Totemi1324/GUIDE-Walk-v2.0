import xml.etree.ElementTree as ET
import sys
import os

def sort_out_classes(img_dir, ann_dir, classes):
    for xml_file in os.listdir(ann_dir):
        print("Processing:", xml_file)
        
        real_filename = os.path.join(ann_dir, xml_file)
        matches = 0
        tree = ET.parse(real_filename)
        root = tree.getroot()

        for _o in root.findall('object'):
            print(" | --> Deleting part labels...")
            for _p in _o.findall('part'):
                _o.remove(_p)

            if _o.find('name').text in classes:
                matches += 1
            else:
                root.remove(_o)
                print(" | --> Wrong class detected (" + _o.find('name').text + "), deleting...")
        
        if matches > 0:
            os.remove(real_filename)
            tree.write(real_filename)
            print(" | --> Saving to:", real_filename)
        else:
            os.remove(real_filename)
            os.remove(str(os.path.join(img_dir, xml_file[:-4]) + ".jpg"))
            print(" | --> No matching classes found, deleting", real_filename, "with corresponding image", str(os.path.join(img_dir, xml_file[:-4]) + ".jpg"))
                


if __name__ == '__main__':
    image_directory = "/home/tamas/voc_data/train/images/"
    annotations_directory = "/home/tamas/voc_data/train/labels/"
    classes = ["person", "bicycle", "car", "motorcycle", "bus", "bench", "chair", "bin"]

    sort_out_classes(image_directory, annotations_directory, classes);
