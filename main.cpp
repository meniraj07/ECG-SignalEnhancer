#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <cstdlib>
using namespace std;

struct ECGData {
    double time;
    double voltage;
};

// Reading .dat file (voltage)
vector<ECGData> readDAT(const string& filename) {
    vector<ECGData> data;
    ifstream file(filename);
    
    if (!file.is_open()) {
        cerr << "Error opening file! Check if ecg_data.dat exists." << endl;
        return data;
    }

    double voltage;
    double time = 0.0;
    double sampling_rate = 0.004; 
    
    while (file >> voltage) {
        data.push_back({time, voltage});
        time += sampling_rate; // Adding time increment
    }
    
    file.close();
    cout << "Read " << data.size() << " data points" << endl;
    return data;
}

// Moving average filter
vector<ECGData> filterECG(const vector<ECGData>& input, int window_size = 5) {
    vector<ECGData> output = input;
    int half_window = window_size / 2;
    
    for (size_t i = half_window; i < input.size() - half_window; i++) {
        double sum = 0.0;
        for (int j = -half_window; j <= half_window; j++) {
            sum += input[i + j].voltage;
        }
        output[i].voltage = sum / window_size;
    }
    
    return output;
}

// Plotting graph with Gnuplot
void plotData(const vector<ECGData>& data, const string& filename, const string& title) {
    FILE *gnuplot = _popen("gnuplot -persist", "w"); // Using _popen for Windows
    if (!gnuplot) {
        cerr << "Gnuplot error! Check installation and PATH" << endl;
        return;
    }
    
    fprintf(gnuplot, "set terminal pngcairo enhanced size 1280,720\n");
    fprintf(gnuplot, "set output '%s'\n", filename.c_str());
    fprintf(gnuplot, "set title '%s'\n", title.c_str());
    fprintf(gnuplot, "set xlabel 'Time (s)'\n");
    fprintf(gnuplot, "set ylabel 'Voltage (mV)'\n");
    fprintf(gnuplot, "plot '-' with lines lw 1 title 'ECG'\n");
    
    for (const auto& point : data) {
        fprintf(gnuplot, "%f %f\n", point.time, point.voltage);
    }
    
    fprintf(gnuplot, "e\n");
    fflush(gnuplot);
    _pclose(gnuplot);
}

int main() {
    // Reading data
    auto ecg_data = readDAT("ecg_data.dat");
    if (ecg_data.empty()) return 1;

    // Plotting the original graph
    plotData(ecg_data, "original.png", "Raw ECG Signal");
    
    // Filtering data
    auto filtered_data = filterECG(ecg_data, 7); // Window size 7
    
    // Plotting the filtered graph
    plotData(filtered_data, "filtered.png", "Filtered ECG Signal");
    
    cout << "Processing complete. Check .png files!" << endl;
    return 0;
}