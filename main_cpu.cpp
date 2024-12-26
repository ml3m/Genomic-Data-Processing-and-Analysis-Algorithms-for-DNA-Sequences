#include <iostream>
#include <fstream>
#include <string>
#include <csignal>
#include <atomic>
#include <regex>

std::atomic<bool> interrupted(false); // Flag for interruption

// Signal handler to capture interrupt and print results
void signalHandler(int signum) {
    interrupted = true;
    std::cout << "\nInterrupted. Exiting..." << std::endl;
    exit(signum);
}

// Function to process each sequence and calculate GC content
void processSequence(const std::string& sequence, const std::string& header, int sequenceNumber) {
    int gcCount = 0;
    int totalBases = 0;

    // Calculate GC count and total base count for this sequence
    for (char base : sequence) {
        if (base == 'G' || base == 'C') {
            gcCount++;
        }
        if (base != 'N') { // Count bases other than 'N'
            totalBases++;
        }
    }

    // Ensure we don't divide by zero
    if (totalBases == 0) {
        std::cout << "Warning: Empty sequence found, skipping." << std::endl;
        return;
    }

    // Calculate percentage
    float gcPercentage = (static_cast<float>(gcCount) / totalBases) * 100.0f;

    // Print sequence header, number, GC count, and percentage
    std::cout << "Sequence " << sequenceNumber << " (" << header << "):" << std::endl;
    std::cout << "GC count: " << gcCount << std::endl;
    std::cout << "Percentage: " << gcPercentage << "%" << std::endl;
}

// Function to process the file and count GC for each sequence
void processFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Could not open file: " << filename << std::endl;
        return;
    }

    std::string line;
    std::string sequence;
    int sequenceNumber = 0;  // Sequence counter
    std::string header;

    // Process each line in the file
    while (std::getline(file, line)) {
        // Check for header line (starts with '>')
        if (line[0] == '>') {
            // If there was a previous sequence, process it
            if (!sequence.empty()) {
                processSequence(sequence, header, sequenceNumber++);
                sequence.clear(); // Clear sequence for next one
            }
            // Extract the header (sequence name)
            header = line.substr(1, line.find(' ') - 1);  // Get the part before the first space
        } else {
            // Append the sequence data (ignore newline characters)
            sequence += line;
        }

        // Handle interruption gracefully
        if (interrupted.load()) {
            std::cout << "\nInterrupt received. Exiting..." << std::endl;
            break;
        }
    }

    // Process the last sequence in the file (if any)
    if (!sequence.empty()) {
        processSequence(sequence, header, sequenceNumber);
    }

    file.close();
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <FASTA file>" << std::endl;
        return 1;
    }

    // Register signal handler
    signal(SIGINT, signalHandler);

    // Process the file
    processFile(argv[1]);

    return 0;
}
