#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>


// so i dont have to put std everywhere
using namespace std;

int main() {
    
    ifstream file("data.txt");      // opens the file

    if (!file.is_open()){
        cout << "Error opening the file for reading" << endl;
        return 1;
    }
    
    vector<int> classes;                // column from the txt file indicating class (1/2)
    vector<vector<double>> features;    // 2D table of "label: feature1, feature2, etc." for all other columns contain features
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



    
    int myFeatures = 0;
    int myInstances = 0;
    double myAccuracy = 0.0;
    string fileName = "";
    int myAlg = 0;
    
    cout << "Welcome to Jacob Kantor's Feature Selection Algorithm." << endl << endl;
    cout << "Type in the name of the file to test : " << endl;
    cin >> fileName;
    cout << "Type the nubmer of algorithm you want to run." << endl;
    cout << "\t1) Forward Selection" << endl;
    cout << "\t2) Backward Elimination" << endl;
    cin >> myAlg;

    // calc features, instances, and percent

    cout << "This dataset has " << myFeatures << "features (not including the class attribute), with " << myInstances << "instances." << endl;
    cout << "Running nearest neighbor with all 4 features, using \"leaving-one-out\" evaluation, I get an accuracy of " << myAccuracy << "%" << endl;
    cout << "Beginning search." << endl;

    // run search alg
}