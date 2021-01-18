import xml.etree.ElementTree as ET
import json
import coco_separate_class
import check_dataset
import cv2
import os


for ann_file in os.listdir('Annotations/train2017'):
    print("Processing", ann_file)
    json_file = open(str('Annotations/train2017/' + ann_file))
    json_str = json_file.read()
    json_data = json.loads(json_str)
    json_ann = json_data['annotation']
    size_list = [int(json_data['image']['width']), int(json_data['image']['height'])]
    
    if len(json_ann) == 0:
        print("ERROR: File does not contain any annotations! Aborting...")
    else:
        coco_separate_class.create_xml('coco_data', json_ann, size_list)

coco_separate_class.post_process_xml('coco_data')

# bounding_box_list = check_dataset.get_bounding_boxes('000000000094.xml')
# image = cv2.imread(os.path.join("coco_images", bounding_box_list[0]['filename']))

# for bounding_box in bounding_box_list:
#     check_dataset.draw_bounding_box(image, bounding_box)

# while True:
#     cv2.imshow(str("Output: " + bounding_box_list[0]['filename']), image)
#     if cv2.waitKey(0) == ord('c'):
#         cv2.destroyAllWindows()
#         break

