#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>

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
    if (myAlg == 1){
        cout << "Forward Selection" << endl;
        forwardSelection(features, classes);
    }else{
        cout << "Backward Elimination" << endl;
        // backwardElimination(features, classes);
    }
}

void forwardSelection(const vector<vector<double>>& features, vector<int> classes){
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
        if (bestAccuracy > maxAccuracy){
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

void backwardsElimination(const vector<vector<double>>& features, vector<int> classes){
    cout << "backward selection" << endl;
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

                // FORWARD SELECTION
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