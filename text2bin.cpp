// Converts text file with integer to binary format.
#include <iostream>
#include <string>
#include <fstream>

int main(int, char **argv) {
    using std::ifstream;
    using std::ofstream;
    using std::ios_base;

    ifstream fin(argv[1], ios_base::in);
    ofstream fout(argv[2], ios_base::out | ios_base::binary);

    int value = 0;
    while (fin) {
        fin >> value;
        if (!fin) {
            break;
        }
        fout.write((const char *) &value, sizeof(value));
    }
    fin.close();
    fout.close();

    return 0;
}
