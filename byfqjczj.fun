c = test-for-arg-c
print argc
argc = 100
print argc
c = robustness-test-for-arm-keywords
main = 10
LDR = 10
LR = 0
ADD = 0
SP = 0
start = 0

c = test - for - basic - logic - operators
x = 1
print x<1
print x==1
print x>1
print x!=1
print x<=1
print x>=1
print 2<=1
print 2>=1

c = test - for - the - ordering - of - the - logics
print 1&&0
print 1&&1||0
print 1||1&&0
print 1&&(1||0)
print 1&&1&&1&&1||0&&1
print 1||0&&1
print 0&&1
print (0+1)||0

c = if - and - while - loop - basic - test
if 1 == 0 {
    print 10
}
else {
    print 11
}
x = 0
while x < 100 {
    print x
    x = x + 1
}
y = 0
while y < 100 {
    if y % 2 == 1 {
        print y
    }
    y = y + 1
}
a = 100
while a < 0 {
    print a
}  
c = normal - fnctions
randomArithmatic = fun {
    print 1
    it = it + 3
    return it
}
print(randomArithmatic(10))
randomArithmaticTwo = fun {
    it=0
    return 0
}
print(randomArithmaticTwo(10))
c = recursive - fnctions
toStart = 1
threeToThePower = fun {
    if it == 0 {
        return toStart
    }
    else {
        return 3 * threeToThePower(it - 1)
    }
}
print threeToThePower(5)

c = interrelated - fnctions
SomeArithmatic = fun {
    it = it + 1
    it = it * 2
    it = it + 1
    it = it + 3
    return it
}
SomeMoreArithmatic = fun {
    it = it + 2
    it = it * 4
    it = it - 3
    it = SomeArithmatic(it)
    return it
}
print SomeMoreArithmatic(10)

c = combining - fnctions - while - loops - and - if - statements
holder = 0
counter = 0

EvenDigitSum = fun {
    while it != 0 {
        holder = it % 10
        if holder % 2 == 0 {
            counter = counter + holder
        }
        it = it - holder
        it = it / 10   
    }
    return counter
}

print EvenDigitSum(444555666)