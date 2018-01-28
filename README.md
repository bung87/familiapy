# familiapy

## Installation

```
    git clone --recurse-submodules https://github.com/bung87/familiapy
    make && make install
```  
you may specify gcc and g++ higher version since dependencies rely on c++11 supported compiler.  
eg. on mine macbook:   
`CC=gcc-6 CXX=g++-6 make && make install`  

your may check your gcc version by tpye `gcc -v` probaly is 4.2,
then use *homebrew* `brew install gcc@6`

