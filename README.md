# Opis programu

## 1. Z książki

Program jest oparty na przykładzie z rozdziału 8.7 książki:

**The Continuous Integer Knapsack Set and the Gomory Mixed Integer Set**

Główne ograniczenie:

```txt
7.2 y1 - 3.5 y2 + 5.4 y3 <= 12.0 + s
```

Jest to ograniczenie typu plecakowego. Lewa strona oznacza wykorzystanie limitu, a prawa strona oznacza dostępny limit.

Zmienne `y1`, `y2`, `y3` są całkowite i nieujemne. Zmienna `s` jest ciągła i nieujemna. `s` działa jak luz, czyli może zwiększyć prawą stronę ograniczenia.

Książka następnie dzieli całe ograniczenie przez `7.2`:

```txt
7.2 y1 / 7.2 = y1
-3.5 y2 / 7.2 = -35/72 y2
5.4 y3 / 7.2 = 3/4 y3
12.0 / 7.2 = 5/3
s / 7.2 = 10/72 s
```

Po tym przekształceniu dostajemy:

```txt
y1 - 35/72 y2 + 3/4 y3 <= 5/3 + 10/72 s
```

W programie zapisuję to ogólnie jako:

```txt
sum a[j] * y[j] <= b + q * s
```

czyli:

```txt
a[1] = 1
a[2] = -35/72
a[3] = 3/4
b = 5/3
q = 10/72
```

Nierówność MIR:

```txt
y1 - y2 + 0.25 y3 <= 1 + 0.4166666667 s
```

Ta nierówność wynika z mixed integer rounding. Dodaję ją jako dodatkowe ograniczenie, bo zmienne `y` są całkowite i ta nierówność wzmacnia model.

Do implementacji sam dodaję funkcję celu:

```txt
max 10 y1 + 2 y2 + 6 y3 - s
```

Książka w tym fragmencie skupia się na zbiorze rozwiązań i nierówności MIR, więc funkcja celu jest dodana po to, żeby CPLEX miał konkretny problem do rozwiązania.

Dodaję też górne ograniczenia:

```txt
0 <= y1, y2, y3 <= 10
0 <= s <= 20
```

Robię to, żeby problem obliczeniowy miał skończone rozwiązanie.

---

## 2. Kod

```cpp
ILOSTLBEGIN
```

Makro CPLEX

```cpp
IloEnv env;
```

Środowisko CPLEX


```cpp
IloModel model(env);
```

Model CPLEX, do którego później dodaję zmienne, funkcję celu i ograniczenia.

```cpp
const int n = 3;
```

Liczba zmiennych całkowitych `y`, bo w przykładzie z książki występują `y1`, `y2` i `y3`.

```cpp
std::vector<double> a = {
        1.0,
        -0.4861111111,
        0.75
};
```

Współczynniki głównego ograniczenia po podzieleniu przez `7.2`, czyli `1`, `-35/72` i `3/4`.

```cpp
const double b = 1.6666666667;
```

Zapisuję podstawowy limit po prawej stronie ograniczenia, czyli `5/3`.

```cpp
const double q = 0.1388888889;
```

Zapisuję współczynnik przy zmiennej `s`, czyli `10/72`.

```cpp
std::vector<int> yUpperBound = {
        10,
        10,
        10
};
```

Dodaję górne ograniczenia dla zmiennych `y`, żeby problem miał skończone rozwiązanie.

```cpp
const double sUpperBound = 20.0;
```

Dodaję górne ograniczenie dla zmiennej `s`, żeby luz nie mógł rosnąć bez ograniczeń.

```cpp
std::vector<double> profit = {
        10.0,
        2.0,
        6.0
};
```

Zapisuję współczynniki funkcji celu dla zmiennych `y`, czyli zysk z `y1`, `y2` i `y3`.

```cpp
const double sPenalty = 1.0;
```

Zapisuję karę za użycie zmiennej `s`, bo `s` zwiększa dostępny limit.

```cpp
std::vector<double> mirCoef = {
        1.0,
        -1.0,
        0.25
};
```

Zapisuję współczynniki lewej strony nierówności MIR z książki.

```cpp
const double mirRhsConst = 1.0;
```

Zapisuję stałą część prawej strony nierówności MIR.

```cpp
const double mirSlackCoef = 0.4166666667;
```

Zapisuję współczynnik przy zmiennej `s` w nierówności MIR, czyli `10/24`.

```cpp
IloIntVarArray y(env, n);
```

Tworzę tablicę zmiennych całkowitych `y`, bo w książce `y` należy do `Z+`.

```cpp
for (int j = 0; j < n; ++j) {
        y[j] = IloIntVar(env, 0, yUpperBound[j]);
}
```

Dla każdej zmiennej `y[j]` ustawiam zakres od `0` do `10`; CPLEX sam wybierze ich wartości, ale muszą być całkowite.

```cpp
IloNumVar s(env, 0.0, sUpperBound, ILOFLOAT);
```

Tworzę zmienną ciągłą `s` w zakresie od `0` do `20`, bo w książce `s` należy do `R+`.

```cpp
IloExpr objective(env);
```

Tworzę wyrażenie liniowe dla funkcji celu.

```cpp
for (int j = 0; j < n; ++j) {
        objective += profit[j] * y[j];
}
```

Dodaję do funkcji celu zysk ze zmiennych `y`, czyli `10y1 + 2y2 + 6y3`.

```cpp
objective -= sPenalty * s;
```

Odejmuję karę za użycie zmiennej `s`, żeby solver nie zwiększał luzu bez kosztu.

```cpp
model.add(IloMaximize(env, objective));
```

Dodaję funkcję celu do modelu jako maksymalizację.

```cpp
objective.end();
```

Zwalniam tymczasowe wyrażenie `objective`, bo nie jest już potrzebne.

```cpp
IloExpr knapsack(env);
```

Tworzę wyrażenie dla głównego ograniczenia z książki.

```cpp
for (int j = 0; j < n; ++j) {
        knapsack += a[j] * y[j];
}
```

Buduję lewą stronę głównego ograniczenia jako sumę `a[j] * y[j]`.

```cpp
model.add(knapsack <= b + q * s);
```

Dodaję główne ograniczenie z książki: `y1 - 35/72 y2 + 3/4 y3 <= 5/3 + 10/72 s`.

```cpp
knapsack.end();
```

Zwalniam tymczasowe wyrażenie `knapsack`.

```cpp
IloExpr mir(env);
```

Tworzę wyrażenie dla nierówności MIR.

```cpp
for (int j = 0; j < n; ++j) {
        mir += mirCoef[j] * y[j];
}
```

Buduję lewą stronę nierówności MIR jako `y1 - y2 + 0.25y3`.

```cpp
model.add(mir <= mirRhsConst + mirSlackCoef * s);
```

Dodaję nierówność MIR z książki: `y1 - y2 + 0.25y3 <= 1 + 0.4166666667s`.

```cpp
mir.end();
```

Zwalniam tymczasowe wyrażenie `mir`.

```cpp
IloCplex cplex(model);
```

Tworzę solver CPLEX i przekazuję mu gotowy model.

```cpp
cplex.setOut(env.getNullStream());
```

Wyłączam standardowy log CPLEX, żeby w konsoli zostały tylko moje wypisane wyniki.

```cpp
if (!cplex.solve()) {
        std::cerr << "Nie znaleziono rozwiązania\n";
        env.end();
        return 1;
}
```

Uruchamiam rozwiązywanie modelu; jeśli CPLEX nie znajdzie rozwiązania, program kończy działanie.

```cpp
std::cout << std::fixed << std::setprecision(6);
```

Ustawiam wypisywanie liczb z sześcioma miejscami po przecinku.

```cpp
std::cout << "Status: " << cplex.getStatus() << "\n";
```

Wypisuję status rozwiązania, np. `Optimal`.

```cpp
std::cout << "Objective value: " << cplex.getObjValue() << "\n\n";
```

Wypisuję wartość funkcji celu znalezioną przez CPLEX.

```cpp
for (int j = 0; j < n; ++j) {
        std::cout << "y" << j + 1 << " = " << cplex.getValue(y[j]) << "\n";
}
```

Wypisuję wartości zmiennych `y1`, `y2` i `y3` znalezione przez CPLEX.

```cpp
std::cout << "s = " << cplex.getValue(s) << "\n\n";
```

Wypisuję wartość zmiennej `s` znalezioną przez CPLEX.

```cpp
double knapsackLhs = 0.0;
double mirLhs = 0.0;
```

Tworzę zmienne pomocnicze do policzenia lewej strony głównego ograniczenia i lewej strony nierówności MIR.

```cpp
for (int j = 0; j < n; ++j) {
        knapsackLhs += a[j] * cplex.getValue(y[j]);
        mirLhs += mirCoef[j] * cplex.getValue(y[j]);
}
```

Obliczam lewe strony obu ograniczeń dla rozwiązania znalezionego przez CPLEX, żeby można było sprawdzić wynik.

```cpp
double knapsackRhs = b + q * cplex.getValue(s);
```

Obliczam prawą stronę głównego ograniczenia.

```cpp
double mirRhs = mirRhsConst + mirSlackCoef * cplex.getValue(s);
```

Obliczam prawą stronę nierówności MIR.

```cpp
std::cout << "Knapsack constraint:\n";
std::cout << "LHS = " << knapsackLhs << "\n";
std::cout << "RHS = " << knapsackRhs << "\n\n";
```

Wypisuję lewą i prawą stronę głównego ograniczenia, żeby pokazać, że rozwiązanie je spełnia.

```cpp
std::cout << "MIR inequality:\n";
std::cout << "LHS = " << mirLhs << "\n";
std::cout << "RHS = " << mirRhs << "\n";
```

Wypisuję lewą i prawą stronę nierówności MIR, żeby pokazać, że rozwiązanie spełnia też dodatkowe ograniczenie.

```cpp
env.end();
```

Zwalniam środowisko

---

## 3. Wyniki

Dla danych użytych w programie CPLEX znajduje rozwiązanie:

```txt
Status: Optimal
Objective value: 92.200000

y1 = 9.000000
y2 = 10.000000
y3 = 0.000000
s = 17.800000
```

Sprawdzenie głównego ograniczenia:

```txt
Knapsack constraint:
LHS = 4.138889
RHS = 4.138889
```

Lewa i prawa strona są równe, więc główne ograniczenie jest spełnione jako aktywne.

Sprawdzenie nierówności MIR:

```txt
MIR inequality:
LHS = -1.000000
RHS = 8.416667
```

Lewa strona jest mniejsza od prawej, więc nierówność MIR jest spełniona.
