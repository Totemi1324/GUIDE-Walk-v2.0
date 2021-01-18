import xml.etree.ElementTree as ET
import numpy as np
import os
import sys
import cv2

classes = ["person", "car", "bus", "bicycle", "motorcycle", "bench", "chair", "bin", "trafficlight_red", "trafficlight_green"]

def print_info(directory):
    if not os.path.isdir(directory + "/pics_labeled"):
        print("Error: Picture directory /pics does not exist. Aborting.")
        sys.exit()
    if not os.path.isdir(directory + "/labels"):
        print("Error: Labels directory /labels does not exist. Aborting.")
        sys.exit()
    print("---------------\nDATASET INFORMATION:\n")
    
    _num_p, _num_l, _num_cls = 0, 0, 0
    num_inst = [0] * len(classes)

    DIR = directory + "/pics_labeled"
    _num_p = len([name for name in os.listdir(DIR) if os.path.isfile(os.path.join(DIR, name))])
    DIR = directory + "/labels"
    _num_l = len([name for name in os.listdir(DIR) if os.path.isfile(os.path.join(DIR, name))])
    _num_cls = len(classes)

    for label_file in os.listdir(DIR):
        tree = ET.parse(DIR + "/" + label_file)
        root = tree.getroot()
        
        for obj in root.iter('object'):
            _c = obj[0].text
            for i in range(len(classes)):
                if _c == classes[i]:
                    num_inst[i] += 1;

    print("Images count: {}\nLabels count: {}\nNumber of classes: {}\n".format(_num_p, _num_l, _num_cls))
    for i in range(len(num_inst)):
        print("Instance no. for class #" + str(i) + ": " + str(num_inst[i]) + " (" + classes[i] + ")")
    print("---------------")

if __name__ == '__main__':
    dataset_dir = "/home/tamas/Jugend_Forscht_2021"
    print_info(dataset_dir)
