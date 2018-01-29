from .. import familia

class InferenceEngineWrapper:
    """InferenceEngineWrapper

    对InferenceEngine进行封装，避免直接操作tokenizer等对象指针

    Attributes:
        _inference_engine: InferenceEngine对象指针，用来访问infer接口
        _tokenizer: Tokenizer对象指针，用来分词
        _emb_file: TopicalWordEmbeddings模型文件
        _twe: TopicalWordEmbeddings对象指针，如果_emb_file为空则不需创建
    """

    def __init__(self, model_dir, conf_file, emb_file = None):
        """
        初始化InferenceEngine,Tokenizer,TopicalWordEmbeddings对象指针

        Args:
            model_dir: 模型目录
            conf_file: 模型配置文件
            emb_file: TWE模型文件，可选参数
        """
        self._emb_file = emb_file
        self._inference_engine = familia.init_inference_engine(model_dir, conf_file, 1)
        self._tokenizer = familia.init_tokenizer(model_dir + "/vocab_info.txt")
        if self._emb_file is None:
            return
        self._twe = familia.init_twe(model_dir, emb_file)

    def __del__(self):
        """销毁各个对象指针"""
        familia.destroy_inference_engine(self._inference_engine)
        familia.destroy_tokenizer(self._tokenizer)
        if self._emb_file is not None:
            familia.destroy_twe(self._twe)

    def tokenize(self, input_text):
        """分词

        简易的FMM分词工具，只针对主题模型中出现的词表进行正向匹配

        Args:
            input_text: 输入文本，编码需要跟词表编码保持一致

        Returns:
            返回一个list对象，其中每个元素为分词后的结果。
        """
        return familia.tokenize(self._tokenizer, input_text)

    def lda_infer(self, words):
        """LDA模型推断

        使用LDA模型对输入文本进行推断，得到其主题分布

        Args:
            words: 分词后的list结果

        Returns:
            返回一个list对象，存放输入文本对应的稀疏主题分布，list中每个元素为tuple
            每个tuple包含一个主题ID以及该主题对应的概率，按照概率从大到小排序。
            例如：
            [(15, 0.5), (10, 0.25), (1999, 0.25)]
        """
        seg_text = ' '.join(words)
        return familia.lda_infer(self._inference_engine, seg_text)

    def slda_infer(self, sentences):
        """SentenceLDA模型推断

        使用SentenceLDA模型对输入文本进行推断，得到其主题分布

        Args:
            sentences: 其中每个元素为一个list对象，存放一个句子
            例如：
            [[A, B, C], ..., [A, E, F]]

        Returns:
            返回一个list对象，存放输入文本对应的稀疏主题分布，list中每个元素为tuple
            每个tuple包含一个主题ID以及该主题对应的概率，按照概率从大到小排序。
            例如：
            [(15, 0.5), (10, 0.25), (1999, 0.25)]
        """
        seg_text = ""
        for sent in sentences:
            for word in sent:
                seg_text += word + ' '
            seg_text = seg_text.strip() + '\t'
        seg_text = seg_text.strip()
        return familia.slda_infer(self._inference_engine, seg_text)

    def cal_doc_distance(self, doc1, doc2):
        """计算长文本与长文本之间的距离

        计算两个长文本的主题分布之间的距离，包括jensen_shannon_divergence和hellinger_distance

        Args:
            doc1: 输入文档1分词后的list结果
            doc2: 输入文档2分词后的list结果

        Returns:
            返回一个list对象，其中有两个float元素，第一个表示jensen_shannon_divergence距离，
            第二个表示hellinger_distance距离。例如：
            [0.187232, 0.23431]
        """
        doc1 = ' '.join(doc1)
        doc2 = ' '.join(doc2)
        return familia.cal_doc_distance(self._inference_engine,
                                                 doc1,
                                                 doc2)


    def cal_keywords_similarity(self, words, doc):
        """关键词计算

        使用LDA模型计算候选关键词与文档的相关性

        Args:
            words: 候选词，用空格隔开
            doc: 分词后的文档内容

        Returns:
            返回一个list对象，每个元素为关键词以及其与文档相关性。
        """
        return familia.cal_keywords_similarity(self._inference_engine,
                                                words,
                                                doc)

    def cal_keywords_twe_similarity(self, words, doc):
        """关键词计算

        使用TWE模型计算候选关键词与文档的相关性

        Args:
            words: 候选词，用空格隔开
            doc: 分词后的文档内容

        Returns:
            返回一个list对象，每个元素为关键词以及其与文档相关性。
        """
        # 如果没有加载TWE模型无法进行计算
        if self._emb_file is None:
            sys.stderr.write("Need to load Topical Word Embeddings file.\n")
            return
        return familia.cal_keywords_twe_similarity(self._inference_engine,
                                                   self._twe,
                                                   words,
                                                   doc)


    def cal_query_doc_similarity(self, query, doc):
        """计算短文本与长文本之间的相关性

        使用LDA模型和TWE模型分别衡量短文本跟长文本之间的相关性

        Args:
            query: 输入短文本分词后的list结果
            doc: 输入长文本分词后的list结果

        Returns:
            返回一个list对象，其中有两个float元素，第一个表示根据LDA模型得到的相关性，
            第二个表示通过TWE模型衡量得到的相关性。例如：
            [0.397232, 0.45431]
        """
        # 如果没有加载TWE模型无法进行计算
        if self._emb_file is None:
            sys.stderr.write("Need to load Topical Word Embeddings file.\n")
            return
        query = ' '.join(query)
        doc = ' '.join(doc)
        return familia.cal_query_doc_similarity(self._inference_engine,
                                                         self._twe,
                                                         query,
                                                         doc)



