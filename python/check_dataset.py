import xml.etree.ElementTree as ET
import numpy as np
import os
import cv2


def get_bounding_boxes(ann_file):
    _x, _y, _w, _h, _class = 0, 0, 0, 0, 0

    tree = ET.parse(ann_file)
    root = tree.getroot()

    _name = root[1].text
    _w = root[3][0].text
    _h = root[3][1].text

    bbox_list = []
    for obj in root.iter('object'):
        _class = obj[0].text
        _xmin = obj[1][0].text
        _ymin = obj[1][1].text
        _xmax = obj[1][2].text
        _ymax = obj[1][3].text

        bbox = {
            "filename": _name,
            "class": _class,
            "width": int(_w),
            "height": int(_h),
            "xmin": int(_xmin),
            "ymin": int(_ymin),
            "xmax": int(_xmax),
            "ymax": int(_ymax)
        }
        bbox_list.append(bbox)

    return bbox_list

def draw_bounding_box(img, bbox):
    cv2.rectangle(img, (bbox["xmin"], bbox["ymin"]), (bbox["xmax"], bbox["ymax"]), (0, 0, 255), 2)


if __name__ == '__main__':
    for ann in os.listdir("coco_data"):
        bounding_box_list = get_bounding_boxes(os.path.join("coco_data", ann))
        print(ann, bounding_box_list)
        image = cv2.imread(os.path.join("coco_images", bounding_box_list[0]['filename']))
        
        for bounding_box in bounding_box_list:
            draw_bounding_box(image, bounding_box)

        while True:
            cv2.imshow(str("Output: " + bounding_box_list[0]['filename']), image)
            if cv2.waitKey(0) == ord('c'):
                cv2.destroyAllWindows()
                break

