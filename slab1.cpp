#include<iostream>
#include<vector>
#include<cstring>
#include<string>
#include<cmath>
#include<bitset>
#include<algorithm>
#include<slab.h>

using namespace std;

SSDCACHE::SSDCACHE(int maxn = 0, int max_read = 0){
    this->maxn = maxn;
    this->max_read = max_read;
    for(int i = 3; i <= 10; i++){
        slabclass.slab_cnt = 0;
        slabclass.full = 0;
        slabclass.size = pow(2, i); 
        slabclass.single = 4 * 1024 / pow(2, i);
        slablist.push_back(slabclass); 
    }
}

bool SSDCACHE::addnewslab(int size){
    if(cnt == maxn)
        return false;
    int no = log(size / 8) / log(2);
    node.number = 0;
    node.serial = ++slab_number;
    slablist[no].slab.push_back(node);
    slablist[no].full++;
    locate.serial = slab_number;
    locate.size = size;
    slablru.insert(slablru.begin(), locate);
    cnt++;
    return true;
}

int SSDCACHE::countsize(int size){
    return log(size / 8) / log(2);
}

int SSDCACHE::findkey(int no, const string &key){
    for(int i = 0; i < slablist[no].full; i++){
        for(auto j : slablist[no].slab[i].keyset){
            if(j == key)
                return slablist[no].slab[i].serial;
        }
    }
    return -1;
}

void SSDCACHE::halved(){
    for(auto i : slablru)
        i.read_cnt /= 2;
    return ;
}

void SSDCACHE::put(const string &key, int size, int &full){
    full = 0;
    int no = log(size / 8) / log(2);
    int seri = findkey(no, key);
    if(seri == -1){
        if(slablist[no].slab_cnt == slablist[no].full){
            full = addnewslab(size);
            if(!full){
                full = -1;
                return ;
            }
            slablist[no].slab[slablist[no].slab_cnt].keyset.push_back(key);
            slablist[no].slab[slablist[no].slab_cnt].number++;
            if(slablist[no].slab[slablist[no].slab_cnt].number == slablist[no].single)
                slablist[no].slab_cnt++;
        }else{
            int t = slablist[no].slab[slablist[no].slab_cnt].serial;
            for(int i = 0; i < cnt; i++){
                if(slablru[i].serial == seri){
                    slablru.insert(slablru.begin(), slablru[i]);
                    slablru.erase(slablru.begin() + (i + 1));
                }
            }
            slablist[no].slab[slablist[no].slab_cnt].keyset.push_back(key);
            slablist[no].slab[slablist[no].slab_cnt].number++;
            if(slablist[no].slab[slablist[no].slab_cnt].number == slablist[no].single)
                slablist[no].slab_cnt++;
        }
    }else{
        for(int i = 0; i < cnt; i++){
            if(slablru[i].serial == seri){
                slablru.insert(slablru.begin(), slablru[i]);
                slablru.erase(slablru.begin() + (i + 1));
            }
        }
    }

}

vector<string> SSDCACHE::get(const string &key, int size, int &maxread, bool isput = false){
    vector<string> vec;
    maxread = 0;
    int no = log(size / 8) / log(2);
    int flag = 0;
    int itrFind = findkey(no, key);
    if(itrFind == -1){
        maxread = -1;
        return vec;
    }else{
        int nums = slablru.size();
        for(int i = 0; i < nums; i++){
            if(slablru[i].serial == itrFind && isput == true){
                for(auto j : slablist[no].slab){
                    if(slablru[i].serial == itrFind){
                        slablru[i].read_cnt++;
                        if(slablru[i].read_cnt == max_read){
                            slablru[i].read_cnt = 0;
                            vec = j.keyset;
                        }
                    }
                }
            }
            slablru.insert(slablru.begin(), slablru[i]);
            slablru.erase(slablru.begin() + (i + 1));
            break;
        }
    }
    return vec;
}

vector<string> SSDCACHE::del(int &slab_no){
    vector<string> vec;
    int No = slablru.size() - 1;
    int serials = slablru[No].serial;
    int sizes = slablru[No].size;
    slablru.pop_back();
    int no = log(sizes / 8) / log(2);
    for(int i = 0; i < slablist[no].slab.size(); i++){
        if(slablist[no].slab[i].serial == serials){
            if(slablist[no].slab[i].number == slablist[no].single)
                slablist[no].slab_cnt--;
            slab_no = serials;
            slablist[no].full--;
            vec = slablist[no].slab[i].keyset;
            slablist[no].slab.erase(slablist[no].slab.begin() + i);
            cnt--;
            break;
        }
    }
    return vec;
}

SSDCACHE::~SSDCACHE(){
    slablist.clear();
    slablru.clear();
}


//test pressure
int main()
{
    SSDCACHE ssdcache(2);
    int slab_no, offset;
    for(int i = 0; i < 513; i++){
        string s = to_string(i);
        if(i == 512){
            int a = 999999;
            int b = 999;
            int c = 11;
        }
        ssdcache.put(s, 8, slab_no);
    }
    for(int i = 1000; i < 1256; i++){
        string s = to_string(i);
        if(i == 512){
            int a = 999999;
            int b = 999;
            int c = 11;
        }
        ssdcache.put(s, 16, slab_no);
        if(slab_no == -1){
            vector<string> vec = ssdcache.del(slab_no);
            for(auto j : vec)
                cout << j << endl;
        }
        //cout << slab_no << endl;
    }
    ssdcache.get("1001", 16, slab_no, true);
    cout << endl;
    cout << slab_no << endl;
    return 0;
}