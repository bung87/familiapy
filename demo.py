#coding=utf-8

# Copyright (c) 2017, Baidu.com, Inc. All Rights Reserved
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

import sys,os

from familiapy.topictable import TopicTable
from familiapy.api import InferenceEngineWrapper
from familiapy.api import TopicalWordEmbeddingsWrapper

if __name__ == '__main__':
    model_dir = os.path.join( os.path.dirname(__file__),'Familia/model/weibo')
    conf_file = "slda.conf"
    emb_file = "weibo_slda.model"
    # topic_words = os.path.join(os.path.dirname(__file__) ,'Familia/model/topic_words.slda.conf')

    inference_engine_wrapper = InferenceEngineWrapper(model_dir,conf_file,emb_file)
    
    vocpath =  os.path.join(model_dir,"vocab_info.txt") 
    topicpath = os.path.join(model_dir,"weibo_slda.model") 

    topic = TopicTable(2000,vocpath,topicpath)
    # twe_wrapper = TopicalWordEmbeddingsWrapper(model_dir, emb_file)
    while True:
        input_text = input("Enter Document: ")
        # 分词
        seg_list = inference_engine_wrapper.tokenize(input_text.strip())
        # 构建句子结构,5个词为一个句子
        sentences = []
        length = len(seg_list)
        for index in range(0, length, 5):
            sentences.append(seg_list[index: index + 5])
        # 进行推断
        topic_dist = inference_engine_wrapper.slda_infer(sentences)
        # 打印结果
        print("Document Topic Distribution:")
        print(topic_dist)
        for t in topic_dist:
            print(topic.topic_words(t[0],10) )
            # result_list = twe_wrapper.nearest_words_around_topic(t[0], 10)
            # print_result(result_list)