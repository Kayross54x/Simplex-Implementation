#include <iostream>
#include <vector>
#include <fstream>
#include <iomanip>
#include <limits>

using namespace std;

const double EPS = 1e-9;
const double INF = numeric_limits<double>::infinity();

void printVector(const vector<double>& vec) {
    for (double val : vec) {
        cout << fixed << setprecision(3) << val << " ";
    }
    cout << endl;
}

void insertionSort(vector<int>& arr) {
    int n = arr.size();
    for (int i = 1; i < n; i++) {
        int key = arr[i];
        int j = i - 1;
        while (j >= 0 && arr[j] > key) {
            arr[j + 1] = arr[j];
            j = j - 1;
        }
        arr[j + 1] = key;
    }
}

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

void generateAuxiliaryPL(const vector<vector<double>> &A, const vector<double> &b, int n, int m, vector<vector<double>> &A_aux, vector<double> &b_aux, vector<double> &c_aux, vector<vector<double>> &optimalCertificate) {
    A_aux = A;
    b_aux = b;
    c_aux = vector<double>(m, 0.0);

    for (int i = 0; i < n + 1; i++) {
        for (int j = 0; j < n; j++) {
            if(i == j) {
                if(b_aux[j] < 0) {
                    optimalCertificate[i][j] = -1;
                } else {
                    optimalCertificate[i][j] = 1;
                }
            } else {
                optimalCertificate[i][j] = 0;
            }
        }
    }
    // Ajusta as linhas com b negativo
    for (int i = 0; i < n; i++) {
        if (b_aux[i] < 0) {
            for (int j = 0; j < m; j++) {
                if(A_aux[i][j] != 0){
                    A_aux[i][j] *= -1;
                } else {
                    A_aux[i][j] = 0;
                }
            }
            b_aux[i] *= -1;
        }
    }

    // Adiciona variáveis artificiais
    for (int i = 0; i < n; i++) {
        A_aux[i].resize(m + n, 0);
        A_aux[i][m + i] = 1; // Variável artificial
    }

    c_aux.resize(m + n, 0);
    for (int i = 0; i < n; i++) {
        c_aux[m + i] = -1; // Coeficiente da função objetivo para a variável artificial
    }
}

void initializeIdentityTable(vector<vector<double>> &table, int n) {
    for (int i = 0; i < n + 1; i++) {
        for (int j = 0; j < n; j++) {
            if(i == j) {
                table[i][j] = 1;
            } else {
                table[i][j] = 0;
            }
        }
    }
}

struct SimplexOriginal {
    vector<double> c_original, optimalSolution, ilimitedCetificate;
    vector<vector<double>> optimalCertificate;
    int n, m, n_original;
    vector<int> B, N;
    vector<vector<double>> D;

    SimplexOriginal(const vector<vector<double>> &A, const vector<double> &b, const vector<double> &c, vector<int> &initialBases,  int k, vector<vector<double>> &certificateMatrix) :
        n(c.size()), m(b.size()),D(m + 1, vector<double>(n + 1)), n_original(k), optimalCertificate(certificateMatrix) {

        for (int i = 0; i < m; i++)
            for (int j = 0; j < n; j++)
                D[i][j] = A[i][j];

        for (int i = 0; i < m; i++) {
            D[i][n] = b[i];
        }

        for (int j = 0; j < n; j++) {
            N.push_back(j);
            if(c[j] != 0){
                D[m][j] = -1 * c[j];
            } else {
                D[m][j] = 0;
            }
        }
        
        c_original = c;

        int maxValueInitialBase = 0;
        for (int i = 0; i < initialBases.size(); i++) {
            if(initialBases[i] > maxValueInitialBase){
                maxValueInitialBase = initialBases[i];
            }
        }

        vector<bool> changeBases(n+m, false);
        for (int val : initialBases) {
            changeBases[val] = true;
        } 

        int firstFalse = 0;

        //verificação pra ver se uma das bases iniciais encontrada pela auxiliar é uma variável artificial, se for eu troco por uma variavel original que ainda nao esta na base
        for (int i = 0; i < initialBases.size(); i++) {
            if(initialBases[i] > n) {
                for (int j = 0; j < changeBases.size(); j++) {
                    if(!changeBases[j]){
                        firstFalse = j;
                        break;
                    }
                }
                initialBases[i] = firstFalse;
            }
        } 
        
        for (int base : initialBases) {
            singlePivot(base);
        }  
    }

    bool singlePivot(int column) {
        int r = -1;
        for (int i = 0; i < m; i++) {
            if (D[i][column] > EPS) {
                if (r == -1 || D[i][n] / D[i][column] < D[r][n] / D[r][column]) r = i;
            }
        }
        if (r == -1) {
            // cout << "Não foi possivel pivotear a coluna: " << column << endl;
            return false;  // Problema é ilimitado (se não houver linha i tal que D[i][s] > 0, então o problema é ilimitado)
        }
    
        pivot(r, column);
        return true;
    }

    void pivot(int r, int s) {
        //Elemento pivoteado é da linha r e coluna s
        int columnOfBThatLeaves = 0;

        //Encontrando a coluna da base que sairá
        for (int j = 0; j < n; j++) {
            if(D[r][j] == 1){
                if(j != s) {
                    for (int i = 0; i < B.size(); i++){
                        if(B[i] == j){
                            columnOfBThatLeaves = i;
                            break;
                        }
                    }
                }
            }
        }

        double inv = 1.0;
        if (D[r][s] < 0) {
            inv = -1.0 / D[r][s];  // Inverso do pivô
        } else {
            inv = 1.0 / D[r][s];  // Inverso do pivô
        }
        // Normaliza a linha do pivô (divide todos os elementos por inv pro pivô ser 1)
        for (int j = 0; j <= n; j++) {
            D[r][j] *= inv;
        }

        for (int i = 0; i < n_original; i++) {
            optimalCertificate[r][i] *= inv;
        }

        for (int i = 0; i <= m; i++) {
            if (i != r) {
                double elementThatMultipliesPivotLine = D[i][s];

                for (int j = 0; j < n_original; j++) {
                    optimalCertificate[i][j] = optimalCertificate[i][j] + (elementThatMultipliesPivotLine * -1) * optimalCertificate[r][j];
                }

                for (int j = 0; j <= n; j++) {
                    double newLineElement = D[i][j] + (elementThatMultipliesPivotLine * -1) * D[r][j];
                    D[i][j] = newLineElement;
                }
            }
        }

        D[r][s] = 1;  // Define o pivô como 1
        D[m][s] = 0;

        int sIndex2 = -1;
        for (int i = 0; i < B.size(); i++) {
            if(B[i] == s){
                sIndex2 = i;
                break;
            }
        }

        //Achar o indice de s em N e apagar do vetor das nao basicas
        int sIndex = -1;
        for (int i = 0; i < N.size(); i++) {
            if(N[i] == s){
                sIndex = i;
                N.erase(N.begin() + sIndex);
                break;
            }
        }

        if(B.size() == m) { //Se ja tem o numero maximo de bases possivel, é só troca.
            N.push_back(B[columnOfBThatLeaves]);
            B.erase(B.begin() + columnOfBThatLeaves);
        }

        if (sIndex2 == -1) {
            //Adicionar a variavel de entrada em B
            B.push_back(s);
        }
    }

    bool simplex() {
        //ERA BOM FAZER UMA VERIFICAÇÃO PRA QUE O ELEMENTO PIVO QUE SEJA ESCOLHIDO NAO SEJA UM ELEMENTO DE UMA LINHA QUE JA TEMOS UMA VAR BASICA
        //EVITEI ISSO FAZENDO A ORDENAÇAO DAS BASES, MAS NAO SEI SE SO ISSO RESOLVE
        while (true) {
            int s = -1;
            for (int j = 0; j < n; j++) {
                if (s == -1 || D[m][j] < D[m][s]) s = j;
            }
            if (D[m][s] > -EPS) return true;  // solução ótima foi encontrada (Se todos os elementos da linha da função objetivo forem >= 0, então a solução é ótima)

            int r = -1;
            for (int i = 0; i < m; i++) {
                if (D[i][s] > EPS) {
                    if (r == -1 || D[i][n] / D[i][s] < D[r][n] / D[r][s]) r = i;
                }
            }
            if (r == -1) {
                setIlimitedCertificate(s);
                return false;  // Problema é ilimitado (se não houver linha i tal que D[i][s] > 0, então o problema é ilimitado)
            }

            pivot(r, s);
        }
    }

    double solve(bool &isUnlimited) {
        if (!simplex()) {
            isUnlimited = true;
            return -1;
        }
        
        isUnlimited = false;

        vector<double> aux(n, 0.0);

        for (int base : B) {
            for(int i = 0; i < m; i++) {
                if(D[i][base] == 1){
                    aux[base] = D[i][n];
                    break;
                }
            }
        }

        optimalSolution = aux;

        return D[m][n];
    }

    vector<double> getOptimalSolution() {
        return optimalSolution;
    }

    vector<double> getOptimalCertificate() {
        vector<double> cert;
        for (int i = 0; i < n_original; i++) {
            cert.push_back(optimalCertificate[n_original][i]);
        }
        return cert;
    }

    void setIlimitedCertificate(int s) {
        vector<double> cert(n, 0.0);
        cert[s] = 1.0;

        for (int i = 0; i < n; i++) {
            if(i != s) {
                bool isBase = false;
                for (int val : B) {
                    if (i == val) {
                        isBase = true;
                        for (int j = 0; j < m; j++) {
                            if (D[j][val] == 1) {
                                cert[i] = D[j][s] * -1;
                                break;
                            }
                        }
                        break;
                    }
                }
                if (!isBase) cert[i] = 0.0;
            }
            
        }

        ilimitedCetificate = cert;
    }
    
    vector<double> getIlimitedCertificate() {
        return ilimitedCetificate;
    }

    vector<double> getIlimitedSolution() {
        vector<double> solution(n, 0.0);
        for (int base : B) {
            for (int i = 0; i < m; i++) {
                if (D[i][base] == 1) {
                    solution[base] = D[i][n];
                }
            }
        }
        return solution;
    }
};

struct SimplexAuxiliar {
    vector<vector<double>> optimalCertificate;
    vector<double> c_original;
    int n_original;
    int n, m;
    vector<int> B, N;
    vector<vector<double>> D;

    SimplexAuxiliar(const vector<vector<double>> &A, const vector<double> &b, const vector<double> &c, int k, vector<vector<double>> &certificateMatrix) :
        n(c.size()), m(b.size()), B(m), N(n + 1), D(m + 1, vector<double>(n + 1)), n_original(k), optimalCertificate(certificateMatrix) { 
        for (int i = 0; i < m; i++)
            for (int j = 0; j < n; j++)
                D[i][j] = A[i][j];

        for (int i = 0; i < m; i++) {
            B[i] = n + i;
            D[i][n] = b[i];
        }

        for (int j = 0; j < n; j++) {
            N[j] = j;
            if(c[j] != 0){
                D[m][j] = -1 * c[j];
            } else {
                D[m][j] = 0;
            }
        }

        c_original = c;

        int m = D.size();    // Número de linhas
        int n = D[0].size(); // Número de colunas

        // Iterando sobre todas as colunas
        for (int j = 0; j < n; ++j) {
            double sum = 0;
            // iterando sobre todss as linhas acima da última linha
            for (int i = 0; i < m - 1; ++i) {
                sum -= D[i][j];
            }
            // armazenanndo a soma da coluna j na última linha
            D[m - 1][j] += sum;
        }
        
        for (int i = 0; i < k; ++i) {
            double sum = 0;
            for (int j = 0; j < k + 1; ++j) {
                sum -= optimalCertificate[j][i];
            }

            optimalCertificate[k][i] = sum;
        }
    }

    void pivot(int r, int s) {
        //Elemento pivoteado é o da linha: r e coluna s;
        double inv = 1.0;  // Inverso do pivô
        if (D[r][s] < 0) {
            inv = -1.0 / D[r][s];  // Inverso do pivô
        } else {
            inv = 1.0 / D[r][s];  // Inverso do pivô
        }
        // Normaliza a linha do pivô (divide todos os elementos por inv pro pivô ser 1)
        for (int j = 0; j <= n; j++) {
            D[r][j] *= inv;
        }

        for (int i = 0; i < n_original; i++) {
            optimalCertificate[r][i] *= inv;
        }

        for (int i = 0; i <= m; i++) {
            if (i != r) {
                double elementThatMultipliesPivotLine = D[i][s];

                for (int j = 0; j < n_original; j++) {
                    optimalCertificate[i][j] = optimalCertificate[i][j] + (elementThatMultipliesPivotLine * -1) * optimalCertificate[r][j];
                }

                for (int j = 0; j <= n; j++) {
                    double newLineElement = D[i][j] + (elementThatMultipliesPivotLine * -1) * D[r][j];
                    D[i][j] = newLineElement;
                }
            }
        }

        D[r][s] = 1;  // Define o pivô como 1
        D[m][s] = 0;

        // Troca as variáveis básicas e não básicas
        swap(B[r], N[s]);
    }

    bool simplex() {
        while (true) {
            int s = -1;
            for (int j = 0; j < n; j++) {
                if (s == -1 || D[m][j] < D[m][s]) s = j;
            }
            if (D[m][s] > -EPS) return true;  // Optimal solution found //Se todos os elementos da linha da função objetivo forem >= 0, então a solução é ótima

            int r = -1;
            for (int i = 0; i < m; i++) {
                if (D[i][s] > EPS) {
                    if (r == -1 || D[i][n] / D[i][s] < D[r][n] / D[r][s]) r = i;
                }
            }
            if (r == -1) return false;  // Problem é ilimitado //Se não houver linha i tal que D[i][s] > 0, então o problema é ilimitado

            pivot(r, s);
        }
    }

    double solve(vector<double> &x, bool &isUnlimited, vector<int> &initialBases) {
        if (!simplex()) {
            isUnlimited = true;
            return -1;
        }

        isUnlimited = false;
        x = vector<double>(n);

        for (int i = 0; i < m; i++)
            if (B[i] < n)
                x[B[i]] = D[i][n];

        //Pegando as bases iniciais pra original
        for (int c = 0; c < n; c++) {
            if(D[m][c] == 0) {
                bool canTakeBase = true;
                for (int i = 0; i < m; i++) {
                    if(D[i][c] != 1 && D[i][c] != 0) {
                        canTakeBase = false;
                        break;
                    }
                }
                if (canTakeBase) initialBases.push_back(c);
            }
        }

        return D[m][n];
    }

    vector<double> getCertificate() {
        vector<double> cert;
        for (int i = 0; i < n_original; i++) {
            cert.push_back(optimalCertificate[n_original][i]);
        }
        return cert;
    }
};

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Uso: " << argv[0] << " <arquivo_entrada>" << endl;
        return 1;
    }

    // o nome do arquivo de entrada digitado na linha de comando
    string filename = argv[1];

    int n, m;
    vector<vector<double>> A, A_aux;
    vector<double> b, b_aux, c, c_aux;

    readInputFromFile(filename, n, m, A, b, c);

    vector<vector<double>> certificateMatrixAux(n + 1, vector<double>(n));

    generateAuxiliaryPL(A, b, n, m, A_aux, b_aux, c_aux, certificateMatrixAux);
    SimplexAuxiliar auxSimplex(A_aux, b_aux, c_aux, n, certificateMatrixAux);

    bool isUnlimitedAux;
    vector<double> x_aux;
    vector<int> initialBases;

    double auxResult = auxSimplex.solve(x_aux, isUnlimitedAux, initialBases);

    if (auxResult < -EPS) {
        cout << "inviavel" << endl;
        printVector(auxSimplex.getCertificate());
    } else {
        insertionSort(initialBases);
        vector<vector<double>> certificateMatrix(n + 1, vector<double>(n));
        initializeIdentityTable(certificateMatrix, n);
        
        SimplexOriginal originalSimplex(A, b, c, initialBases, n, certificateMatrix);
        vector<double> x;
        bool isUnlimitedOriginal;
        double result = originalSimplex.solve(isUnlimitedOriginal);

        if (isUnlimitedOriginal) {
            cout << "ilimitada" << endl;
            printVector(originalSimplex.getIlimitedSolution());
            printVector(originalSimplex.getIlimitedCertificate());
        } else {
            cout << "otima" << endl;
            cout << fixed << setprecision(3) << result << endl;
            printVector(originalSimplex.getOptimalSolution());
            printVector(originalSimplex.getOptimalCertificate());
        }
    }

    return 0;
}