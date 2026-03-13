#include <vector>
#include <string>
using namespace std;

class Instance {
    public:
        int n_jobs;
        int n_machines;
        vector<vector<int>> processing_times;
        void loadFile(string filename);
};