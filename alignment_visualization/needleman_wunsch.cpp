#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <algorithm>

// Scoring constants
const int MATCH_SCORE = 1;
const int MISMATCH_SCORE = -1;
const int GAP_SCORE = -2;

struct Cell {
    int score;
    char direction;  // 'D': diagonal, 'U': up, 'L': left, '0': origin
    
    Cell(int s = 0, char d = '0') : score(s), direction(d) {}
};


// ANSI color codes
namespace Color {
    // Foreground colors
    const std::string RED = "\033[31m";
    const std::string GREEN = "\033[32m";
    const std::string YELLOW = "\033[33m";
    const std::string BLUE = "\033[34m";
    const std::string MAGENTA = "\033[35m";
    const std::string CYAN = "\033[36m";
    const std::string WHITE = "\033[37m";
    
    // Background colors
    const std::string BG_RED = "\033[41m";
    const std::string BG_GREEN = "\033[42m";
    const std::string BG_YELLOW = "\033[43m";
    const std::string BG_BLUE = "\033[44m";
    const std::string BG_WHITE = "\033[47m";
    const std::string BG_GRAY = "\033[40m";
    
    // Formatting
    const std::string BOLD = "\033[1m";
    const std::string UNDERLINE = "\033[4m";
    const std::string RESET = "\033[0m";
}

class AlignmentVisualizer {
private:
    static const int LINE_LENGTH = 200;  // Characters per line
    
    static std::string createRuler(int length) {
        std::stringstream ruler;
        ruler << Color::CYAN;
        for (int i = 1; i <= length; ++i) {
            if (i % 10 == 0) {
                ruler << std::setw(9) << i << " ";
            }
        }
        ruler << "\n"; // new line for design .....+.....+....
        ruler << "     ";
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
                //std::cout << Color::BG_GREEN << "|" << Color::RESET;
                // check the type of seq1 and seq2 and decide the color of the | based on that.

                switch (seq1[i]) {
                    case 'A':
                        std::cout << Color::BG_GRAY << Color::RED << "|" << Color::RESET;
                        break;
                    case 'T':
                        std::cout << Color::BG_GRAY << Color::BLUE << "|" << Color::RESET;
                        break;
                    case 'C':
                        std::cout << Color::BG_GRAY << Color::YELLOW << "|" << Color::RESET;
                        break;
                    case 'G':
                        std::cout << Color::BG_GRAY << Color::GREEN << "|" << Color::RESET;
                        break;
                
                }
            } else if (seq1[i] == '-' || seq2[i] == '-') {
                std::cout << Color::BG_GRAY << Color::WHITE << "-" << Color::RESET;
            } else {
                std::cout << Color::BG_YELLOW << Color::RED << "X" << Color::RESET;
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
            std::cout << "       " << createRuler(blockLength);
            
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



class NeedlemanWunsch {
private:
    std::string seq1, seq2;
    std::vector<std::vector<Cell> > matrix;

    std::string readFasta(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open file: " + filename);
        }

        std::string sequence, line;
        bool firstLine = true;
        
        while (std::getline(file, line)) {
            if (line.empty()) continue;
            if (line[0] == '>') {
                if (firstLine) {
                    firstLine = false;
                    continue;
                }
                break;
            }
            // Remove any whitespace from the sequence
            line.erase(std::remove_if(line.begin(), line.end(), ::isspace), line.end());
            sequence += line;
        }
        
        return sequence;
    }

    void initializeMatrix() {
        // Initialize matrix with dimensions (seq1.length + 1) x (seq2.length + 1)
        matrix.resize(seq1.length() + 1, std::vector<Cell>(seq2.length() + 1));
        
        // Initialize first row
        for (size_t j = 0; j <= seq2.length(); ++j) {
            matrix[0][j] = Cell(j * GAP_SCORE, 'L');
        }
        
        // Initialize first column
        for (size_t i = 0; i <= seq1.length(); ++i) {
            matrix[i][0] = Cell(i * GAP_SCORE, 'U');
        }
        
        // Set origin point
        matrix[0][0] = Cell(0, '0');
    }

    void fillMatrix() {
        for (size_t i = 1; i <= seq1.length(); ++i) {
            for (size_t j = 1; j <= seq2.length(); ++j) {
                // Calculate scores for all possible moves
                int matchScore = matrix[i-1][j-1].score + 
                    (seq1[i-1] == seq2[j-1] ? MATCH_SCORE : MISMATCH_SCORE);
                int deleteScore = matrix[i-1][j].score + GAP_SCORE;
                int insertScore = matrix[i][j-1].score + GAP_SCORE;
                
                // Find the maximum score and its direction
                if (matchScore >= deleteScore && matchScore >= insertScore) {
                    matrix[i][j] = Cell(matchScore, 'D');
                } else if (deleteScore >= insertScore) {
                    matrix[i][j] = Cell(deleteScore, 'U');
                } else {
                    matrix[i][j] = Cell(insertScore, 'L');
                }
            }
        }
    }

    void traceback() {
        aligned1.clear();
        aligned2.clear();
        
        size_t i = seq1.length();
        size_t j = seq2.length();
        
        // Start from the bottom-right corner and work back to origin
        while (i > 0 || j > 0) {
            char direction = matrix[i][j].direction;
            
            if (direction == 'D' && i > 0 && j > 0) {
                aligned1 = seq1[i-1] + aligned1;
                aligned2 = seq2[j-1] + aligned2;
                i--; j--;
            } else if (direction == 'U' && i > 0) {
                aligned1 = seq1[i-1] + aligned1;
                aligned2 = '-' + aligned2;
                i--;
            } else if (j > 0) {
                aligned1 = '-' + aligned1;
                aligned2 = seq2[j-1] + aligned2;
                j--;
            }
        }
    }

public:
    std::string aligned1, aligned2;
    NeedlemanWunsch(const std::string& file1, const std::string& file2) {
        seq1 = readFasta(file1);
        seq2 = readFasta(file2);
    }

    void align() {
        initializeMatrix();
        fillMatrix();
        traceback();
    }
};

int main(int argc, char* argv[]) {
    if (argc != 3) {
        //./needleman data/1.fna data/2.fna 
        std::cerr << "Usage: " << argv[0] << " <sequence1.fna> <sequence2.fna>" << std::endl;
        std::cerr << "ex: " << argv[0] << " data/1.fna data/2.fna" << std::endl;

        return 1;
    }

    try {
        NeedlemanWunsch nw(argv[1], argv[2]);
        nw.align();

        try {
            AlignmentVisualizer::visualizeAlignment(nw.aligned1, nw.aligned2);
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
            return 1;
        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
