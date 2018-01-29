# familiapy
[Familia](https://github.com/baidu/Familia) python binding.

**Familia** is an open-source project, which implements three popular topic models based on the large-scale industrial data. They are Latent Dirichlet Allocation(LDA)、SentenceLDA and Topical Word Embedding(TWE). In addition, **Familia** offers several tools including lda-infer and lda-query-doc-sim. **Familia** could be easily applied to many tasks, such as document classification, document clustering and personalized recommendation. Due to the high cost of model training, we will continue to release well-trained topic models based on the various types of large-scale data.  

## Installation

```
    git clone --recurse-submodules https://github.com/bung87/familiapy
    make && make install
```  

### notice
1. you may specify gcc and g++ higher version since dependencies rely on c++11 supported compiler.  
eg. on mine macbook:   
`CC=gcc-6 CXX=g++-6 make && make install`  

your may check your gcc version by tpye `gcc -v` probaly is 4.2,
then use *homebrew* `brew install gcc@6`

2. your encounter `error: unrecognized command line option '-Wshorten-64-to-32'` during compilation time under system default Python Interpreter(in mine case python2.7),try to use a higher version Python Interpreter(in mine case python3.5)


