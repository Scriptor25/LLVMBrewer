# Compute the fibonacci number of x. #
def fib(x)
    if x < 3 then 1
    else fib(x - 1) + fib(x - 2)

# This expression will compute the 40th number. #
fib(40)
