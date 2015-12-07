def foo():
    return 16

def x():
    return foo(24)

def bar():
    x = 1
    y = 2
    z = 3
    return x() + foo()
