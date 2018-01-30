#include "familia/semantic_matching.h"
#include "familia/util.h"
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <tuple>

#include <gflags/gflags.h>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <vector>

using std::string;
using std::unordered_map;
using std::vector;

const double EPS = 1e-8;
namespace py = pybind11;

namespace familia {
struct WordCount
{
    int word_id;
    int count;
    WordCount(int id, int cnt) : word_id(id), count(cnt) {}
};

struct WordCountCmpGreater
{
    bool operator()(const WordCount &w1, const WordCount &w2)
    {
        return w1.count > w2.count;
    }
} word_count_cmp_greater;

// 加载词典信息
void load_vocabulary(
    const string &vocabulary_path,
    unordered_map<int, string> &vocabulary)
{
    std::ifstream infile(vocabulary_path);
    string line;

    while (std::getline(infile, line))
    {
        // 词典每行格式: factor_name term word_id tf df
        vector<string> items;
        split(items, line, '\t');
        // 需要保证至少有前三列
        CHECK_GE(items.size(), 3);
        string word = items[1];
        string id = items[2];
        // 加入词典
        vocabulary.insert({std::stoi(id), word});
    }
    infile.close();
}

void load_item_topic_table(
    const string &item_topic_table_path,
    vector<int64_t> &topic_sum_table,
    vector<vector<WordCount>> &topic_words)
{
    std::ifstream infile(item_topic_table_path);
    string line;

    while (std::getline(infile, line))
    {
        vector<string> items;
        split(items, line, ' ');
        CHECK_GE(items.size(), 2);

        int word_id = std::stoi(items[0]);
        for (size_t i = 1; i < items.size(); i++)
        {
            vector<string> temps;
            split(temps, items[i], ':');
            CHECK_EQ(temps.size(), 2);
            int topic_index = std::stoi(temps[0]);
            int count = std::stoi(temps[1]);
            // 统计每个主题下词的总数
            topic_sum_table[topic_index] += count;
            WordCount current_word_count(word_id, count);
            topic_words[topic_index].push_back(current_word_count);
        }
    }
    infile.close();
}


  class TopicTable
{
  public:
    TopicTable(int num_topics,const string &vocabulary_path,const string &item_topic_table_path) :_num_topics(num_topics)      
    {
       
        _topic_sum_table.resize(_num_topics);
        _topic_words.resize(_num_topics);
        load_vocabulary(vocabulary_path, _vocabulary);
        load_item_topic_table(item_topic_table_path, _topic_sum_table, _topic_words);
        // 排序
        for (int i = 0; i < num_topics; i++)
        {
            std::sort(_topic_words[i].begin(), _topic_words[i].end(), word_count_cmp_greater);
        }
    };

    static TopicTable create(int num_topics,
               const string &vocabulary_path,
               const string &item_topic_table_path) { return TopicTable( num_topics,
               vocabulary_path,
               item_topic_table_path); };

    ~TopicTable() = default;

    // 打印指定主题下的前k个词
    std::vector<std::tuple<std::string, float>> show_topics(int topic_id, int k)
    {
        std::vector<std::tuple<std::string, float>> result;
        if (topic_id >= 0 && topic_id < _num_topics)
        {
            k = std::min(k, static_cast<int>(_topic_words[topic_id].size()));

            for (int i = 0; i < k; i++)
            {
                float prob = (_topic_words[topic_id][i].count * 1.0) /
                             (_topic_sum_table[topic_id] + EPS);
                std::string word = _vocabulary[_topic_words[topic_id][i].word_id];
                std::tuple<std::string, float> item = std::make_tuple(word, prob);
                result.push_back(item);                
                // cout << _vocabulary[_topic_words[topic_id][i].word_id] << "\t" << prob << endl;
            }
        }
        else
        {
            LOG(ERROR) << topic_id << " is out of range!";
        }
        return result;
    };

  private:
    int _num_topics;
    vector<int64_t> _topic_sum_table;
    unordered_map<int, string> _vocabulary;
    vector<vector<WordCount>> _topic_words;
};
};

PYBIND11_MODULE(topictable, m)
{
    py::class_<familia::TopicTable>(m, "TopicTable")
        .def(py::init(&familia::TopicTable::create))
        .def("topic_words", &familia::TopicTable::show_topics);
}
// st_demo.show_topics(t_id, FLAGS_top_k);