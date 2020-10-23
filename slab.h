#include<iostream>
#include<vector>
#include<cstring>
#include<string>
#include<cmath>
#include<bitset>
#include<algorithm>

using namespace std;

#ifndef CIRCLE_H
#define CIRCLE_H

int slab_number = 0;

struct Locate{
    int size;
    int serial;
    int read_cnt;
}locate;

struct Node{
    int serial;
    int number;
    vector<string> keyset;
}node;

struct SlabClass{
    int size; 
    int single;
    int slab_cnt;
    int full;
    vector<Node> slab;
}slabclass;

struct SSDCACHE{
    vector<SlabClass> slablist;
    int maxn;
    int max_read;
    int cnt = 0;
    vector<Locate> slablru;
    SSDCACHE(int maxn, int max_read);
    void halved();
    bool addnewslab(int size);
    int countsize(int size);
    int findkey(int no, const string &key);
    void put(const string &key, int size, int &full);
    vector<string> get(const string &key, int size, int &maxread, bool isput = false);
    vector<string> del(int &slab_no);
private:
    ~SSDCACHE();
};

#endif