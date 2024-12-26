__kernel void gc_count(
    __global const char* dna_sequence, 
    __global int* gc_counts, 
    const int sequence_length) {
    // Get thread ID
    int thread_id = get_global_id(0);
    int local_gc_count = 0;

    // Each thread processes a chunk of the DNA sequence
    for (int i = thread_id; i < sequence_length; i += get_global_size(0)) {
        char base = dna_sequence[i];
        if (base == 'G' || base == 'C') {
            local_gc_count++;
        }
    }

    // Store the result in the output array
    gc_counts[thread_id] = local_gc_count;
}
