import os
import coco_separate_class

def split_annotation(filename, splits):
    annotations = coco_separate_class.read_anns(filename)
    file_len = int(len(annotations) / splits)
    process_count = 0

    for i in range(splits):
        with open(str("annotations_" + str(i) + ".txt"), 'w') as f:
            for ann in annotations:
                if i < splits - 1:
                    process_count += 1
                    f.write("%s\n" % str(ann))
                    if process_count == file_len:
                        annotations = annotations[process_count:]
                        process_count = 0
                        break
                else:
                    f.write("%s\n" % str(ann))


if __name__ == '__main__':
    split_annotation("annotations.txt", 6)
