#include <iostream>
#include <vector>
#include <queue>

using namespace std;

class Solution {
public:
    int minRefuelStops(int target, int startFuel, vector<vector<int>>& stations) {
        priority_queue<int> maxHeap;

        int currentFuel = startFuel;
        int stops = 0;
        int i = 0;
        //пока топлива не хватает чтобы доехать
        while (currentFuel < target) {
            while (i < stations.size() && stations[i][0] <= currentFuel/*можем ли до неё доехать*/) {
                maxHeap.push(stations[i][1]);
                i++;
            }

            if (maxHeap.empty()) {
                return -1;             }
            //заправляемся
            currentFuel += maxHeap.top();
            maxHeap.pop();
            stops++;
        }

        return stops;
    } 
};




