//
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <time.h>
#include <cmath>
#include <random>
#include <functional>
#include <chrono>

using namespace std;


double min_vals(double x, double y) {
    return min(x, y);
}

inline void min_col(vector<vector<double>>& Mat, int i_min, int j_min)
{
    int dist_mat_size = (int)Mat.size();

    for (int i = 0; i < dist_mat_size; i++) {
        double min_val = min(Mat[i][i_min], Mat[i][j_min]);
        if (min_val != -1) {
            Mat[i][i_min] = min_val; //update min value
            Mat[i][j_min] = -1; //flag value as merged, to be ignored later.
        }
    }
}


class HAC {
public:
    void hac(vector<vector<double>>& Mat, int num_of_clusters) {
        
        init(Mat, num_of_clusters);
        
        auto start = chrono::steady_clock::now();
        cout << "Clustering...";
        
        //run clustering
        run_clustering();

        auto end = chrono::steady_clock::now();
        cout << "Time for clustering: "
            << chrono::duration_cast<chrono::nanoseconds>(end - start).count()/pow(10,9)
            << " [SEC]\n" << endl;
        
        print_clusters();
    }
private:
    int K;
    int num_of_vectors;
    vector<vector<double>> Mat;
    vector<vector<int>> clusters;

    void init(vector<vector<double>>& Mat, int num_of_clusters) {
        this->Mat = Mat;
        this->num_of_vectors = (int)Mat.size();
        this->K = num_of_clusters;
        this->clusters.clear();
    }

    void run_clustering() {
        int rows = (int)Mat.size();
        vector<double> nullvec(rows, -1); //tmp vec to replace row and col [merge]
        vector<double> init_vec(2);
        // initialize vec of vecs to be rowsX2
        vector<vector<double>> min_idx_val(rows, vector<double>(init_vec));
        clusters.resize(num_of_vectors);

        for (int i = 0; i < num_of_vectors; i++)
            Mat[i][i] = -1;

        for (int i = 0; i < num_of_vectors;i++)
            clusters[i].push_back({ i });
        
        // find minimal values and their indexes for each vector in matrix
        for (int i = 0; i < num_of_vectors; i++) {
            double min_val = DBL_MAX;
            double min_j = 0;
            for (int j = 0; j < num_of_vectors; j++)
                if (Mat[i][j] != -1)
                    if (Mat[i][j] < min_val) {
                        min_val = Mat[i][j];
                        min_j = j;
                    }
            min_idx_val[i][0] = min_j;
            min_idx_val[i][1] = min_val;
        }
        
        int cnt = num_of_vectors;
        while (cnt > K) {
            
            int min_i = 0;
            double min_val = DBL_MAX;
            for (int i = 0; i < num_of_vectors; i++) {
                if (min_idx_val[i][1] != -1)
                    if (min_idx_val[i][1] < min_val) {
                       min_val = min_idx_val[i][1];
                       min_i = i;
                    }
            }
            int min_j = min_idx_val[min_i][0];   // update min index for j

            transform(Mat[min_i].begin(), Mat[min_i].end(), Mat[min_j].begin(), Mat[min_i].begin(), min_vals);
            min_col(Mat, min_i, min_j);


            clusters[min_i].reserve(clusters[min_i].size() + clusters[min_j].size());
            clusters[min_i].insert(clusters[min_i].end(), clusters[min_j].begin(), clusters[min_j].end()); //add values to cluster
            clusters[min_j].clear();

            Mat[min_j] = nullvec; //mark as merged.
            min_idx_val[min_j] = { -1, -1 };
            for (int i = 0; i < num_of_vectors; i++) {
                if(min_idx_val[i][0] != -1 && min_idx_val[i][1])
                    if (min_idx_val[i][0] == min_j)
                        min_idx_val[i][0] = min_i;
            }

            min_val = DBL_MAX;   //find minimal value in vector
            for (int i = 0; i < num_of_vectors; i++) {
                if (Mat[min_i][i] != -1)
                    if (Mat[min_i][i] < min_val) {
                        min_val = Mat[min_i][i];
                        min_j = i;
                    }
            }
            // update vector min_i with minimal value and it's index
            min_idx_val[min_i][0] = min_j;
            min_idx_val[min_i][1] = Mat[min_i][min_j];
            
            cnt--;
        }
    }

    void print_clusters() {
        for (const std::vector<int>& v : clusters)
        {
            if (!v.empty()) {
                cout << "Cluster:" << endl;
                for (int x : v) std::cout << x << ' ';
                std::cout << std::endl;
            }
        }
    }

};

vector<vector<double>> readMatrix();

int main()
{
    auto start = chrono::steady_clock::now();
    cout << "Reading matrix from file..";
    //Read matrix
    vector<vector<double>> Mat = readMatrix();

    auto end = chrono::steady_clock::now();
    cout << "Time reading matrix: "
        << chrono::duration_cast<chrono::nanoseconds>(end - start).count()/ pow(10, 9)
        << " [SEC]" << endl;
    if (Mat.empty())
        return 0;

    // Call the class, and start clustering
    HAC hac;
    hac.hac(Mat, 2);
    
}

vector<vector<double>> readMatrix() {
    std::vector<std::vector<double>> v;
    std::ifstream ifs("data_2_4.txt");
    std::string tempstr;
    double tempint;
    char delimiter;
    while (std::getline(ifs, tempstr)) {
        std::istringstream iss(tempstr);
        std::vector<double> tempv;
        while (iss >> tempint) {
            tempv.push_back(tempint);
            iss >> delimiter;
        }
        v.push_back(tempv);
    }
    return v;
}
