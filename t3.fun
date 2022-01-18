f = fun {
  print(it + it*10 + it*100)
  if it&1 
    if it&2
      print 2
    else
      print 3
}

g = fun {
  print(it + it*10 + it*100)
  if it&1 {
    if it&2
      print 2 }
    else
      print 3
}

a = f(0)
a = f(1)
a = f(2)
a = f(3)

a = g(0)
a = g(1)
a = g(2)
a = g(3)



