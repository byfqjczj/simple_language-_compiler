f1 = fun {
    print it
    z = f2(it+1)
    print it
}

f2 = fun {
    print it
}

z = f1(10)

