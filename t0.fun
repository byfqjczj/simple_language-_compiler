fact = fun {
   out = 0
   while (it > 0) {
       out = out + it
       it = it - 1
   }
   return out
}

print fact(100000)

