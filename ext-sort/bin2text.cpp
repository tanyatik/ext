// Converts binary file with integers to text format.
#include <iostream>
#include <string>
#include <fstream>

int main(int, char **argv) {
    using std::ifstream;
    using std::ofstream;
    using std::ios_base;

    ifstream fin(argv[1], ios_base::in | ios_base::binary);
    ofstream fout(argv[2], ios_base::out);

    uint64_t value = 0;
    while (fin) {
        fin.read((char *) &value, sizeof(value));
        if (fin.gcount() == 0) {
            break;
        }
        fout << value << std::endl;;
    }
    fin.close();
    fout.close();

    return 0;
}
