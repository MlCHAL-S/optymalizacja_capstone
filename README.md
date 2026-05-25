    Program rozwiązuje przykład z rozdziału 8.7 książki:
    "The Continuous Integer Knapsack Set and the Gomory Mixed Integer Set"

    Punkt wyjścia jest podobny do problemu plecakowego

    W klasycznym plecaku mamy rzeczy, które mają:
        wagę
        wartość

    Trzeba wybrać rzeczy tak, żeby:
        wartość była jak największa
        waga nie przekroczyła pojemności plecaka

    W tym zadaniu nie pakujemy dosłownie rzeczy do plecaka
    Używamy tej samej idei matematycznej:
        lewa strona ograniczenia oznacza wykorzystanie limitu
        prawa strona ograniczenia oznacza dostępny limit

    W książce przykład zaczyna się od ograniczenia:

        7.2 y1 - 3.5 y2 + 5.4 y3 <= 12.0 + s

    Zmienne y1, y2, y3 są zmiennymi całkowitymi
    To znaczy, że CPLEX może wybrać np. 0, 1, 2, 3 itd.
    Nie może wybrać wartości ułamkowych typu 2.5

    Zmienna s jest zmienną ciągłą nieujemną
    To znaczy, że CPLEX może wybrać np. 0, 1.5, 17.8 itd.
    Ta zmienna działa jak luz po prawej stronie ograniczenia

    Można to rozumieć tak:
        b to podstawowy limit
        s pozwala ten limit zwiększyć
        ale s jest karane w funkcji celu, więc solver nie używa go za darmo

    Książka potem dzieli całe ograniczenie przez 7.2

    Czyli z:

        7.2 y1 - 3.5 y2 + 5.4 y3 <= 12.0 + s

    robi się:

        y1 - 35/72 y2 + 3/4 y3 <= 5/3 + 10/72 s

    To nadal jest to samo ograniczenie, tylko zapisane w wygodniejszej postaci
    Dzielimy wszystko przez 7.2, żeby współczynnik przy y1 był równy 1

    W kodzie zapisujemy to jako:

        a[1] = 1
        a[2] = -35/72
        a[3] = 3/4
        b = 5/3
        q = 10/72

    Czyli ogólny zapis ograniczenia jest taki:

        a[1] y1 + a[2] y2 + a[3] y3 <= b + q s

    W tym zapisie:
        a[] to współczynniki przy zmiennych y
        b to podstawowy limit po prawej stronie
        q to współczynnik mówiący, jak mocno s zwiększa prawą stronę
        s to luz, który zwiększa dostępną prawą stronę

    Do tego książka wyprowadza nierówność MIR:

        y1 - y2 + 0.25 y3 <= 1 + 0.4166666667 s

    MIR to dodatkowe ograniczenie wynikające z tego, że zmienne y są całkowite
    Dodajemy je, żeby wzmocnić model
    To znaczy, że solver ma mniej niepotrzebnych rozwiązań ułamkowych
    do sprawdzania w relaksacji liniowej

    Książka skupia się głównie na zbiorze rozwiązań i nierówności MIR
    Nie podaje jednej konkretnej funkcji celu do obliczeń
    Dlatego w programie dodajemy przykładową funkcję celu:

        max 10 y1 + 2 y2 + 6 y3 - s

    Oznacza to:
        y1 daje zysk 10
        y2 daje zysk 2
        y3 daje zysk 6
        s ma koszt 1

    CPLEX ma więc dobrać wartości y1, y2, y3 oraz s tak, żeby:
        spełnić główne ograniczenie z książki
        spełnić nierówność MIR
        zmaksymalizować funkcję celu

