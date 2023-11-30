#include <iostream>
#include <fstream>

int main() {
    const char* filename = "img.png";

    std::ifstream file(filename, std::ios::binary);

    if (file.is_open()) {
        // Seek to the end of the file
        file.seekg(0, std::ios::end);

        // Get the current position (which is the size of the file)
        std::streampos size = file.tellg();

        if (size != -1) {
            std::cout << "Size of file " << filename << " is: " << size << " bytes." << std::endl;
        } else {
            std::cerr << "Error getting file size." << std::endl;
        }

        file.close(); // Close the file
    } else {
        std::cerr << "Error opening file: " << filename << std::endl;
    }

    return 0;
}

