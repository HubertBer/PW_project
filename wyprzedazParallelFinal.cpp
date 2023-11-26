#include <iostream>
#include <vector>
#include <map>
#include <queue>
#include <algorithm>
#include <thread>
#include <chrono>
#include <iomanip>
using namespace std;

int THREADS_NUM = 4;
int LOG2_THREADS_NUM  = 2;
int PEEK_THREADS_NUM  = 5;
int FAITH_THREADS_NUM = 4;

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
    vector<unsigned char> cantTake(n, 0);
    vector<unsigned char> faith(FAITH_THREADS_NUM, 0);
    vector<unsigned char> peekToJoin(PEEK_THREADS_NUM, 0);
    // bool canDo[n][THREADS_NUM];

    int i = 0;
    thread threads[THREADS_NUM];
    thread peekThreads[PEEK_THREADS_NUM];
    thread faithThreads[FAITH_THREADS_NUM];

    for(; i < n; i += LOG2_THREADS_NUM){    
      loopBegin:
        //make sure next checks aren't redundant
        for(int j = 0; j < LOG2_THREADS_NUM && i + j < n; ++j){
            int k = i + j;
            if(cantTake[k]){
                while(k > i){
                    swap(data[k-1], data[k]);
                    swap(canDo[k-1], canDo[k]);
                    swap(cantTake[k-1], cantTake[k]);
                    --k;
                }
                ++i;
                if(i == n)goto loopEnd;
                goto loopBegin;
            }
        }

    
        while(i + LOG2_THREADS_NUM - 1 >= n){
            --LOG2_THREADS_NUM;
            THREADS_NUM >>= 1;
        }
        
        // Faith leaps
        int faithMaxK = 0;
        for(int k = 0; k < FAITH_THREADS_NUM && i + LOG2_THREADS_NUM + k < n; ++k, ++faithMaxK){
            //this and next ones are impossible
            if(cantTake[i + LOG2_THREADS_NUM + k])break;
            
            faith[k] = 0;
            faithThreads[k] = thread(
            [&data, &taken, &faith](int i, int k, int takenCount){
                vector<segment> toTake;
                toTake.reserve(takenCount + k + 1 + LOG2_THREADS_NUM);
                
                for(int j = 0; j < i; ++j)
                    if(taken[j])toTake.emplace_back(data[j]);
                
                for(int j = i; j <= i + LOG2_THREADS_NUM + k; ++j)
                    toTake.emplace_back(data[j]);
                
                if(canTake(toTake))
                    faith[k] = 1; 
                // cantTake[i + LOG2_THREADS_NUM + k] = !canTake(toTake);
            }, i, k, takenCount);
        }

        //peeking forward
        for(int k = 0; k < PEEK_THREADS_NUM && i + LOG2_THREADS_NUM + k < n; ++k){
            if(cantTake[i + LOG2_THREADS_NUM + k])continue;
            
            peekToJoin[k] = 1;
            peekThreads[k] = thread(
            [&data, &taken, &cantTake](int i, int k, int takenCount){
                vector<segment> toTake;
                toTake.reserve(takenCount + 1);
                
                for(int j = 0; j < i; ++j)
                    if(taken[j])toTake.emplace_back(data[j]);
                
                toTake.emplace_back(data[i+ LOG2_THREADS_NUM + k]);

                cantTake[i + LOG2_THREADS_NUM + k] = !canTake(toTake);
            }, i, k, takenCount);
            // peekThreads[k].detach();
        }

        //Thourough checking
        for(int k = THREADS_NUM - 1; k > 0; --k){
            
            threads[k] = thread(
            [&data, &taken, &mask, &canDo, &cantTake](int i, int k, int takenCount){
                vector<segment> toTake;
                toTake.reserve(takenCount + LOG2_THREADS_NUM);
                
                for(int j = 0; j < i; ++j)
                    if(taken[j])toTake.emplace_back(data[j]);
                
                for(int j = 0; j < LOG2_THREADS_NUM; ++j){
                    if(mask[k][j]){
                        // if(cantTake[i+j]) return;
                        toTake.emplace_back(data[i + j]);
                    }
                }
                    
                canDo[i][k] = canTake(toTake);
            }, i, k, takenCount);
        }
        
        int bestJ = 0;
        for(int j = THREADS_NUM - 1; j > 0; --j){
            if(bestJ > 0){
                threads[j].detach();
                continue;
            }
            threads[j].join();
            if(!canDo[i][j])continue;
            bestJ = j;
        }

        for(int j = 0; j < LOG2_THREADS_NUM ; ++j){
            taken[i + j] = mask[bestJ][j];
            takenCount += mask[bestJ][j];    
        }

        int newI = i;
        for (int k = 0; k < faithMaxK; ++k){
            faithThreads[k].join();
            if(faith[k]){
                taken[i + LOG2_THREADS_NUM + k] = true;
                ++takenCount;
                ++newI;
            }
        }

        for(int j = 0; j < PEEK_THREADS_NUM; ++j){
            if(!peekToJoin[j])continue;
            peekThreads[j].join();
            peekToJoin[j] = 0;
        }
        // for(int j = 0; j < PEEK_THREADS_NUM && (j + i + LOG2_THREADS_NUM < n); ++j){
        //     // if(cantTake[i + LOG2_THREADS_NUM + j])continue;
        //     if(peekThreads[j].joinable())
        //         peekThreads[j].join();
        // }

        i = newI;
      
    }
  loopEnd:
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
    cin >> z;
    while (z--)case1();
    return 0;
}