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

struct CACHE{
    int maxn;
    int cnt;
    vector<KV> lru;
}cac;

/*
    the structure of the slabclass which diffs from 8 Byte to 1024 Byte
    the size represents the size of the Class
*/
struct SlabClass{
    int size;  //the size of the k-v
    vector<Node> slab;
    vector<KV> lru;
    vector<KV> free_node;
}slabclass;

struct SSDCACHE{
    vector<SlabClass> slablist;
    int single[class];  
    int sum[class];
    int cnt[class];
    vector<CACHE> cache;
    SSDCACHE(){
        for(int i = 3; i <= 10; i++){
            cnt[i - 3] = 0;
            slabclass.size = pow(2, i); 
            slablist.push_back(slabclass); 
            cac.maxn = 4 * 1024 / pow(2, i);
            cac.cnt = 0;
            cache.push_back(cac);
        }
    }

    int countsize(int size){
        return log(size / 8) / log(2);
    }

    int findkey(int no, const string &key, int &flag){
        int sum = slablist[no].lru.size();
        for(int i = 0; i < sum; i++){
            if(slablist[no].lru[i].key == key){
                return i;
            }
        }
        sum = cache[no].lru.size();
        for(int i = 0; i < sum; i++){
            if(cache[no].lru[i].key == key){
                flag = 1;
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
            for(int j = 0; j < mapnum; j++){
                kv.slab_no = i;
                kv.offset = j;
                slablist[no].free_node.push_back(kv);
            }
        }
        return ;
    }

    void put(const string &key, int size, int &slab_no, int &offset){
        int no = log(size / 8) / log(2);
        get(key, size, slab_no, offset);
        if(slab_no == -1){
            if(cnt[no] == sum[no]){
                string cachekey = slablist[no].lru[cnt[no] - 1].key;
                int full = putcache(cachekey, size);
                if(full == 1){
                    slab_no = -2;
                    return ;
                }
                kv.key = key;
                kv.slab_no = slablist[no].lru[cnt[no] - 1].slab_no;
                kv.offset = slablist[no].lru[cnt[no] - 1].offset;
                slab_no = kv.slab_no;
                offset = kv.offset;
                slablist[no].lru.pop_back();
                slablist[no].lru.insert(slablist[no].lru.begin(), kv);
            }else{
                slab_no = slablist[no].free_node[0].slab_no;
                offset = slablist[no].free_node[0].offset;
                slablist[no].slab[slab_no].bitmap.set(offset);
                kv.key = key;
                kv.slab_no = slab_no;
                kv.offset = offset;
                slablist[no].free_node.erase(slablist[no].free_node.begin());
                slablist[no].lru.insert(slablist[no].lru.begin(), kv);
                cnt[no]++;
            }
        }else{
            ;
        }

    }

    void get(const string &key, int size, int &slab_no, int &offset){
        int no = log(size / 8) / log(2);
        int flag = 0;
        int itrFind = findkey(no, key, flag);
        if(itrFind == -1){
            slab_no = -1;
            offset = -1;
            return ;
        }else{
            if(flag == 0){
                kv.key = slablist[no].lru[itrFind].key;
                kv.slab_no = slablist[no].lru[itrFind].slab_no;
                kv.offset = slablist[no].lru[itrFind].offset;
                slab_no = kv.slab_no;
                offset = kv.offset;
                slablist[no].lru.erase(slablist[no].lru.begin() + itrFind);
                slablist[no].lru.insert(slablist[no].lru.begin(), kv);
            }
            else{
                delcache(itrFind, size);
                put(key, size, slab_no, offset);
            }
        }
    }

    void del(const string &key, int size, int &slab_no, int &offset){
        int no = log(size / 8) / log(2);
        int flag = 0;
        int itrFind = findkey(no, key, flag);
        if(itrFind == -1){
            slab_no = -1;
            offset = -1;
            return ;
        }else{
            if(flag == 1){
                delcache(itrFind, size);
                return ;
            }
            kv.key = slablist[no].lru[itrFind].key;
            kv.slab_no = slablist[no].lru[itrFind].slab_no;
            kv.offset = slablist[no].lru[itrFind].offset;
            slablist[no].lru.erase(slablist[no].lru.begin() + itrFind);
            slablist[no].free_node.push_back(kv);
            slablist[no].slab[kv.slab_no].bitmap.reset(kv.offset);
            cnt[no]--;
        }
    }

    int putcache(const string &key, int size){
        int no = log(size / 8) / log(2);
        if(cache[no].cnt == cache[no].maxn)
            return 1;
        kv.key = key;
        kv.slab_no = -1;
        kv.offset = -1;
        cache[no].lru.insert(cache[no].lru.begin(), kv);
        cache[no].cnt++;
        return 0;
    }

    void delcache(int itrFind, int size){
        int no = log(size / 8) / log(2);
        cache[no].lru.erase(cache[no].lru.begin() + itrFind);
        cache[no].cnt--;
    }

    void delcachelist(int size){
        int no = log(size / 8) / log(2);
        cache[no].lru.clear();
    }

    ~SSDCACHE(){
        slablist.clear();
        cache.clear();
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
    int slab_no, offset;
    for(int i = 0; i < 1000; i++){
        string s = to_string(i);
        if(i == 512){
            int a = 999999;
            int b = 999;
            int c = 11;
        }
        ssdcache.put(s, 16, slab_no, offset);
        cout << slab_no << endl;
    }
    ssdcache.get("800", 16, slab_no, offset);
    cout << offset << " " << slab_no << endl;
    return 0;
}