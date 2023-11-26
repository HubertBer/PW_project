#include <iostream>
#include <vector>
#include <map>
#include <queue>
#include <algorithm>
#include <thread>
#include <chrono>
#include <iomanip>
using namespace std;

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
    int takenCount = 0;    

    for(int i = 0; i < n; ++i){
        taken[i] = true;
        vector<segment> toTake;
        toTake.reserve(takenCount + 1);
        
        for(int j = 0; j <= i; ++j)
            if(taken[j])toTake.emplace_back(data[j]);

        taken[i] = canTake(toTake);
        takenCount += taken[i];
    }

    int sum = 0;
    for(int i = 0; i < n; ++i)
        sum += taken[i] * data[i].val;
    
    auto t2 = high_resolution_clock::now();

    cout<<sum<<'\n';


    duration<double, std::milli> ms_double = t2 - t1;
    cerr << setprecision(6) << ms_double.count() << "ms\n";
}

int main(){
    ios_base::sync_with_stdio(0);
    cin.tie(0);
    int z = 1;
    cin>>z;
    while (z--)case1();
    return 0;
}