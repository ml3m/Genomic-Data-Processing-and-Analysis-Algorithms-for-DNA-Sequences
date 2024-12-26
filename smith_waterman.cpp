#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <memory>

// Scoring constants
const int MATCH_SCORE = 1;
const int MISMATCH_SCORE = -1;
const int GAP_SCORE = -2;

// Structure to store cell information for traceback
struct Cell {
    int score;
    int i;
    int j;
    char direction;  // 'D': diagonal, 'U': up, 'L': left

    // Constructor for easier initialization
    Cell(int s = 0, int ii = 0, int jj = 0, char d = '0') 
        : score(s), i(ii), j(jj), direction(d) {}
};

class SmithWaterman {
private:
    std::string seq1, seq2;
    std::vector<std::vector<Cell> > matrix;  // Fixed syntax here
    std::string aligned1, aligned2;
    int maxScore;
    Cell maxCell;

    // Read FASTA format file
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
            sequence += line;
        }
        
        return sequence;
    }

    // Initialize scoring matrix
    void initializeMatrix() {
        matrix.resize(seq1.length() + 1, std::vector<Cell>(seq2.length() + 1));
        
        // Initialize first row and column with zeros
        for (size_t i = 0; i <= seq1.length(); ++i) {
            matrix[i][0] = Cell();  // Fixed initialization
        }
        for (size_t j = 0; j <= seq2.length(); ++j) {
            matrix[0][j] = Cell();  // Fixed initialization
        }
    }

    // Fill the scoring matrix
    void fillMatrix() {
        maxScore = 0;
        
        for (size_t i = 1; i <= seq1.length(); ++i) {
            for (size_t j = 1; j <= seq2.length(); ++j) {
                // Calculate match/mismatch score
                int match = matrix[i-1][j-1].score + 
                    (seq1[i-1] == seq2[j-1] ? MATCH_SCORE : MISMATCH_SCORE);
                
                // Calculate gap scores
                int del = matrix[i-1][j].score + GAP_SCORE;
                int ins = matrix[i][j-1].score + GAP_SCORE;
                
                // Find maximum score
                int maxLocal = std::max(0, std::max(match, std::max(del, ins)));  // Fixed max syntax
                
                // Store the maximum score and its direction
                if (maxLocal == 0) {
                    matrix[i][j] = Cell(0, i, j, '0');  // Fixed initialization
                } else if (maxLocal == match) {
                    matrix[i][j] = Cell(maxLocal, i, j, 'D');  // Fixed initialization
                } else if (maxLocal == del) {
                    matrix[i][j] = Cell(maxLocal, i, j, 'U');  // Fixed initialization
                } else {
                    matrix[i][j] = Cell(maxLocal, i, j, 'L');  // Fixed initialization
                }
                
                // Update maximum score if necessary
                if (matrix[i][j].score > maxScore) {
                    maxScore = matrix[i][j].score;
                    maxCell = matrix[i][j];
                    maxCell.i = i;
                    maxCell.j = j;
                }
            }
        }
    }

    // Perform traceback to find alignment
    void traceback() {
        aligned1.clear();
        aligned2.clear();
        
        int i = maxCell.i;
        int j = maxCell.j;
        
        while (i > 0 && j > 0 && matrix[i][j].score > 0) {
            char direction = matrix[i][j].direction;
            
            if (direction == 'D') {
                aligned1 = seq1[i-1] + aligned1;
                aligned2 = seq2[j-1] + aligned2;
                i--; j--;
            } else if (direction == 'U') {
                aligned1 = seq1[i-1] + aligned1;
                aligned2 = '-' + aligned2;
                i--;
            } else if (direction == 'L') {
                aligned1 = '-' + aligned1;
                aligned2 = seq2[j-1] + aligned2;
                j--;
            }
        }
    }

public:
    // Constructor
    SmithWaterman(const std::string& file1, const std::string& file2) {
        seq1 = readFasta(file1);
        seq2 = readFasta(file2);
    }

    // Perform alignment
    void align() {
        initializeMatrix();
        fillMatrix();
        traceback();
    }

    // Generate match line
    std::string generateMatchLine() const {
        std::string matchLine;
        for (size_t i = 0; i < aligned1.length(); ++i) {
            if (aligned1[i] == aligned2[i] && aligned1[i] != '-') {
                matchLine += '|';
            } else if (aligned1[i] == '-' || aligned2[i] == '-') {
                matchLine += ' ';
            } else {
                matchLine += ' ';
            }
        }
        return matchLine;
    }

    // Print alignment results
    void printResults() const {
        // Print sequences information
        std::cout << "Sequence 1 length: " << seq1.length() << std::endl;
        std::cout << "Sequence 2 length: " << seq2.length() << std::endl;
        std::cout << "Alignment score: " << maxScore << std::endl << std::endl;

        // Print alignment
        const int LINE_LENGTH = 200;  // Characters per line
        for (size_t i = 0; i < aligned1.length(); i += LINE_LENGTH) {
            std::cout << aligned1.substr(i, LINE_LENGTH) << std::endl;
            std::cout << generateMatchLine().substr(i, LINE_LENGTH) << std::endl;
            std::cout << aligned2.substr(i, LINE_LENGTH) << std::endl << std::endl;
        }

        // Print alignment statistics
        int matches = 0, mismatches = 0, gaps = 0;
        for (size_t i = 0; i < aligned1.length(); ++i) {
            if (aligned1[i] == aligned2[i]) matches++;
            else if (aligned1[i] == '-' || aligned2[i] == '-') gaps++;
            else mismatches++;
        }

        std::cout << "Alignment Statistics:" << std::endl;
        std::cout << "Matches: " << matches << std::endl;
        std::cout << "Mismatches: " << mismatches << std::endl;
        std::cout << "Gaps: " << gaps << std::endl;
        std::cout << "Alignment length: " << aligned1.length() << std::endl;
        float identity = (float)matches / aligned1.length() * 100;
        std::cout << "Sequence identity: " << identity << "%" << std::endl;
    }
};

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <sequence1.fna> <sequence2.fna>" << std::endl;
        return 1;
    }

    try {
        SmithWaterman sw(argv[1], argv[2]);
        sw.align();
        sw.printResults();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
