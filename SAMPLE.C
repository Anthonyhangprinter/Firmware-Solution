#include <iostream>
#include <fstream>
#include <sstream>
#include <bitset>
#include <string>
#include <iomanip>

// Function to parse a hexadecimal string to an unsigned integer
unsigned long parseHex(const std::string& hexStr) {
    unsigned long result = 0;
    std::stringstream ss;
    ss << std::hex << hexStr;
    ss >> result;
    return result;
}

// Function to extract the signal value from the data payload
double extractSignalValue(const std::string& dataPayload, int startBit, int bitLength, double scaleFactor) {
    // Convert hex data payload to an unsigned long
    unsigned long data = parseHex(dataPayload);

    // Calculate mask and shift to extract the relevant bits
    unsigned long mask = ((1UL << bitLength) - 1) << startBit;
    unsigned long signal = (data & mask) >> startBit;

    // Convert to double and scale
    return signal * scaleFactor;
}

int main() {
    // Update the file paths to match your directory structure
    std::ifstream logFile("c:\\Users\\Acer\\Desktop\\nA\\dump.log");
    std::ofstream outputFile("c:\\Users\\Acer\\Desktop\\nA\\output\\output.txt");
    std::string line;

    // Check if the files are opened successfully
    if (!logFile.is_open()) {
        std::cerr << "Error opening dump.log file." << std::endl;
        return 1;
    }

    if (!outputFile.is_open()) {
        std::cerr << "Error creating output.txt file." << std::endl;
        return 1;
    }

    // Define parameters based on DBC file
    int canIdWheelSpeed = 1797; // Decimal CAN ID for ECU_WheelSpeed
    int startBitFR = 0;        // Example start bit for WheelSpeedFR
    int startBitRR = 16;       // Example start bit for WheelSpeedRR
    int bitLength = 16;        // Example bit length for both signals
    double scaleFactorFR = 0.1; // Scaling factor for WheelSpeedFR
    double scaleFactorRR = 0.1; // Scaling factor for WheelSpeedRR

    while (getline(logFile, line)) {
        // Parsing log line
        std::stringstream ss(line);
        std::string timestamp, interface, idData;

        ss >> timestamp >> interface >> idData;

        int hashPos = idData.find('#');
        std::string idStr = idData.substr(0, hashPos);
        std::string dataPayload = idData.substr(hashPos + 1);

        int canId = std::stoi(idStr, nullptr, 16);

        // Debugging output
        std::cout << "Parsing line: " << line << std::endl;
        std::cout << "Timestamp: " << timestamp << std::endl;
        std::cout << "CAN ID: " << canId << std::endl;
        std::cout << "Data Payload: " << dataPayload << std::endl;

        if (canId == canIdWheelSpeed) {
            double wheelSpeedFR = extractSignalValue(dataPayload, startBitFR, bitLength, scaleFactorFR);
            double wheelSpeedRR = extractSignalValue(dataPayload, startBitRR, bitLength, scaleFactorRR);

            // Debugging output
            std::cout << "WheelSpeedFR: " << wheelSpeedFR << std::endl;
            std::cout << "WheelSpeedRR: " << wheelSpeedRR << std::endl;

            // Write results to output file
            outputFile << timestamp << ": WheelSpeedFR: " << std::fixed << std::setprecision(1) << wheelSpeedFR << "\n";
            outputFile << timestamp << ": WheelSpeedRR: " << std::fixed << std::setprecision(1) << wheelSpeedRR << "\n";
        }
    }

    // Close the files
    logFile.close();
    outputFile.close();

    return 0;
}
