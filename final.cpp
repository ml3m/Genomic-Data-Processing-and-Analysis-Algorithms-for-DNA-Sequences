#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include <sstream>
#include <algorithm>

// ANSI color codes
namespace Color {
    // Foreground colors
    const std::string RED = "\033[31m";
    const std::string GREEN = "\033[32m";
    const std::string YELLOW = "\033[33m";
    const std::string BLUE = "\033[34m";
    const std::string MAGENTA = "\033[35m";
    const std::string CYAN = "\033[36m";
    
    // Background colors
    const std::string BG_RED = "\033[41m";
    const std::string BG_GREEN = "\033[42m";
    const std::string BG_YELLOW = "\033[43m";
    const std::string BG_BLUE = "\033[44m";
    
    // Formatting
    const std::string BOLD = "\033[1m";
    const std::string UNDERLINE = "\033[4m";
    const std::string RESET = "\033[0m";
}

class AlignmentVisualizer {
private:
    static const int LINE_LENGTH = 60;  // Characters per line
    
    static std::string createRuler(int length) {
        std::stringstream ruler;
        ruler << Color::CYAN;
        for (int i = 1; i <= length; ++i) {
            if (i % 10 == 0) {
                ruler << std::setw(9) << i << " ";
            }
        }
        ruler << "\n";
        for (int i = 1; i <= length; ++i) {
            if (i % 10 == 0) {
                ruler << "|";
            } else if (i % 5 == 0) {
                ruler << "+";
            } else {
                ruler << ".";
            }
        }
        ruler << Color::RESET << "\n";
        return ruler.str();
    }
    
    static std::string colorBase(char base) {
        switch(base) {
            case 'A': return Color::RED + "A" + Color::RESET;
            case 'T': return Color::BLUE + "T" + Color::RESET;
            case 'G': return Color::GREEN + "G" + Color::RESET;
            case 'C': return Color::YELLOW + "C" + Color::RESET;
            case '-': return Color::MAGENTA + "-" + Color::RESET;
            default: return std::string(1, base);
        }
    }
    
    static void printSequenceBlock(const std::string& seq1, const std::string& seq2, 
                                 size_t start, size_t blockLength) {
        // Print sequence names and information
        std::cout << Color::BOLD << "Seq1 " << Color::RESET;
        
        // Print first sequence with colored bases
        for (size_t i = start; i < start + blockLength && i < seq1.length(); ++i) {
            std::cout << colorBase(seq1[i]);
        }
        std::cout << "\n";
        
        // Print match/mismatch line
        std::cout << "     ";
        for (size_t i = start; i < start + blockLength && i < seq1.length(); ++i) {
            if (seq1[i] == seq2[i] && seq1[i] != '-') {
                std::cout << Color::BG_GREEN << "|" << Color::RESET;
            } else if (seq1[i] == '-' || seq2[i] == '-') {
                std::cout << Color::BG_RED << " " << Color::RESET;
            } else {
                std::cout << Color::BG_YELLOW << "x" << Color::RESET;
            }
        }
        std::cout << "\n";
        
        // Print second sequence with colored bases
        std::cout << Color::BOLD << "Seq2 " << Color::RESET;
        for (size_t i = start; i < start + blockLength && i < seq2.length(); ++i) {
            std::cout << colorBase(seq2[i]);
        }
        std::cout << "\n\n";
    }

public:
    static void visualizeAlignment(const std::string& seq1, const std::string& seq2) {
        if (seq1.length() != seq2.length()) {
            throw std::runtime_error("Sequences must be aligned (same length)");
        }
        
        // Print header
        std::cout << Color::BOLD << Color::UNDERLINE 
                  << "Sequence Alignment Visualization" 
                  << Color::RESET << "\n\n";
                  
        // Print sequence information
        std::cout << "Length: " << seq1.length() << " bases\n\n";
        
        // Print color legend
        std::cout << "Legend:\n";
        std::cout << colorBase('A') << " : Adenine  ";
        std::cout << colorBase('T') << " : Thymine  ";
        std::cout << colorBase('G') << " : Guanine  ";
        std::cout << colorBase('C') << " : Cytosine  ";
        std::cout << colorBase('-') << " : Gap\n\n";
        std::cout << Color::BG_GREEN << "|" << Color::RESET << " : Match  ";
        std::cout << Color::BG_YELLOW << "x" << Color::RESET << " : Mismatch  ";
        std::cout << Color::BG_RED << " " << Color::RESET << " : Gap\n\n";
        
        // Process sequences in blocks
        for (size_t i = 0; i < seq1.length(); i += LINE_LENGTH) {
            size_t blockLength = std::min(static_cast<int>(LINE_LENGTH), static_cast<int>(seq1.length() - i));
            
            // Print ruler for this block
            std::cout << "     " << createRuler(blockLength);
            
            // Print sequences for this block
            printSequenceBlock(seq1, seq2, i, blockLength);
        }
        
        // Print alignment statistics
        int matches = 0, mismatches = 0, gaps = 0;
        for (size_t i = 0; i < seq1.length(); ++i) {
            if (seq1[i] == seq2[i] && seq1[i] != '-') matches++;
            else if (seq1[i] == '-' || seq2[i] == '-') gaps++;
            else mismatches++;
        }
        
        std::cout << Color::BOLD << "\nAlignment Statistics:\n" << Color::RESET;
        std::cout << Color::GREEN << "Matches: " << matches 
                  << " (" << std::fixed << std::setprecision(1) 
                  << (100.0 * matches / seq1.length()) << "%)\n" << Color::RESET;
        std::cout << Color::YELLOW << "Mismatches: " << mismatches 
                  << " (" << (100.0 * mismatches / seq1.length()) << "%)\n" << Color::RESET;
        std::cout << Color::RED << "Gaps: " << gaps 
                  << " (" << (100.0 * gaps / seq1.length()) << "%)\n" << Color::RESET;
    }
};

// Example usage
int main() {
    // Example aligned sequences
    std::string seq1 = "ACGT-ACGT-ACGT";
    std::string seq2 = "ACGTAACGTAACGT";
    
    try {
        AlignmentVisualizer::visualizeAlignment(seq1, seq2);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
