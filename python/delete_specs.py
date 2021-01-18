import xml.etree.ElementTree as ET
import sys
import os

def sort_out_classes(img_dir, ann_dir):
    for xml_file in os.listdir(ann_dir):
        print("Processing:", xml_file)
        
        real_filename = os.path.join(ann_dir, xml_file)
        tree = ET.parse(real_filename)
        root = tree.getroot()

        for _o in root.findall('object'):
            print(" | --> Deleting specification labels...")
            for _p in _o.findall('pose'):
                _o.remove(_p)
            for _p in _o.findall('part'):
                _o.remove(_p)
            for _p in _o.findall('occluded'):
                _o.remove(_p)
            for _p in _o.findall('actions'):
                _o.remove(_p)
            for _p in _o.findall('point'):
                _o.remove(_p)
        
        os.remove(real_filename)
        tree.write(real_filename)
        print(" | --> Saving to:", real_filename)


if __name__ == '__main__':
    image_directory = "/home/tamas/Desktop/coco_and_voc_images/"
    annotations_directory = "/home/tamas/Desktop/coco_and_voc_data/"

    sort_out_classes(image_directory, annotations_directory);
