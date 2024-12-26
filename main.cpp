#define CL_HPP_TARGET_OPENCL_VERSION 120
#define CL_HPP_MINIMUM_OPENCL_VERSION 120
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <CL/opencl.hpp>

// [Previous kernel source code remains the same]
const char* kernelSource = R"(
__kernel void calculateGC(__global const char* sequence,
                         __global int* gcCount,
                         __global int* totalBases,
                         const int sequenceLength) {
    int gid = get_global_id(0);
    int localGC = 0;
    int localTotal = 0;
    
    if (gid < sequenceLength) {
        char base = sequence[gid];
        if (base == 'G' || base == 'C' || base == 'g' || base == 'c') {
            localGC = 1;
        }
        if (base != 'N' && base != 'n') {
            localTotal = 1;
        }
        atomic_add(gcCount, localGC);
        atomic_add(totalBases, localTotal);
    }
}
)";

class GCCalculator {
private:
    cl::Context context;
    cl::CommandQueue queue;
    cl::Program program;
    cl::Kernel kernel;
    
    void initializeOpenCL() {
        std::vector<cl::Platform> platforms;
        cl::Platform::get(&platforms);
        if (platforms.empty()) {
            throw std::runtime_error("No OpenCL platforms found");
        }
        
        std::vector<cl::Device> devices;
        platforms[0].getDevices(CL_DEVICE_TYPE_GPU, &devices);
        if (devices.empty()) {
            throw std::runtime_error("No GPU devices found");
        }
        
        context = cl::Context(devices[0]);
        queue = cl::CommandQueue(context, devices[0]);
        
        cl::Program::Sources sources;
        sources.push_back({kernelSource, strlen(kernelSource)});
        program = cl::Program(context, sources);
        
        try {
            program.build({devices[0]});
        } catch (const std::exception& e) {
            std::cerr << "Build error: " << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(devices[0]) << std::endl;
            throw;
        }
        
        kernel = cl::Kernel(program, "calculateGC");
    }
    
public:
    GCCalculator() {
        initializeOpenCL();
    }
    
    void processSequence(const std::string& sequence, const std::string& header, int sequenceNumber,
                        int& totalGCCount, int& totalBaseCount) {
        if (sequence.empty()) return;
        
        try {
            cl::Buffer sequenceBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                                    sequence.size(), const_cast<char*>(sequence.data()));
            cl::Buffer gcCountBuffer(context, CL_MEM_READ_WRITE, sizeof(int));
            cl::Buffer totalBasesBuffer(context, CL_MEM_READ_WRITE, sizeof(int));
            
            int gcCount = 0, totalBases = 0;
            queue.enqueueWriteBuffer(gcCountBuffer, CL_TRUE, 0, sizeof(int), &gcCount);
            queue.enqueueWriteBuffer(totalBasesBuffer, CL_TRUE, 0, sizeof(int), &totalBases);
            
            kernel.setArg(0, sequenceBuffer);
            kernel.setArg(1, gcCountBuffer);
            kernel.setArg(2, totalBasesBuffer);
            kernel.setArg(3, static_cast<int>(sequence.size()));
            
            queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(sequence.size()));
            queue.finish();
            
            queue.enqueueReadBuffer(gcCountBuffer, CL_TRUE, 0, sizeof(int), &gcCount);
            queue.enqueueReadBuffer(totalBasesBuffer, CL_TRUE, 0, sizeof(int), &totalBases);
            
            // Add these values to the running totals
            totalGCCount += gcCount;
            totalBaseCount += totalBases;
            
            if (totalBases > 0) {
                float gcPercentage = (static_cast<float>(gcCount) / totalBases) * 100.0f;
                std::cout << "Sequence " << sequenceNumber << " (" << header << "):\n"
                         << "GC count: " << gcCount << "\n"
                         << "Percentage: " << gcPercentage << "%\n\n";
            }
        } catch (const std::exception& e) {
            std::cerr << "Error in sequence " << sequenceNumber << ": " << e.what() << "\n";
        }
    }
};

// [Rest of the code remains the same]
void processFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + filename);
    }
    
    GCCalculator calculator;
    std::string line, sequence, header;
    int sequenceNumber = 0;
    long totalGCCount = 0;    // Changed to long
    long totalBaseCount = 0;  // Changed to long
    
    while (std::getline(file, line)) {
        if (line[0] == '>') {
            if (!sequence.empty()) {
                int seqGC = 0, seqBases = 0;
                calculator.processSequence(sequence, header, sequenceNumber++, seqGC, seqBases);
                totalGCCount += seqGC;
                totalBaseCount += seqBases;
                sequence.clear();
            }
            header = line.substr(1, line.find(' ') - 1);
        } else {
            sequence += line;
        }
    }
    
    if (!sequence.empty()) {
        int seqGC = 0, seqBases = 0;
        calculator.processSequence(sequence, header, sequenceNumber, seqGC, seqBases);
        totalGCCount += seqGC;
        totalBaseCount += seqBases;
    }
    
    std::cout << "\nTotal Statistics:\n"
              << "Total GC count: " << totalGCCount << "\n"
              << "Total base count: " << totalBaseCount << "\n"
              << "Overall GC percentage: " << (static_cast<float>(totalGCCount) / totalBaseCount * 100.0f) << "%\n";
    
    file.close();
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <FASTA file>\n";
        return 1;
    }
    
    try {
        processFile(argv[1]);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}
