#define CL_HPP_TARGET_OPENCL_VERSION 120
#define CL_HPP_MINIMUM_OPENCL_VERSION 120
#include <CL/cl2.hpp>
#include <iostream>
#include <vector>
#include <fstream>
#include <numeric>

const std::string kernel_code = R"CLC(
__kernel void gc_count(
    __global const char* dna_sequence, 
    __global int* gc_counts, 
    const int sequence_length) {
    int thread_id = get_global_id(0);
    int local_gc_count = 0;

    for (int i = thread_id; i < sequence_length; i += get_global_size(0)) {
        char base = dna_sequence[i];
        if (base == 'G' || base == 'C') {
            local_gc_count++;
        }
    }

    gc_counts[thread_id] = local_gc_count;
}
)CLC";

int main() {
    // Input DNA sequence
    std::string dna_sequence = "ATGCGGTTGCACGTAAGCGG"; // Example DNA sequence
    int sequence_length = dna_sequence.length();

    // Convert DNA sequence to char array
    std::vector<char> dna_array(dna_sequence.begin(), dna_sequence.end());

    // Load OpenCL platform and device
    std::vector<cl::Platform> platforms;
    cl::Platform::get(&platforms);
    cl::Platform platform = platforms[0];

    std::vector<cl::Device> devices;
    platform.getDevices(CL_DEVICE_TYPE_GPU, &devices);
    cl::Device device = devices[0];

    cl::Context context(device);
    cl::CommandQueue queue(context, device);

    // Create buffers
    cl::Buffer buffer_dna(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, dna_array.size(), dna_array.data());
    cl::Buffer buffer_gc_counts(context, CL_MEM_WRITE_ONLY, sizeof(int) * sequence_length);

    // Create and build the program
    cl::Program program(context, kernel_code);
    program.build({device});

    // Create the kernel
    cl::Kernel kernel(program, "gc_count");
    kernel.setArg(0, buffer_dna);
    kernel.setArg(1, buffer_gc_counts);
    kernel.setArg(2, sequence_length);

    // Execute the kernel
    size_t global_work_size = 256; // Number of threads
    queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(global_work_size));

    // Retrieve results
    std::vector<int> gc_counts(sequence_length);
    queue.enqueueReadBuffer(buffer_gc_counts, CL_TRUE, 0, sizeof(int) * sequence_length, gc_counts.data());

    // Compute total GC count
    int total_gc_count = std::accumulate(gc_counts.begin(), gc_counts.end(), 0);

    // Calculate GC content percentage
    double gc_content_percentage = (static_cast<double>(total_gc_count) / sequence_length) * 100.0;

    // Output the result
    std::cout << "GC Content: " << gc_content_percentage << "%" << std::endl;

    return 0;
}
