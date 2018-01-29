#include <pybind11/pybind11.h>

#include "familia/inference_engine.h"
#include "familia/tokenizer.h"
#include "familia/semantic_matching.h"

namespace py = pybind11;
using namespace familia;
using namespace std;

InferenceEngine *init_inference_engine(const string &model_dir, const string &conf, SamplerType samplerType)
{
        InferenceEngine *engine;
        engine = new InferenceEngine(model_dir, conf, samplerType);
        return engine;
}

void destroy_inference_engine(InferenceEngine &infer_ptr)
{
        InferenceEngine *inference_engine = &infer_ptr;
        delete (inference_engine);
}

Tokenizer *init_tokenizer(const string &vocab_file)
{
        Tokenizer *tokenizer = new SimpleTokenizer(vocab_file);
        return tokenizer;
}

void destroy_tokenizer(Tokenizer &tokenizer_ptr)
{
        Tokenizer *tokenizer = &tokenizer_ptr;
        delete (tokenizer);
}
TopicalWordEmbedding *init_twe(const string &model_dir, const string &emb_file)
{
        TopicalWordEmbedding *twe = new TopicalWordEmbedding(model_dir, emb_file);
        return twe;
}

void destroy_twe(TopicalWordEmbedding &twe_ptr)
{
        TopicalWordEmbedding *twe = &twe_ptr;
        delete (twe);
}

vector<string> tokenize(Tokenizer &tokenizer_ptr, const string &input)
{
        string str = input;
        vector<string> result;
        Tokenizer *tokenizer = &tokenizer_ptr;
        tokenizer->tokenize(str, result);
        return result;
}

vector<Topic> lda_infer(InferenceEngine &infer_ptr, const string &input)
{
        LDADoc doc;
        vector<string> input_vec;
        split(input_vec, input, ' ');
        InferenceEngine *inference_engine = &infer_ptr;
        inference_engine->infer(input_vec, doc);
        vector<Topic> topics;
        doc.sparse_topic_dist(topics);
        return topics;
}

vector<Topic> slda_infer(InferenceEngine &infer_ptr, const string &input)
{
        vector<string> input_vec;
        vector<vector<string>> sentences;
        split(input_vec, input, '\t');
        for (size_t i = 0; i < input_vec.size(); ++i)
        {
                vector<string> sent;
                // 句子中词与词之间用空格隔开
                split(sent, input_vec[i], ' ');
                sentences.push_back(sent);
        }

        SLDADoc doc;
        InferenceEngine *inference_engine = &infer_ptr;
        inference_engine->infer(sentences, doc);
        vector<Topic> topics;
        doc.sparse_topic_dist(topics);
        return topics;
}

vector<float> cal_doc_distance(InferenceEngine &infer_ptr, const string &doc_text1, const string &doc_text2)
{
        InferenceEngine *inference_engine = &infer_ptr;
        vector<string> doc1_tokens;
        vector<string> doc2_tokens;
        split(doc1_tokens, doc_text1, ' ');
        split(doc2_tokens, doc_text2, ' ');

        // 文档主题推断, 输入分词结果，主题推断结果存放于LDADoc中
        LDADoc doc1, doc2;
        inference_engine->infer(doc1_tokens, doc1);
        inference_engine->infer(doc2_tokens, doc2);

        // 计算jsd需要传入稠密型分布
        // 获取稠密的文档主题分布
        vector<float> dense_dist1;
        vector<float> dense_dist2;
        doc1.dense_topic_dist(dense_dist1);
        doc2.dense_topic_dist(dense_dist2);

        // 计算分布之间的距离, 值越小则表示文档语义相似度越高
        float jsd = SemanticMatching::jensen_shannon_divergence(dense_dist1, dense_dist2);
        float hd = SemanticMatching::hellinger_distance(dense_dist1, dense_dist2);
        vector<float> r = {jsd, hd};
        return r;
}

vector<float> cal_query_doc_similarity(InferenceEngine &infer_ptr, TopicalWordEmbedding &twe_ptr,
                                            const string &query, const string &document)
{
        InferenceEngine *inference_engine = &infer_ptr;
        TopicalWordEmbedding *twe = &twe_ptr;
        vector<string> q_tokens;
        vector<string> doc_tokens;
        split(q_tokens, query, ' ');
        split(doc_tokens, document, ' ');

        // 对长文本进行主题推断，获取主题分布
        LDADoc doc;
        inference_engine->infer(doc_tokens, doc);
        vector<Topic> doc_topic_dist;
        doc.sparse_topic_dist(doc_topic_dist);

        // 计算在LDA跟TWE模型上的相关性
        float lda_sim = SemanticMatching::likelihood_based_similarity(q_tokens,
                                                                      doc_topic_dist,
                                                                      inference_engine->get_model());
        float twe_sim = SemanticMatching::twe_based_similarity(q_tokens, doc_topic_dist, *twe);
        vector<float> r = {lda_sim, twe_sim};
        return r;
}
vector<WordAndDis> cal_keywords_similarity(InferenceEngine &infer_ptr, const string words, const string document)
{
        InferenceEngine *inference_engine = &infer_ptr;
        vector<string> word_tokens;
        vector<string> doc_tokens;
        split(word_tokens, words, ' ');
        split(doc_tokens, document, ' ');

        LDADoc doc;
        inference_engine->infer(doc_tokens, doc);
        vector<Topic> doc_topic_dist;
        doc.sparse_topic_dist(doc_topic_dist);

        vector<WordAndDis> items;
        for (auto word : word_tokens)
        {
                WordAndDis wd;
                wd.word = word;
                vector<string> tmp;
                tmp.push_back(word);
                wd.distance = SemanticMatching::likelihood_based_similarity(tmp,
                                                                            doc_topic_dist,
                                                                            inference_engine->get_model());
                items.push_back(wd);
        }
        return items;
}
vector<WordAndDis> cal_keywords_twe_similarity(InferenceEngine &infer_ptr, TopicalWordEmbedding &twe_ptr, const string words, const string document)
{
        InferenceEngine *inference_engine = &infer_ptr;
        TopicalWordEmbedding *twe = &twe_ptr;
        vector<string> word_tokens;
        vector<string> doc_tokens;
        split(word_tokens, words, ' ');
        split(doc_tokens, document, ' ');

        LDADoc doc;
        inference_engine->infer(doc_tokens, doc);
        vector<Topic> doc_topic_dist;
        doc.sparse_topic_dist(doc_topic_dist);

        vector<WordAndDis> items;
        for (auto word : word_tokens)
        {
                WordAndDis wd;
                wd.word = word;
                vector<string> tmp;
                tmp.push_back(word);
                wd.distance = SemanticMatching::twe_based_similarity(tmp, doc_topic_dist, *twe);
                items.push_back(wd);
        }
        return items;
}
vector<WordAndDis> nearest_words(TopicalWordEmbedding &twe_ptr, const string &word, const int &k)
{
        TopicalWordEmbedding *twe = &twe_ptr;
        vector<WordAndDis> items(k);
        twe->nearest_words(word, items);
        return items;
}
vector<WordAndDis> nearest_words_around_topic(TopicalWordEmbedding &twe_ptr, const int &topic_id, const int &k)
{
        TopicalWordEmbedding *twe = &twe_ptr;
        vector<WordAndDis> items(k);
        twe->nearest_words_around_topic(topic_id, items);
        return items;
}
PYBIND11_MODULE(familia, m)
{
        m.def("init_inference_engine", &init_inference_engine)
            .def("destroy_inference_engine", &destroy_inference_engine)
            .def("init_tokenizer", &init_tokenizer)
            .def("init_twe", &init_twe)
            .def("destroy_twe", &destroy_twe)
            .def("tokenize", &tokenize)
            .def("lda_infer", &lda_infer)
            .def("slda_infer", &slda_infer)
            .def("cal_doc_distance", &cal_doc_distance)
            .def("cal_query_doc_similarity", &cal_query_doc_similarity)
            .def("cal_keywords_similarity", &cal_keywords_similarity)
            .def("cal_keywords_twe_similarity", &cal_keywords_twe_similarity)
            .def("nearest_words", &nearest_words)
            .def("nearest_words_around_topic", &nearest_words_around_topic);
}