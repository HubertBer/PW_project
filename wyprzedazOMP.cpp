#include <iostream>
#include <vector>
#include <map>
#include <queue>
#include <algorithm>
#include <thread>
#include <chrono>
#include <iomanip>
#include <omp.h>
using namespace std;

int THREADS_NUM = 8;
int LOG2_THREADS_NUM  = 3;
int PEEK_THREADS_NUM  = 3;
int FAITH_THREADS_NUM = 3;

struct segment{
    int l;
    int r;
    int val;

    friend bool operator<(const segment& s1, const segment& s2){
        if(s1.r < s2.r)return false;
        if(s1.r > s2.r)return true;
        if(s1.l <= s2.l)return false;
        return true;
    }
};

bool canTake(vector<segment>& take){
    sort(take.begin(), take.end(), [](const segment& s1, const segment& s2){
        if(s1.l < s2.l)return true;
        return false;
    });

    int i = 0;
    int now = take[i].l;

    priority_queue<segment> q;
    while(i < take.size() && take[i].l == now){
        q.push(take[i]);
        ++i;
    }

    while(true){
        while(!q.empty()){
            auto s = q.top(); q.pop();
            if(s.r <= now)return false;
            
            ++now;
            while(i < take.size() && take[i].l == now){
                q.push(take[i]);
                ++i;
            }
        }

        if(i >= take.size())return true;
        now = take[i].l;
        while(i < take.size() && take[i].l == now){
            q.push(take[i]);
            ++i;
        }
    }

    return true;
}

void case1(){
    int n;
    cin>>n;
    vector<segment> data(n);

    for(int i = 0; i < n; ++i)
        cin>>data[i].l>>data[i].r>>data[i].val;

    using std::chrono::high_resolution_clock;
    using std::chrono::duration_cast;
    using std::chrono::duration;
    using std::chrono::milliseconds;
    auto t1 = high_resolution_clock::now();
    
    sort(data.begin(), data.end(), [](const segment& s1, const segment& s2){
        return s1.val >= s2.val;
    });


    vector<bool> taken(n, false);
    vector<vector<bool>> mask(THREADS_NUM, vector<bool>(LOG2_THREADS_NUM, false));
    for(int k = 1; k < THREADS_NUM; ++k)
        for(int j = 0; j < LOG2_THREADS_NUM; ++j)
            if((k >> j)&1)mask[k][LOG2_THREADS_NUM - 1 - j] = true;
    
    int takenCount = 0;
    vector<vector<unsigned char>> canDo(n, vector<unsigned char>(THREADS_NUM, 0));

    int i = 0;

    omp_set_num_threads(THREADS_NUM);
    for(; i + LOG2_THREADS_NUM - 1 < n; i += LOG2_THREADS_NUM){    
        //Thourough checking
        #pragma omp parallel for
        for(int k = THREADS_NUM - 1; k > 0; --k){
            vector<segment> toTake;
            toTake.reserve(takenCount + LOG2_THREADS_NUM);
            
            for(int j = 0; j < i; ++j)
                if(taken[j])toTake.emplace_back(data[j]);
            
            for(int j = 0; j < LOG2_THREADS_NUM; ++j)
                if(mask[k][j])
                    toTake.emplace_back(data[i + j]);
                
            canDo[i][k] = canTake(toTake);
        }
        
        int bestJ = 0;
        for(int j = THREADS_NUM - 1; j > 0; --j){
            if(!canDo[i][j])continue;
            bestJ = j;
            break;
        }

        for(int j = 0; j < LOG2_THREADS_NUM ; ++j){
            taken[i + j] = mask[bestJ][j];
            takenCount += mask[bestJ][j];    
        }
    }

    for(; i < n; ++i){
        vector<segment> toTake;
        toTake.reserve(takenCount + 1);
        
        for(int j = 0; j < i; ++j)
            if(taken[j])toTake.emplace_back(data[j]);
        
        toTake.emplace_back(data[i]);
            
        taken[i] = canTake(toTake);
    }

    int sum = 0;
    for(int i = 0; i < n; ++i)
        sum += taken[i] * data[i].val;
    
    auto t2 = high_resolution_clock::now();

    cout<<sum<<endl;

    duration<double, std::milli> ms_double = t2 - t1;
    cerr << setprecision(6) << ms_double.count() << "ms\n";
}

int main(int argc, char* argv[]){
    if(argc > 1) LOG2_THREADS_NUM = atoi(argv[1]);
    if(argc > 2) PEEK_THREADS_NUM = atoi(argv[2]);
    if(argc > 3) FAITH_THREADS_NUM = atoi(argv[3]);
    
    if(argc > 1) THREADS_NUM = 1 << LOG2_THREADS_NUM;
    ios_base::sync_with_stdio(0);
    cin.tie(0);
    int z = 1;
    cin>>z;
    while (z--)case1();
    return 0;
}