
from language.interpreter import ICFPInterpreter, run_fast
from inspect import signature
from dataclasses import dataclass, field
from typing import Any, List

interpreter = ICFPInterpreter()

used_variables = 0

@dataclass
class Expr:
    head: List[str] = field(default_factory=list)
    subexprs: List["Expr"] = field(default_factory=list)

    def __add__(self, other):
        return Expr(["B+"], [self, other])
    
    def __mul__(self, other):
        return Expr(["B*"], [self, other])
    
    def __sub__(self, other):
        return Expr(["B-"], [self, other])
    
    def __truediv__(self, other):
        return Expr(["B/"], [self, other])

    def __mod__(self, other):
        return Expr(["B%"], [self, other])
    
    def __lt__(self, other):
        return Expr(["B<"], [self, other])
    
    def __gt__(self, other):
        return Expr(["B>"], [self, other])
    
    def __eq__(self, other):
        return Expr(["B="], [self, other])
    
    def __or__(self, other):
        return Expr(["B|"], [self, other])
    
    def __and__(self, other):
        return Expr(["B&"], [self, other])
    
    def head_(self, other):
        return Expr(["BT"], [other, self])

    def tail(self, other):
        return Expr(["BD"], [other, self])

    def __call__(self, *args, eager=False):
        return Expr(["B!" if eager else "B$"] * len(args), [self, *args])

    def __neg__(self):
        return Expr(["U-"], [self])
    
    def __not__(self):
        return Expr(["U!"], [self])
    
    def s2i(self):
        return Expr(["U#"], [self])
    
    def i2s(self):
        return Expr(["U$"], [self])
    
    def __str__(self):
        all_exprs = []
        def rec(expr):
            if isinstance(expr, str):
                all_exprs.append(f"S{interpreter.encode_string(expr)}")
            elif isinstance(expr, int):
                all_exprs.append(f"I{interpreter.int_to_base94(expr)}")
            elif isinstance(expr, bool):
                all_exprs.append("FT"[expr])
            else:
                all_exprs.extend(expr.head)
                for subexpr in expr.subexprs:
                    rec(subexpr)
        rec(self)
        return " ".join(all_exprs)
    

def lam(body):
    global used_variables
    n_args = len(signature(body).parameters)
    var_is = [interpreter.int_to_base94(used_variables + _) for _ in range(n_args)]
    used_variables += n_args
    body = body(*[Expr([f"v{var_i}"]) for var_i in var_is])
    assert body is not None
    ret = Expr([f"L{var_i}" for var_i in var_is], [body])
    used_variables -= n_args
    return ret

def If(cond, then, els):
    return Expr(["?"], [cond, then, els])

def cat(self, other):
    return Expr(["B."], [self, other])

Yc = lam(lambda f: lam(lambda g: g(g))(lam(lambda x: f(x(x)))))

def str2expr(s):
    return Expr([f"S{interpreter.encode_string(s)}"])

def int2expr(i):
    return Expr([f"I{interpreter.int_to_base94(i)}"])

def bool2expr(b):
    return Expr(["FT"[b]])

if __name__ == "__main__":
    def fib(rec, n):
        return If(n < 2, 1, rec(n - 1) + rec(n - 2))
    fib = Yc(lam(fib))(16)
    print(run_fast(str(fib)))