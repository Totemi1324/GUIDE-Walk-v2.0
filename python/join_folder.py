import os
import shutil

def join_image_folders(src_folder, target_folder):
    for filename in os.listdir(src_folder):
        shutil.copy(os.path.join(src_folder, filename), target_folder)

if __name__ == '__main__':
    join_image_folders(src_folder='truck_images', target_folder='car_images')
