#include<iostream>
#include<vector>
#include<cstring>
#include<string>
#include<cmath>
#include<bitset>
#include <algorithm>

#define class 8

using namespace std;

/*
    the structure of the slab in the slabclass
    the serial represents the location
*/
struct Node{
    int serial;
    bitset<512> bitmap;
}node;

struct KV{
    string key;
    int slab_no;
    int offset;
}kv;
/*
    the structure of the slabclass which diffs from 8Byte to 1024 Byte
    the size represents the size of the Class
*/
struct SlabClass{
    int size;  //the size of the k-v
    vector<Node> slab;
    vector<KV> lru;
}slabclass;

struct SSDCACHE{
    vector<SlabClass> slablist;
    int single[class];  
    int sum[class];
    int cnt;
    SSDCACHE(){
        cnt = 0;
        for(int i = 3; i <= 10; i++){
            slabclass.size = pow(2, i); 
            slablist.push_back(slabclass); 
        }
    }

    int findkey(int no, const string &key){
        int sum = slablist[no].lru.size();
        for(int i = 0; i < sum; i++){
            if(slablist[no].lru[i].key == key){
                return i;
            }
        }
        return -1;
    }

    /*
        init the ssdcache by the value passed
    */
    void init_ssdcache(int size, int number){
        int no = log(size / 8) / log(2);   //计算位置
        int mapnum = 4 * 1024 / size;
        single[no] = mapnum; 
        sum[no] = mapnum * number;
        for(int i = 0; i < number; i++){
            node.serial = i;
            slablist[no].slab.push_back(node);
        }
        return ;
    }

    void put(const string &key, int size){
        int no = log(size / 8) / log(2);
        int slab_no, offset;
        get(key, size, slab_no, offset);
        if(slab_no == -1){
            if(cnt == sum[no]){
                kv.key = slablist[no].lru[cnt - 1].key;
                kv.slab_no = slablist[no].lru[cnt - 1].slab_no;
                kv.offset = slablist[no].lru[cnt - 1].offset;
                slablist[no].lru.pop_back();
                slablist[no].lru.insert(slablist[no].lru.begin(), kv);
            }else{
                slab_no = cnt / single[no];
                offset = cnt % single[no];
                slablist[no].slab[slab_no].bitmap.set(offset);
                kv.key = key;
                kv.slab_no = slab_no;
                kv.offset = offset;
                slablist[no].lru.insert(slablist[no].lru.begin(), kv);
                cnt++;
            }
        }else{
            ;
        }

    }

    void get(const string &key, int size, int &slab_no, int &offset){
        int no = log(size / 8) / log(2);
        int itrFind = findkey(no, key);
        if(itrFind == -1){
            slab_no = -1;
            offset = -1;
            return ;
        }else{
            kv.key = slablist[no].lru[itrFind].key;
            kv.slab_no = slablist[no].lru[itrFind].slab_no;
            kv.offset = slablist[no].lru[itrFind].offset;
            slab_no = kv.slab_no;
            offset = kv.offset;
            slablist[no].lru.erase(slablist[no].lru.begin() + itrFind);
            slablist[no].lru.insert(slablist[no].lru.begin(), kv);
        }
    }

    void del(const string &key, int size, int &slab_no, int &offset){
        int no = log(size / 8) / log(2);
        int itrFind = findkey(no, key);
        if(itrFind == -1){
            slab_no = -1;
            offset = -1;
            return ;
        }else{
            kv.key = slablist[no].lru[itrFind].key;
            kv.slab_no = slablist[no].lru[itrFind].slab_no;
            kv.offset = slablist[no].lru[itrFind].offset;
            slablist[no].lru.erase(slablist[no].lru.begin() + itrFind);
            slablist[no].slab[kv.slab_no].bitmap.reset(kv.offset);
            cnt--;
        }
    }

    ~SSDCACHE(){
        slablist.clear();
    }
};


//test pressure
int main()
{
    SSDCACHE ssdcache;
    ssdcache.init_ssdcache(8, 2);
    ssdcache.init_ssdcache(16, 2);
    ssdcache.init_ssdcache(32, 2);
    ssdcache.init_ssdcache(64, 2);
    ssdcache.init_ssdcache(128, 2);
    ssdcache.init_ssdcache(256, 2);
    ssdcache.init_ssdcache(512, 2);
    ssdcache.init_ssdcache(1024, 2);
    for(int i = 0; i < 1000; i++){
        string s = to_string(i);
        ssdcache.put(s, 16);
    }
    int slab_no, offset;
    ssdcache.get("511", 16, slab_no, offset);
    cout << slab_no << " " << offset << endl;
    return 0;
}