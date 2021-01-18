from pycocotools.coco import COCO
import pandas as pd
import shutil
import os

def create_data_subset(folder_name, category):
  # note this only refers to  the training set and not the validation set
  coco = COCO('annotations/instances_train2017.json')  

  # note this only refers to the captions of the training set and not the validation set   
  caps = COCO('annotations/captions_train2017.json') 

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

  # Create empty dataframe with two columns for the image file name and the corresponding captions
  df = pd.DataFrame(columns=['image_id', 'caption'])

  # Create folder in for the images of the selected category
  os.mkdir(folder_name)

  # Create map for image id (key) to captions (values)
  captions_dict = {}
  for i, n in enumerate(annotations):
      captions_dict[annotations[i][0]['image_id']] = annotations[i]

  horse_file_names = []
  for img in images:
      print("New image found:", img)
      horse_file_names.append(img['file_name'])
      for entry in captions_dict[img['id']]:
          df.loc[len(df)] = [img['file_name'], entry['caption']]

  # Convert dataframe to csv file and save to folder
  df.to_csv(folder_name + "/captions.csv", index=False)

  # Copy all images of given category to new folder
  for filename in os.listdir('train2017'):
      print("Checking for", filename)
      if filename in horse_file_names:
          print("Approved! Copying", filename)
          shutil.copy(os.path.join('train2017', filename), folder_name)

  print('Done creating data subset with images....')

if __name__ == '__main__':
  create_data_subset(folder_name='person_images', category='person')
