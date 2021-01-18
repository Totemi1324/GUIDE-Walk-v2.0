import caffe_pb2
import lmdb

#import PIL.Image
#from StringIO import StringIO
import numpy as np

def check_lmdb_data(lmdb_folder)
    lmdb_env = lmdb.open(lmdb_folder)
    lmdb_txn = lmdb_env.begin()
    lmdb_cursor = lmdb_txn.cursor()

    datum = caffe_pb2.AnnotatedDatum()
    for key, value in lmdb_cursor:
        print("Key: {}", key)
        datum.ParseFromString(value)
        for ann in datum.annotation_group:
            print("Annotation ", ann.group_label)
            for a in ann.annotation:
                print("  instance_id:", a.instance_id)
                print("  bbox:", a.bbox.xmin, a.bbox.xmax, a.bbox.ymin, a.bbox.ymax, a.bbox.label)
