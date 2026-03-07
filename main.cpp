#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>
#include <iomanip>
#include <chrono>

// so i dont have to put std everywhere
using namespace std;
using namespace std::chrono;

// function stubs forward declaration for reading code sequentially
double leaveOneOutCrossValidation(const vector<vector<double>>& features, const vector<int>& classes, const vector<int>& currentSet, int featureToAdd);
void forwardSelection(const vector<vector<double>>& features, vector<int>& classes);
void backwardsElimination(const vector<vector<double>>& features, vector<int>& classes, vector<int> currentSetOfFeatures);

int main() {
    
    cout << fixed << setprecision(1);   // used for rounding my outputs for accuracy to the tenths place of precision (doesn't truncate)
    vector<int> classes;                // column from the txt file indicating class (1/2)
    vector<vector<double>> features;    // 2D table of "label: feature1, feature2, etc." for all other columns contain features
    
    string fileName = "";
    int myAlg = 0;

    cout << "Welcome to Jacob Kantor's Feature Selection Algorithm." << endl << endl;
    cout << "Type in the name of the file to test : " << endl;
    cin >> fileName;

    cout << "Type the number of the algorithm you want to run." << endl;
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

    // compile all features for nearest neighbor accuracy check
    vector<int> allFeatures;
    for (int i = 1; i <= myFeatures; i++){
        allFeatures.push_back(i);
    }
    myAccuracy = leaveOneOutCrossValidation(features, classes, allFeatures, 0);
    
    // calc features, instances, and percent
    cout << "This dataset has " << myFeatures << " features (not including the class attribute), with " << myInstances << " instances." << endl;
    cout << "Running nearest neighbor with all " << myFeatures << " features, using \"leaving-one-out\" evaluation, I get an accuracy of " << myAccuracy << "%" << endl;

    // run search algorithms
    cout << "Beginning search." << endl;
    // declare clock start and stop variables
    high_resolution_clock::time_point start;
    high_resolution_clock::time_point stop;
    start = high_resolution_clock::now();                  // starts the timer
    if (myAlg == 1){
        cout << "Forward Selection" << endl;
        forwardSelection(features, classes);
    }else{
        backwardsElimination(features, classes, allFeatures);
    }
    stop = high_resolution_clock::now();                // ends timer
    duration<double, ratio<60>> duration = stop - start;    // converts the difference of time to double milliseconds type
    cout << "\n\nCompletion Time: " << duration.count() << " minutes" << endl;
}

void forwardSelection(const vector<vector<double>>& features, vector<int>& classes){
    int numFeatures = features[0].size();
    vector<int> currentSetOfFeatures;
    vector<int> bestFeatureSet;
    double maxAccuracy = 0.0;

    double emptySetAccuracy = leaveOneOutCrossValidation(features, classes, currentSetOfFeatures, 0);
    cout << "Using feature(s) {} accuracy is " << emptySetAccuracy << "%" << endl;

    for (int i = 1; i <= numFeatures; i++){     // i traverses each level (number of features) of the search tree
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
                double accuracy = leaveOneOutCrossValidation(features, classes, currentSetOfFeatures, k);   // Checks the current feature using leave-one-out test
                
                // output the current features
                cout << "     Using feature(s) {";
                for (int f = 0; f < currentSetOfFeatures.size(); f++){
                    cout << currentSetOfFeatures[f] << ",";
                }
                cout << k << "} accuracy is " << accuracy << "%" << endl;

                if (accuracy > bestAccuracy){   // if current accuracy is better than the known best, update
                    bestAccuracy = accuracy;
                    featureToAdd = k;
                }
            } 
        }
        currentSetOfFeatures.push_back(featureToAdd);       // adds the the current feature
        
        // output features set
        cout << "Feature set {";
        for (int f = 0; f < currentSetOfFeatures.size(); f++){
            cout << currentSetOfFeatures[f];
            if (f != currentSetOfFeatures.size() - 1){
                cout << ",";
            }
        }
        cout << "} was best, accuracy is " << bestAccuracy << "%" << endl;
        if (bestAccuracy < maxAccuracy){    // If the accuracy decreases
            cout << "(Warning, Accuracy has decreased! Continuing search in case of local maxima)" << endl;
        }
        if (bestAccuracy > maxAccuracy){    // update best accuracy when new one is found
            maxAccuracy = bestAccuracy;
            bestFeatureSet = currentSetOfFeatures;
        }
    }

    // output the best set pf features
    cout << "Finished search!! The best feature subset is {";
    for (int i = 0; i < bestFeatureSet.size(); i++){
        cout << bestFeatureSet[i];
        if (i != bestFeatureSet.size() - 1){
            cout << ",";
        }
    }
    cout << "}, which has an accuracy of " << maxAccuracy << "%" << endl;
}

void backwardsElimination(const vector<vector<double>>& features, vector<int>& classes, vector<int> currentSetOfFeatures){
    int numFeatures = features[0].size();
    vector<int> bestFeatureSet;
    double maxAccuracy = 0.0;

    for (int i = numFeatures; i > 0; i--){     // i traverses each level (number of features) of the search tree for the one removed feature
        int featureToRemove = -1;           // index of the feature to be removed
        double bestAccuracy = 0.0;      // Compared with current best accuracy, and updates if new best found

        for (int k = 0; k < currentSetOfFeatures.size(); k++){ // check removing each feature currently in the set one by one
            vector<int> removedFeatureSet = currentSetOfFeatures;
            removedFeatureSet.erase(removedFeatureSet.begin() + k);     // removes the kth feature from a temporary set, without modifying the original set
            double accuracy = leaveOneOutCrossValidation(features, classes, removedFeatureSet, 0);   // Checks the current feature using leave-one-out test
            
            // output the current features after the removed check
            cout << "     Using feature(s) {";
            for (int f = 0; f < removedFeatureSet.size(); f++){
                cout << removedFeatureSet[f];
                if (f != removedFeatureSet.size() - 1){
                    cout << ",";
                }
            }
            cout << "} accuracy is " << accuracy << "%" << endl;

            if (accuracy > bestAccuracy){   // if current accuracy is better than the known best, update
                bestAccuracy = accuracy;
                featureToRemove = k;
            }
        }
        currentSetOfFeatures.erase(currentSetOfFeatures.begin() + featureToRemove);       // removes the identified kth feature
        
        // output features set
        cout << "Feature set {";
        for (int f = 0; f < currentSetOfFeatures.size(); f++){
            cout << currentSetOfFeatures[f];
            if (f != currentSetOfFeatures.size() - 1){
                cout << ",";
            }
        }
        cout << "} was best, accuracy is " << bestAccuracy << "%" << endl;
        if (bestAccuracy < maxAccuracy){    // If the accuracy decreases
            cout << "(Warning, Accuracy has decreased! Continuing search in case of local maxima)" << endl;
        }
        if (bestAccuracy > maxAccuracy){    // update best accuracy when new one is found
            maxAccuracy = bestAccuracy;
            bestFeatureSet = currentSetOfFeatures;
        }
    }

    // output the best set pf features
    cout << "Finished search!! The best feature subset is {";
    for (int i = 0; i < bestFeatureSet.size(); i++){
        cout << bestFeatureSet[i];
        if (i != bestFeatureSet.size() - 1){
            cout << ",";
        }
    }
    cout << "}, which has an accuracy of " << maxAccuracy << "%" << endl;
}

double leaveOneOutCrossValidation(const vector<vector<double>>& features, const vector<int>& classes, const vector<int>& currentSet, int featureToAdd){
    int numCorrectlyClassified = 0;

    for (int i = 0; i < features.size(); i++){              // traverses each feature for the leave one out. All the other points are treated as data
        int labelObjectToClassify = classes[i];             // remembers the real classification

        double nearestNeighborDistance = INFINITY;          // initialized at large distance to be immediately replaced
        int nearestNeighborLocation = 0;
        int nearestNeighborLabel = 0;

        for (int k = 0; k < features.size(); k++){          // makes comparisons of current against all other points
            if (k != i){                                    // skips self comparisons
                
                // Compute Distance
                double sumSquaredDistance = 0.0;   
                for (int j = 0; j < currentSet.size(); j++){ // loops the selected features
                    int featureIndex = currentSet[j] - 1;    // Shift since feature 1 = index 0 
                    
                    // distance = sqrt(sum((object_to_classify - data(k,2:end)).^2))
                    double difference = features[i][featureIndex] - features[k][featureIndex];   // The difference in features compared
                    sumSquaredDistance += difference * difference;     // squared difference added to the total over each iteration
                }

                // FORWARD SELECTION check
                if (featureToAdd != 0){     // skip backwards elimination. It isn't adding a feature so it's always 0
                    // includes the feature in the test
                    int featureIndex = featureToAdd - 1;
                    double difference = features[i][featureIndex] - features[k][featureIndex];
                    sumSquaredDistance += difference * difference;
                }

                double distance = sqrt(sumSquaredDistance);    // square root of summed squared differences between features
                
                // UPDATE, check if its the closest neighbor
                if (distance < nearestNeighborDistance){  // if current distance is closer
                    nearestNeighborDistance = distance;   // then update
                    // save the location and class for predicting
                    nearestNeighborLocation = k;
                    nearestNeighborLabel = classes[nearestNeighborLocation];
                }
            }
        }

        // If the prediction matches the remembered label, increment
        if (labelObjectToClassify == nearestNeighborLabel){
            numCorrectlyClassified++;
        }
    }
    // Accuracy computation
    double accuracy = static_cast<double>(numCorrectlyClassified) / features.size();    // num correct classified features / total features
    return accuracy * 100;
}