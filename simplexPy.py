import sys

class Simplex:

    def __init__(self):
        self.table = []
    
    def setObjectiveFunction(self, fo: list):
        self.table.append(fo) #Adicionar a função objetivo

    def addRestrictions(self, sa: list):
        self.table.append(sa) #Adicionar restrições

    def getEntryColumn(self) -> int:
        columnPivot = min(self.table[0]) #pegar o menor valor da linha da função objetivo
        index = self.table[0].index(columnPivot) #pegar o index da nova coluna pivot (a que ta entrando)

        return index
    
    def getExitLine(self, entryColumn: int) -> int:
        #dividir o valor de b pelo pivot (elemento da coluna escolhida) respectivo de cada linha
        results = {}

        for line in range(len(self.table)):
            if line > 0:
                if self.table[line][entryColumn] > 0:
                    division = self.table[line][-1] / self.table[line][entryColumn]
                    results[line] = division

        index = min(results, key=results.get) #Achar o menor valor e retornar seu index
        
        return index
    
    def calculateNewPivotLine(self, entryColumn: int, exitLine: int, ) -> list:
        pivot = self.table[exitLine][entryColumn]
        newPivotLine = [value / pivot for value in self.table[exitLine]]

        return newPivotLine
    
    def calculateNewLine(self, line: list, entryColumn: int, pivotLine: list) -> list:
        pivot = line[entryColumn] * -1 
        
        resultLine = [value * pivot for value in pivotLine]

        newLine = []

        for i in range(len(resultLine)):
            sumElements = resultLine[i] + line[i]
            newLine.append(sumElements)

        return newLine
    
    def isNegative(self) -> bool:
        #to checando aqui se existe algum valor negativo na linha da função objetivo
        for value in self.table[0]:
            if value < 0:
                return True
            
        # negative = list(filter(lambda x: x < 0, self.table[0])) #filtrar os valores negativos
        # return True if len(negative) > 0 else False

        return False
    
    def showTable(self):
        for i in range(len(self.table)):
            for j in range(len(self.table[0])):
                print(f"{self.table[i][j]}\t", end="")
            print()

    def calculate(self):
        entryColumn = self.getEntryColumn()
        firstExitLine = self.getExitLine(entryColumn)

        pivotLine = self.calculateNewPivotLine(entryColumn, firstExitLine)

        self.table[firstExitLine] = pivotLine

        tableCopy = self.table.copy()

        index = 0

        while(index < len(tableCopy)):
            if index != firstExitLine:
                newLine = self.calculateNewLine(tableCopy[index], entryColumn, pivotLine)
                self.table[index] = newLine
            index += 1

    def solve(self):
        self.calculate()

        while self.isNegative():
            self.calculate()
        
        self.showTable()
        

if __name__ == "__main__":
    """
        MAX fo: 5x +2y
        sa:
            2x + y <= 6
            10x + 12y <= 60
            x, y >= 0

        forma canonica:

        z - 5x -2y = 0
        2x + y + f1 = 6
        10x + 12y + f2 = 60
        x, y >= 0
    """
    # simplex = Simplex()
    # simplex.setObjectiveFunction([1, -5, -2, 0, 0, 0])
    # simplex.addRestrictions([0, 2, 1, 1, 0, 6])
    # simplex.addRestrictions([0, 10, 12, 0, 1, 60])
    # simplex.solve()

     
    input = sys.stdin.read
    data = input().strip().split('\n')

    n, m = map(int, data[0].split())
    c = list(map(int, data[1].split()))
    constraints = [list(map(int, line.split())) for line in data[2:]]

    simplex = Simplex()
    simplex.setObjectiveFunction(c)

    for constraint in constraints:
        simplex.addRestrictions(constraint)

    simplex.solve()