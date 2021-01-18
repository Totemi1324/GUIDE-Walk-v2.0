from pycocotools.coco import COCO
import xml.etree.ElementTree as ET
import pandas as pd
import os
import shutil
import yaml

if __name__ == '__main__':
    # note this only refers to  the training set and not the validation set
    coco = COCO('annotations/instances_train2017.json')

    # note this only refers to the captions of the training set and not the validation set
    caps = COCO('annotations/captions_train2017.json')

def create_data_subset(folder_name, category):
    global coco
    global caps

    categories = coco.loadCats(coco.getCatIds())
    names = [cat['name'] for cat in categories] 

    print("Available categories: ")
    for index, n in enumerate(names):
        print(index, n)

    category_ids = coco.getCatIds(catNms=[category])
    image_ids = coco.getImgIds(catIds=category_ids)
    images = coco.loadImgs(image_ids)
    annIds = caps.getAnnIds(imgIds=image_ids)
    annotations = caps.loadAnns(annIds)

    # Split the annotations every 5 captions since there are 5 captions for each image
    annotations = [annotations[x:x + 5] for x in range(0, len(annotations), 5)]

    # Create empty dataframe with two columns for the image file name and the corresponding 
    captions
    df = pd.DataFrame(columns=['image_id', 'caption'])

    # Create folder in for the images of the selected category
    os.mkdir(folder_name)

    # Create map for image id (key) to captions (values)
    captions_dict = {}
    for i, n in enumerate(annotations):
        captions_dict[annotations[i][0]['image_id']] = annotations[i]

    horse_file_names = []
    for img in images:
        horse_file_names.append(img['file_name'])
        for entry in captions_dict[img['id']]:
            df.loc[len(df)] = [img['file_name'], entry['caption']]

    # Convert dataframe to csv file and save to folder
    df.to_csv(folder_name + "/captions.csv", index=False)

    # Copy all images of given category to new folder
    for filename in os.listdir('train2017'):
        if filename in horse_file_names:
            shutil.copy(os.path.join('train2017', filename), folder_name)

    print('Done creating data subset with images....')

def save_ids(filename, categories):
    global coco

    all_ids = []
    for i in range(len(categories)):
        category_ids = coco.getCatIds(catNms=categories[i])
        image_ids = coco.getImgIds(catIds=category_ids)
        for j in range(len(image_ids)):
            all_ids.append(image_ids[j]) if image_ids[j] not in all_ids else all_ids
        category_ids = []
        image_ids = []

    with open(filename, 'w') as f:
        for item in all_ids:
            f.write("%s\n" % str(item))

def read_ids(filename):
    with open(filename) as f:
        image_ids = f.read().splitlines()
    for i in range(len(image_ids)):
        image_ids[i] = int(image_ids[i])
  
    return image_ids

def save_anns(filename, ids):
    global coco
  
    annIds = coco.getAnnIds(imgIds=ids)
    annotations = coco.loadAnns(annIds)

    with open(filename, 'w') as f:
        for item in annotations:
            f.write("%s\n" % str(item))

def read_anns(filename):
    with open(filename) as f:
        annotations = f.read().splitlines()
    # for i in range(len(annotations)):
    #     print(str(i) + ": " + annotations[i])
    
    return annotations

def create_xml(directory, ann_set, size_list):
    print(ann_set)
    template = "000000000000"
    filename = str(os.path.join(directory, str(template[len(str(ann_set[0]['image_id'])):] + str(ann_set[0]['image_id']))) + '.xml')
    print(filename)
    f = open(filename, "w+")
    f.write("<annotation>\n\t<folder></folder>\n\t<filename></filename>\n\t<source>\n\t\t<database></database>\n\t\t<annotation></annotation>\n\t\t<image></image>\n\t</source>\n\t<size>\n\t\t<width></width>\n\t\t<height></height>\n\t\t<depth></depth>\n\t</size>\n")
    for ann in ann_set:
        f.write("\t<object>\n\t\t<name></name>\n\t\t<bndbox>\n\t\t\t<xmin></xmin>\n\t\t\t<ymin></ymin>\n\t\t\t<xmax></xmax>\n\t\t\t<ymax></ymax>\n\t\t</bndbox>\n\t</object>\n")
    f.write("</annotation>")
    f.close()
    
    tree = ET.parse(filename)
    root = tree.getroot()

    root[0].text = "Coco"
    root[1].text = str(template[len(str(ann_set[0]['image_id'])):] + str(ann_set[0]['image_id']) + ".jpg")
    root[2][0].text = "StreetVision Inference Dataset"
    root[2][1].text = "PASCAL VOC"
    root[2][2].text = "MS COCO-Dataset (Common Objects in Context)"
    root[3][0].text = str(size_list[0])
    root[3][1].text = str(size_list[1])
    root[3][2].text = "3"
    for i in range(len(ann_set)):
        if int(str(ann_set[i]['category_id'])) == 1:
            root[4 + i][0].text = "person"
        elif int(str(ann_set[i]['category_id'])) == 2:
            root[4 + i][0].text = "bicycle"
        elif int(str(ann_set[i]['category_id'])) == 3 or int(str(ann_set[i]['category_id'])) == 8:
            root[4 + i][0].text = "car"
        elif int(str(ann_set[i]['category_id'])) == 4:
            root[4 + i][0].text = "motorcycle"
        elif int(str(ann_set[i]['category_id'])) == 6:
            root[4 + i][0].text = "bus"
        elif int(str(ann_set[i]['category_id'])) == 14:
            root[4 + i][0].text = "bench"
        elif int(str(ann_set[i]['category_id'])) == 57:
            root[4 + i][0].text = "chair"
        else:
            print("ERROR: Unidentified class index:", str(ann_set[i]['category_id']))
            root[4 + i][0].text = "unidentified"
        root[4 + i][1][0].text = str(int(ann_set[i]['bbox'][0]))
        root[4 + i][1][1].text = str(int(ann_set[i]['bbox'][1]))
        root[4 + i][1][2].text = str(int(int(ann_set[i]['bbox'][0]) + int(ann_set[i]['bbox'][2])))
        root[4 + i][1][3].text = str(int(int(ann_set[i]['bbox'][1]) + int(ann_set[i]['bbox'][3])))

    print("Completed", filename)
    os.remove(filename)
    tree.write(filename)

def post_process_xml(directory):
    for xml_file in os.listdir(directory):
        tree = ET.parse(os.path.join(directory, xml_file))
        root = tree.getroot()

        for obj in root.findall('object'):
            if obj[0].text == "unidentified":
                print("Found unidentified object in " + xml_file + ", deleting...")
                root.remove(obj)

        os.remove(os.path.join(directory, xml_file))
        tree.write(os.path.join(directory, xml_file))


if __name__ == '__main__':
    # create_data_subset(folder_name='bench_images', category=['person', 'car', 'motorcycle', 'bus', 'bicycle', 'bench'])
    categories = coco.loadCats(coco.getCatIds())
    names = [cat['name'] for cat in categories] 

    print("Available categories: ")
    for index, n in enumerate(names):
        print(index, n)

    category_ids = coco.getCatIds(catNms=['person', 'car', 'motorcycle', 'bus', 'bicycle', 'bench'])
    print("Category-indexes: (person - car - motorcycle - bus - bicycle - bench)\n", category_ids)

    annotations = read_anns("annotations_5.txt")
    
    ann_memory = []
    ann_all = []
    counter = 0
    len_ann = len(annotations)
    for annotation in annotations:
        counter += 1
        annotation = yaml.load(annotation)
        if len(ann_memory) == 0:
            prev_id = int(annotation['image_id'])
            new_id = int(annotation['image_id'])
        else:
            new_id = int(annotation['image_id'])
        ann_memory.append(annotation)
        if new_id == prev_id:
            prev_id = int(annotation['image_id'])
            continue
        else:
            print("Read all annotations for: " + str(prev_id) + " (" + str(counter) + "/" + str(len_ann) + ")")
            a = ann_memory.pop(-1)
            ann_all.append(ann_memory)
            prev_id = a['image_id']
            ann_memory = []
            ann_memory.append(a)
    
    # os.system('rm coco_data/*')
    for annotation_set in ann_all:
        create_xml("coco_data", annotation_set)
    post_process_xml("coco_data")
