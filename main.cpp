#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cstdlib>

// so i dont have to put std everywhere
using namespace std;

// function stubs forward declaration for reading code sequentially
double leaveOneOutCrossValidation(const vector<vector<double>>& features, const vector<int>& classes, const vector<int>& currentSet, int featureToAdd);
void forwardSelection(const vector<vector<double>>& features, vector<int> classes);
void backwardsElimination(const vector<vector<double>>& features, vector<int> classes);

int main() {
    
    vector<int> classes;                // column from the txt file indicating class (1/2)
    vector<vector<double>> features;    // 2D table of "label: feature1, feature2, etc." for all other columns contain features
    
    string fileName = "";
    int myAlg = 0;

    cout << "Welcome to Jacob Kantor's Feature Selection Algorithm." << endl << endl;
    cout << "Type in the name of the file to test : " << endl;
    cin >> fileName;

    cout << "Type the nubmer of algorithm you want to run." << endl;
    cout << "\t1) Forward Selection" << endl;
    cout << "\t2) Backward Elimination" << endl;
    cin >> myAlg;

    ifstream file(fileName);      // opens the file

    if (!file.is_open()){
        cout << "Error opening the file for reading" << endl;
        return 1;
    }
    
    string line;
    while (getline(file, line)){         // reads one line at a time until eof
        stringstream ss(line);           // stores the row as a string to read one data point at a time
        double data;

        // Stores class at i
        ss >> data;                     // gets the first data point in the current row
        classes.push_back(static_cast<int>(data)); // the first data point in the row identifies the class and can be simplified to an integer

        // Stores a group of features at the same i
        vector<double> row;             // will store the rest of the remaining data in the current row
        while (ss >> data){             // reads one data point at a time until the end of the row
            row.push_back(data);
        }
        features.push_back(row);        // adds the features to features at the same indices corresponding to the vector class
    }
    file.close();

    // data info
    int myInstances = features.size();   // number of clusters
    int myFeatures = features[0].size();// number of features in first cluster
    double myAccuracy = 0.0;
    
    // calc features, instances, and percent
    cout << "This dataset has " << myFeatures << "features (not including the class attribute), with " << myInstances << "instances." << endl;
    cout << "Running nearest neighbor with all 4 features, using \"leaving-one-out\" evaluation, I get an accuracy of " << myAccuracy << "%" << endl;
    cout << "Beginning search." << endl;

    // run search alg
    if (myAlg == 1){
        cout << "Forward Selection" << endl;
        forwardSelection(features, classes);
    }else{
        cout << "Backward Elimination" << endl;
        // backwardElimination(features, classes);
    }
}

double leaveOneOutCrossValidation(const vector<vector<double>>& features, const vector<int>& classes, const vector<int>& currentSet, int featureToAdd){
    return static_cast<double>(rand()) / static_cast<double>(RAND_MAX) * 100;        // random % from 0-100
}

void forwardSelection(const vector<vector<double>>& features, vector<int> classes){
    int numFeatures = features[0].size();
    vector<int> currentSetOfFeatures;

    for (int i = 1; i <= numFeatures; i++){     // i traverses each level (number of features) of the search tree
        cout << "On the " << i << "th level of the search tree" << endl;
        int featureToAdd = 0;           // updates to the current feature checked for adding
        double bestAccuracy = 0.0;      // Compared with current best accuracy, and updates if new best found

        for (int k = 1; k <= numFeatures; k++){ // checks each features accuracy with leave-one-out one by one
            
            // check if its already in the set
            int alreadyAdded = 0;
            for (int j = 0; j < currentSetOfFeatures.size(); j++){
                if (currentSetOfFeatures[j] == k){
                    alreadyAdded = 1;
                }
            }
            
            if (alreadyAdded == 0){ // if not already added
                cout << "--Considering adding the " << k << " feature" << endl;

                double accuracy = leaveOneOutCrossValidation(features, classes, currentSetOfFeatures, k);   // Checks the current feature using leave-one-out test

                if (accuracy > bestAccuracy){   // if currentAccuracy is better than the known best, update
                    bestAccuracy = accuracy;
                    featureToAdd = k;
                }
            } 
        }
        currentSetOfFeatures.push_back(featureToAdd);       // adds the the current feature

        cout << "On level " << i << " I added feature " << featureToAdd << " to current set" << endl;
    }
}

void backwardsElimination(const vector<vector<double>>& features, vector<int> classes){
    cout << "backward selection" << endl;
}