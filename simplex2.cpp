#include <iostream>
#include <vector>
#include <fstream>
#include <iomanip>
#include <limits>

using namespace std;

const double EPS = 1e-9;
const double INF = numeric_limits<double>::infinity();

struct Simplex {
    int n, m;
    vector<int> B, N;
    vector<vector<double>> D;

    Simplex(const vector<vector<double>> &A, const vector<double> &b, const vector<double> &c) :
        n(c.size()), m(b.size()), B(m), N(n + 1), D(m + 1, vector<double>(n + 1)) {
        for (int i = 0; i < m; i++)
            for (int j = 0; j < n; j++)
                D[i][j] = A[i][j];
                
        for (int i = 0; i < m; i++) {
            B[i] = n + i;
            D[i][n] = b[i];
        }
        for (int j = 0; j < n; j++) {
            N[j] = j;
            D[m][j] = -c[j];
        }
    }

    void pivot(int r, int s) {
        cout << "Pivotando na linha " << r << " e coluna " << s << endl;
        cout << "Variável que está entrando na base: N[" << s << "]" << endl;
        cout << "Variável que está saindo da base: B[" << r << "]" << endl;

        double inv = 1.0 / D[r][s];  // Inverso do pivô

        // Ajusta os elementos que não estão na linha ou coluna do pivô
        for (int i = 0; i <= m; i++) {
            if (i != r) {
                for (int j = 0; j <= n; j++) {
                    if (j != s) {
                        D[i][j] -= D[r][j] * D[i][s] * inv;
                    }
                }
            }
        }
        // Normaliza a linha do pivô
        for (int j = 0; j <= n; j++) {
            if (j != s) {
                D[r][j] *= inv;
            }
        }
        // Ajusta a coluna do pivô
        for (int i = 0; i <= m; i++) {
            if (i != r) {
                D[i][s] *= -inv;
            }
        }
        D[r][s] = 1;  // Define o pivô como 1

        // Troca as variáveis básicas e não básicas
        swap(B[r], N[s]);
    }

    bool simplex() {
        while (true) {
            int s = -1;
            for (int j = 0; j < n; j++) {
                if (s == -1 || D[m][j] < D[m][s]) s = j;
            }
            if (D[m][s] > -EPS) return true;  // Optimal solution found

            int r = -1;
            for (int i = 0; i < m; i++) {
                if (D[i][s] > EPS) {
                    if (r == -1 || D[i][n] / D[i][s] < D[r][n] / D[r][s]) r = i;
                }
            }
            if (r == -1) return false;  // Problem is unbounded

            // Print the pivot step
            cout << "Pivot Step: Entering variable index: " << s << ", Leaving variable index: " << r << endl;

            pivot(r, s);
        }
    }

    double solve(vector<double> &x, bool &isUnlimited) {
        if (!simplex()) {
            isUnlimited = true;
            return -1;
        }
        isUnlimited = false;
        x = vector<double>(n);
        for (int i = 0; i < m; i++)
            if (B[i] < n)
                x[B[i]] = D[i][n];
        return D[m][n];
    }

    vector<double> getCertificate() {
        vector<double> cert(n, 0.0);
        for (int j = 0; j < n; j++) {
            cert[j] = D[m][j];
        }
        return cert;
    }

    vector<double> getSolution() {
        vector<double> solution(n, 0.0);
        for (int i = 0; i < m; i++) {
            if (B[i] < n) {
                solution[B[i]] = D[i][n];
            }
        }
        return solution;
    }
};

void readInputFromFile(const string &filename, int &n, int &m, vector<vector<double>> &A, vector<double> &b, vector<double> &c) {
    ifstream infile(filename);
    if (!infile) {
        cerr << "Não foi possível abrir o arquivo de entrada!" << endl;
        exit(1);
    }

    infile >> n >> m;

    c.resize(m);
    for (int j = 0; j < m; j++)
        infile >> c[j];

    A.resize(n, vector<double>(m));
    b.resize(n);

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            infile >> A[i][j];
        }
        infile >> b[i];
    }

    infile.close();
}

void generateAuxiliaryPL(const vector<vector<double>> &A, const vector<double> &b, int n, int m, vector<vector<double>> &A_aux, vector<double> &b_aux, vector<double> &c_aux) {
    A_aux = A;
    b_aux = b;
    c_aux = vector<double>(m + n, 0.0);  // Variáveis de custo para variáveis artificiais

    // Adiciona variáveis artificiais
    for (int i = 0; i < n; i++) {
        A_aux[i].resize(m + n, 0);
        A_aux[i][m + i] = 1;  // Adiciona variável artificial
        c_aux[m + i] = -1;    // O custo das variáveis artificiais é -1
    }
}

void printVector(const vector<double>& vec) {
    for (double val : vec) {
        cout << fixed << setprecision(3) << val << " ";
    }
    cout << endl;
}

int main() {
    int n, m;
    vector<vector<double>> A, A_aux;
    vector<double> b, b_aux, c, c_aux;

    readInputFromFile("entrada.txt", n, m, A, b, c);

    generateAuxiliaryPL(A, b, n, m, A_aux, b_aux, c_aux);

    Simplex auxSimplex(A_aux, b_aux, c_aux);
    vector<double> x_aux;
    bool isUnlimitedAux;
    double auxResult = auxSimplex.solve(x_aux, isUnlimitedAux);
    cout << auxResult << "AAAAAAAAAAAAA" << endl;

    if (auxResult > EPS) {
        cout << "inviavel" << endl;
        printVector(auxSimplex.getCertificate());
    } else {
        Simplex originalSimplex(A, b, c);
        vector<double> x;
        bool isUnlimitedOriginal;
        double result = originalSimplex.solve(x, isUnlimitedOriginal);

        if (isUnlimitedOriginal) {
            cout << "ilimitada" << endl;
            printVector(originalSimplex.getSolution());
            printVector(originalSimplex.getCertificate());
        } else {
            cout << "otima" << endl;
            cout << fixed << setprecision(3) << result << endl;
            printVector(x);
            printVector(originalSimplex.getCertificate());
        }
    }

    return 0;
}
