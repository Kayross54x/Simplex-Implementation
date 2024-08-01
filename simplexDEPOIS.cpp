#include <iostream>
#include <vector>
#include <fstream>
#include <iomanip>
#include <limits>
#include <algorithm>

using namespace std;

const double EPS = 1e-9;
const double INF = numeric_limits<double>::infinity();

void printVector(const vector<double>& vec) {
    for (double val : vec) {
        cout << fixed << setprecision(3) << val << " ";
    }
    cout << endl;
}

struct SimplexOriginal {
    vector<double> c_original;
    int n, m;
    vector<int> B, N;
    vector<vector<double>> D;

    SimplexOriginal(const vector<vector<double>> &A, const vector<double> &b, const vector<double> &c, vector<int> &initialBases) :
        n(c.size()), m(b.size()),D(m + 1, vector<double>(n + 1)) {

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

        cout << "VARIAVEIS BASICAS INICIAS: " << endl;
        for (int val : B) {
            cout << val << " ";
        }
        
        cout << endl;

        cout << "VARIAVEIS NAO BASICAS INICIAS: " << endl;
        for (int i = 0; i < n; i++) {
            cout << N[i] << " ";
        }
        cout << endl;

        cout << "Matriz D da original:" << endl;
        for (const auto& row : D) {
            for (double val : row) {
                cout << setw(10) << val << " ";
            }
            cout << endl;
        }

        cout << endl;

        c_original = c;

        cout << "bases iniciais a serem pivoteadas" << endl;
        cout << "valor de n " << n << endl;
        for (int i = 0; i < initialBases.size(); i++) {
            cout << "B[" << i << "] = " << initialBases[i] << endl;
        }

        int maxValueInitialBase = 0;
        for (int i = 0; i < initialBases.size(); i++) {
            if(initialBases[i] > maxValueInitialBase){
                maxValueInitialBase = initialBases[i];
            }
        }

        vector<bool> teste(n+m, false);
        for (int val : initialBases) {
            teste[val] = true;
        } 

        int firstFalse = 0;

        for (int i = 0; i < initialBases.size(); i++) {
            if(initialBases[i] > n) {
                cout << "A base inicial " <<initialBases[i] << " é uma variavel de folga" << endl;
                for (int j = 0; j < teste.size(); j++) {
                    if(!teste[j]){
                        firstFalse = j;
                        cout << "Foi trocada pela " << j << endl;
                        break;
                    }
                }
                initialBases[i] = firstFalse;
            }
        } 

        cout << "bases iniciais a serem pivoteadas" << endl;
        cout << "valor de n " << n << endl;
        for (int i = 0; i < initialBases.size(); i++) {
            cout << "B[" << i << "] = " << initialBases[i] << endl;
        }

        
        for (int base : initialBases) {
            singlePivot(base);
        }
        


        // cout << "bases iniciais a B serem pivoteadas" << endl;
        // for (int i = 0; i < m; i++) {
        //     cout << "B[" << i << "] = " << B[i] << endl;
        // }
        // int m = D.size();    // Número de linhas
        // int n = D[0].size(); // Número de colunas

        // // Itera sobre todas as colunas
        // for (int j = 0; j < n; ++j) {
        //     double sum = 0;
        //     // Itera sobre todas as linhas acima da última linha
        //     for (int i = 0; i < m - 1; ++i) {
        //         sum -= D[i][j];
        //     }
        //     // Armazena a soma na última linha, coluna j
        //     cout << " Soma da coluna " << j << " = " << sum << endl;
        //     D[m - 1][j] += sum;
        // }

        // cout << "Nova D da auxiliar:" << endl;
        // for (const auto& row : D) {
        //     for (double val : row) {
        //         cout << val << " ";
        //     }
        //     cout << endl;
        // }
            
    }

    bool singlePivot(int column) {
        int r = -1;
        for (int i = 0; i < m; i++) {
            if (D[i][column] > EPS) {
                if (r == -1 || D[i][n] / D[i][column] < D[r][n] / D[r][column]) r = i;
            }
        }
        if (r == -1) {
            cout << "Não foi possivel pivotear a coluna: " << column << endl;
            return false;  // Problem é ilimitado //Se não houver linha i tal que D[i][s] > 0, então o problema é ilimitado
        }
    
        pivot(r, column);
        return true;
    }

    void pivot(int r, int s) {
        int columnOfBThatLeaves = 0;

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

        cout << "Elemento pivoteado é da linha: " << r << " e coluna: " << s << " esse é o valor de D[r][s] " << D[r][s] << endl;
        double inv = 1.0;
        if (D[r][s] < 0) {
            cout << "endtrou aqui, a coluna que sai de B é: " << columnOfBThatLeaves << "e o inv é " << inv << endl;
            inv = -1.0 / D[r][s];  // Inverso do pivô
        } else {
            inv = 1.0 / D[r][s];  // Inverso do pivô
        }
        // Normaliza a linha do pivô (divide todos os elementos por inv pro pivô ser 1)
        for (int j = 0; j <= n; j++) {
            D[r][j] *= inv;
            // if (j != s) {
            //     D[r][j] *= inv;
            // }
        }

        cout << "NOVA MATRIZ ELEMENTOS NORMALIZADOS" << endl;

        for (const auto& row : D) {
            for (double val : row) {
                cout << setw(10) << val << " ";
            }
            cout << endl;
        }

        for (int i = 0; i <= m; i++) {
            if (i != r) {
                double elementThatMultipliesPivotLine = D[i][s];
                for (int j = 0; j <= n; j++) {
                    // if (j != s) {
                        double newLineElement = D[i][j] + (elementThatMultipliesPivotLine * -1) * D[r][j];
                        D[i][j] = newLineElement;
                        // D[i][j] -= D[r][j] * D[i][s] * inv;
                    // }
                }
            }
        }

        D[r][s] = 1;  // Define o pivô como 1
        D[m][s] = 0;

        // swap(B[r], N[s]);

        //Troca as variáveis básicas e não básicas
        cout << "B ANTES DA TROCA: " << endl;
        for (int val : B) {
            cout << val << " ";
        }
        cout << endl;

        cout << "N ANTES DA TROCA: " << endl;
        for (int val : N) {
            cout << val << " ";
        }
        cout << endl;

        //ver se s nao ta em b e achar o indice se B
        

        // for (int i = 0; i < B.size(); i++) {
            //Percorrer as colunas de D
        // for (int j = 0; j < n; j++) {

        //     if(D[r][j] == 1){
        //         if(j != s) {
        //             for (int i = 0; i < B.size(); i++){
        //                 if(B[i] == j){
        //                     columnOfBThatLeaves = i;
        //                     break;
        //                 }
        //             }
        //         }
        //     }
        // }
        // }

        int sIndex2 = -1;
        for (int i = 0; i < B.size(); i++) {
            if(B[i] == s){
                sIndex2 = i;
                break;
            }
        }

        //Achar o indice de s em N e apagar do vetor n
        int sIndex = -1;
        for (int i = 0; i < N.size(); i++) {
            if(N[i] == s){
                sIndex = i;
                N.erase(N.begin() + sIndex);
                break;
            }
        }

        if(B.size() == m) { //Se ja tem o numero maximo de bases possivel, é só troca.
            //Adicionar a variavel de saida em N
            
            cout << "Column of B that leaves: " << columnOfBThatLeaves << endl;
                            
            N.push_back(B[columnOfBThatLeaves]);
            B.erase(B.begin() + columnOfBThatLeaves);
        }

        if (sIndex2 == -1) {
            B.push_back(s);
        }
        
        // int sIndex2 = -1;
        // for (int i = 0; i < B.size(); i++) {
        //     if(B[i] == s){
        //         sIndex2 = i;
        //         break;
        //     }
        // }

        cout << "m " << m << endl;

        
        //Achar o indice de s em N e apagar do vetor n
        // int sIndex = -1;
        // for (int i = 0; i < N.size(); i++) {
        //     if(N[i] == s){
        //         sIndex = i;
        //         N.erase(N.begin() + sIndex);
        //         break;
        //     }
        // }

        // N.erase(N.begin() + sIndex);
       
        // swap(B[r], N[s]);

        cout << "B DEPOIS DA TROCA: " << endl;
        for (int val : B) {
            cout << val << " ";
        }
        cout << endl;
        cout << "N DEPOIS DA TROCA: " << endl;
        for (int val : N) {
            cout << val << " ";
        }
        cout << endl;

        cout << "========================NOVA MATRIZ ORIGINAL=======================================" << endl;

        cout << "D_NOVA:" << endl;
        for (const auto& row : D) {
            for (double val : row) {
                cout << setw(10) << val << " ";
            }
            cout << endl;
        }

        cout << endl;
    }

    bool simplex() {
        cout << "AQUI COMEÇOU O SIMPLEX" << endl;

        //ERA BOM FAZER UMA VERIFICAÇÃO PRA QUE O ELEMENTO PIVO QUE SEJA ESCOLHIDO NAO SEJA UM ELEMENTO DE UMA LINHA QUE JA TEMOS UMA VAR BASICA
        //EVITEI ISSO FAZENDO A ORDENAÇAO DAS BASES, MAS NAO SEI SE SO ISSO RESOLVE
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

            // Print the pivot step
            cout << "Pivot Step: Entering variable index: " << s << ", Leaving variable index: " << r << endl;

            pivot(r, s);
        }
    }

    double solve(vector<double> &x, bool &isUnlimited) {
            // Print A_aux, b_aux, c_aux for debugging
        cout << "A_aux_dentro_da_solve:" << endl;
        for (const auto& row : D) {
            for (double val : row) {
                cout << setw(10) << val << " ";
            }
            cout << endl;
        }

        // //PRINTAR VARIAVEIS BASICAS ANTES DO INICIO DO SIMPLEX:
        // cout << "Variaveis basicas antes do inicio do simplex:" << endl;
        // for (int i = 0; i < m; i++) {
        //     cout << "B[" << i << "] = " << B[i] << endl;
        // }
        // //PRINTAR VARIAVEIS NAO BASICAS ANTES DO INICIO DO SIMPLEX:
        // cout << "Variaveis nao basicas antes do inicio do simplex:" << endl;
        // for (int i = 0; i < n; i++) {
        //     cout << "N[" << i << "] = " << N[i] << endl;
        // }

        cout << endl;
        if (!simplex()) {
            isUnlimited = true;
            return -1;
        }
        isUnlimited = false;
        x = vector<double>(n-1);
        
        cout << "VARIAVEIS BASICAS FINAIS: " << endl;
        for (int i = 0; i < B.size(); i++) {
            cout << B[i] << " ";
        }
        cout << endl;
        
        cout << "m " << m << endl;
        cout << "n " << n << endl;

        //printa a matriz D
        cout << "D FINAL:" << endl;
        for (const auto& row : D) {
            for (double val : row) {
                cout << setw(6) << val << " ";
            }
            cout << endl;
        }

        vector<double> aux(n, 0.0);

        for (int base : B) {
            for(int i = 0; i < m; i++) {
                cout << "i " << i << " base " << base << endl;
                if(D[i][base] == 1){
                    
                    aux[base] = D[i][n];
                    break;
                }
            }
        }

        cout << "aux " << endl;
        for (int i = 0; i < n; i++) {
            cout << aux[i] << " ";
        }
        cout << endl;

        x = aux;

        
        // cout << "vetor b" << endl;
        // for (int i = 0; i < m; i++) {
        //     cout << B[i] << " ";
        //     cout << D[i][n] << endl;
        // }

        cout << endl;

        cout << "VARIAVEIS NAO BASICAS FINAIS: " << endl;
        for (int i = 0; i < N.size(); i++) {
            cout << N[i] << " ";
        }
        cout << endl;

        cout << "==========================================SOLUCAO=======================================" << endl;
        // cout << "Valores do vetor x (que devemos atribuir às variaveis):" << endl;
        // int counter = 0;
        // for (int val : x) {
        //     cout << "x[" << counter << "] = " << val << endl;
        //     counter++;
        // }

        return D[m][n];
    }

    vector<double> getCertificate() {
        vector<double> cert(n, 0.0);
        for (int j = 0; j < n; j++) {
            cert[j] = D[j][n] * c_original[j];
        }
        return cert;
    }

    vector<double> getCertificateIlimited() {
        vector<double> cert(n, 0.0);
        for (int j = 0; j < n; j++) {
            cert[j] = D[j][n];
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

    vector<double> getSolutionIlimited() {
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

        cout << "MATRIZ CERTIFICADO:" << endl;

        for (const auto& row : optimalCertificate) {
            for (double val : row) {
                cout << setw(6) << val << " ";
            }
            cout << endl;
        }

        c_original = c;

        cout << "D da auxiliar:" << endl;
        for (const auto& row : D) {
            for (double val : row) {
                cout << setw(6) << val << " ";
            }
            cout << endl;
        }

        int m = D.size();    // Número de linhas
        int n = D[0].size(); // Número de colunas

        // Itera sobre todas as colunas
        for (int j = 0; j < n; ++j) {
            double sum = 0;
            // Itera sobre todas as linhas acima da última linha
            for (int i = 0; i < m - 1; ++i) {
                sum -= D[i][j];
            }
            // Armazena a soma na última linha, coluna j
            cout << " Soma da coluna " << j << " = " << sum << endl;
            D[m - 1][j] += sum;
        }
        
        for (int i = 0; i < k; ++i) {
            double sum = 0;
            for (int j = 0; j < k + 1; ++j) {
                sum -= optimalCertificate[j][i];
            }

            optimalCertificate[k][i] = sum;
        }

        cout << "Nova D após canonização: " << endl;
        for (const auto& row : D) {
            for (double val : row) {
                cout << setw(6) << val << " ";
            }
            cout << endl;
        }

        cout << "NOVO CERTIFICADO da Auxiliar após canonização:" << endl;
        for (const auto& row : optimalCertificate) {
            for (double val : row) {
                cout << setw(6) << val << " ";
            }
            cout << endl;
        }
    }

    void pivot(int r, int s) {
        cout << "Elemento pivoteado é da linha: " << r << " e coluna: " << s << endl;
        double inv = 1.0 / D[r][s];  // Inverso do pivô
        // Normaliza a linha do pivô (divide todos os elementos por inv pro pivô ser 1)
        for (int j = 0; j <= n; j++) {
            D[r][j] *= inv;
        }

        for (int i = 0; i < n_original; i++) {
            optimalCertificate[r][i] *= inv;
        }

        cout << "NOVA MATRIZ ELEMENTOS NORMALIZADOS Com o valor do pivo" << endl;

        for (const auto& row : D) {
            for (double val : row) {
                cout << setw(6) << val << " ";
            }
            cout << endl;
        }

        for (int i = 0; i <= m; i++) {
            //cout << "Olhando pra linha " << i << endl;
            if (i != r) {
                double elementThatMultipliesPivotLine = D[i][s];

                for (int j = 0; j < n_original; j++) {
                    optimalCertificate[i][j] = optimalCertificate[i][j] + (elementThatMultipliesPivotLine * -1) * optimalCertificate[r][j];
                }

                for (int j = 0; j <= n; j++) {
                    //cout << "Olhando pra coluna " << j << endl;
                    // if (j != s) {
                        double newLineElement = D[i][j] + (elementThatMultipliesPivotLine * -1) * D[r][j];
                        //cout << "Novo elemento" << newLineElement << endl;
                        D[i][j] = newLineElement;
                        // D[i][j] -= D[r][j] * D[i][s] * inv;
                    // }
                }
            }
        }

        cout << "NOVO CERTIFICADO CONSIDERANDO ESSE PIVOT:" << endl;
        for (const auto& row : optimalCertificate) {
            for (double val : row) {
                cout << setw(6) << val << " ";
            }
            cout << endl;
        }

        D[r][s] = 1;  // Define o pivô como 1
        D[m][s] = 0;

        // Troca as variáveis básicas e não básicas
        swap(B[r], N[s]);

        cout << "==================================NOVA MATRIZ=======================================" << endl;

        cout << "D NOVA:" << endl;
        for (const auto& row : D) {
            for (double val : row) {
                cout << setw(10) << val << " ";
            }
            cout << endl;
        }

        cout << endl;
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

        cout << "==========================================SOLUCAO=======================================" << endl;
        cout << "Valores do vetor x (que devemos atribuir às variaveis):" << endl;
        for (int i = 0; i < n; i++) {
            cout << "x[" << i << "] = " << x[i] << endl;
        }

        cout << "m: " << m << " n: " << n << endl;
        //Pegando as bases iniciais pra original
        for (int c = 0; c < n; c++) {
            if(D[m][c] == 0) {
                int pode = true;
                for (int i = 0; i < m; i++) {
                    if(D[i][c] != 1 && D[i][c] != 0) {
                        pode = false;
                        break;
                    }
                }
                if (pode) initialBases.push_back(c);
            }
        }
        // initialBases = B;

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

int main() {
    int n, m;
    vector<vector<double>> A, A_aux;
    vector<double> b, b_aux, c, c_aux;

    readInputFromFile("entrada.txt", n, m, A, b, c);

    vector<vector<double>> certificateMatrix(n + 1, vector<double>(n));
    generateAuxiliaryPL(A, b, n, m, A_aux, b_aux, c_aux, certificateMatrix);

    SimplexAuxiliar auxSimplex(A_aux, b_aux, c_aux, n, certificateMatrix);
    vector<double> x_aux;
    bool isUnlimitedAux;
    vector<int> initialBases;

    double auxResult = auxSimplex.solve(x_aux, isUnlimitedAux, initialBases);
    
    //printar initialBases
    cout << "Bases iniciais pra começar a original:" << endl;
    for (int i = 0; i < initialBases.size(); i++) {
        cout << "B[" << i << "] = " << initialBases[i] << endl;
    }

    cout << "Resultado do PL Auxiliar: " << auxResult << endl;

    if (auxResult < -EPS) {
        cout << "inviavel" << endl;
        vector<double> inviableCertificate = auxSimplex.getCertificate();
        printVector(inviableCertificate);
    } else {
        sort(initialBases.begin(), initialBases.end());
        SimplexOriginal originalSimplex(A, b, c, initialBases);
        vector<double> x;
        bool isUnlimitedOriginal;
        double result = originalSimplex.solve(x, isUnlimitedOriginal);

        if (isUnlimitedOriginal) {
            cout << "ilimitada" << endl;
            printVector(originalSimplex.getSolutionIlimited());
        } else {
            cout << "otima" << endl;
            cout << fixed << setprecision(3) << result << endl;
            printVector(x);
            // printVector(originalSimplex.getCertificate());
        }
    }

    return 0;
}
