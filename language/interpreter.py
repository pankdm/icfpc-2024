from dataclasses import dataclass

OFFSET = 33
BASE = 94
FROM_CHARS = "".join(chr(x) for x in range(OFFSET, OFFSET + BASE))
TO_CHARS = """abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!"#$%&'()*+,-./:;<=>?@[\\]^_`|~ \n"""
DECODE = str.maketrans(FROM_CHARS, TO_CHARS)
ENCODE = str.maketrans(TO_CHARS, FROM_CHARS)

class ASTNode:
    pass

class BooleanNode(ASTNode):
    def __init__(self, value):
        self.value = value

    def __str__(self):
        return str(self.value)

class IntegerNode(ASTNode):
    def __init__(self, value):
        self.value = value

    def __str__(self):
        return str(self.value)

class StringNode(ASTNode):
    def __init__(self, value):
        self.value = value

    def __str__(self):
        return repr(self.value)

class UnaryOpNode(ASTNode):
    def __init__(self, op, expr):
        self.op = op
        self.expr = expr

    def __str__(self):
        return f"({self.op} {self.expr})"

class BinaryOpNode(ASTNode):
    def __init__(self, op, left, right):
        self.op = op
        self.left = left
        self.right = right

    def __str__(self):
        return f"({self.op} {self.left} {self.right})"

class IfNode(ASTNode):
    def __init__(self, condition, true_branch, false_branch):
        self.condition = condition
        self.true_branch = true_branch
        self.false_branch = false_branch

    def __str__(self):
        return f"(? {self.condition} {self.true_branch} {self.false_branch})"

class LambdaNode(ASTNode):
    def __init__(self, var_num, body, env=None):
        self.var_num = var_num
        self.body = body
        self.env = env if env is not None else {}

    def __str__(self):
        if self.env:
            env_str = f" {self.env}"
        else:
            env_str = ""
        return f"(λ var_{self.var_num} {self.body}{env_str})"

class VariableNode(ASTNode):
    def __init__(self, var_num):
        self.var_num = var_num

    def __str__(self):
        return f"var_{self.var_num}"

@dataclass
class EvalContext:
    beta_reductions: int = 0
    optimize: bool = False
    debug: bool = False

def div_towards_zero(a, b):
    if a < 0 and b < 0:
        return -a // -b
    elif a < 0:
        return -(-a // b)
    elif b < 0:
        return -(a // -b)
    else:
        return a // b

def mod_towards_zero(a, b):
    if a < 0 and b < 0:
        return -a % -b
    elif a < 0:
        return -(-a % b)
    elif b < 0:
        return -(a % -b)
    else:
        return a % b

@dataclass
class Thunk:
    ast: ASTNode
    environment: dict

class ICFPInterpreter:
    def tokenize(self, program):
        return program.split()

    def parse(self, tokens):
        if not tokens:
            raise ValueError("Empty program")

        token = tokens.pop(0)
        indicator = token[0]
        body = token[1:]

        if indicator == 'T':
            return BooleanNode(True)
        elif indicator == 'F':
            return BooleanNode(False)
        elif indicator == 'I':
            return IntegerNode(self.base94_to_int(body))
        elif indicator == 'S':
            return StringNode(self.decode_string(body))
        elif indicator == 'U':
            return UnaryOpNode(body, self.parse(tokens))
        elif indicator == 'B':
            left = self.parse(tokens)
            right = self.parse(tokens)
            return BinaryOpNode(body, left, right)
        elif indicator == '?':
            condition = self.parse(tokens)
            true_branch = self.parse(tokens)
            false_branch = self.parse(tokens)
            return IfNode(condition, true_branch, false_branch)
        elif indicator == 'L':
            var_num = self.base94_to_int(body)
            body = self.parse(tokens)
            return LambdaNode(var_num, body)
        elif indicator == 'v':
            return VariableNode(self.base94_to_int(body))
        else:
            raise ValueError(f"Unknown token: {token}")

    def evaluate(self, ast, context: EvalContext, environment: dict = None):
        if environment is None:
            environment = {}

        if context.debug:
            print(ast, {f"var_{k}": str(v) for k, v in environment.items()})

        # If ast is already a Python primitive, return it directly
        if isinstance(ast, (bool, int, str)):
            return ast

        if isinstance(ast, BooleanNode):
            return ast.value
        elif isinstance(ast, IntegerNode):
            return ast.value
        elif isinstance(ast, StringNode):
            return ast.value
        elif isinstance(ast, VariableNode):
            if ast.var_num in environment:
                thunk = environment[ast.var_num]
                if isinstance(thunk, Thunk):
                    if context.optimize and hasattr(thunk, 'val'):
                        return thunk.val
                    eval_result = self.evaluate(thunk.ast, context, thunk.environment)
                    thunk.val = eval_result
                    return eval_result
                else:
                    return thunk
            raise ValueError(f"Unbound variable: {ast.var_num}")
        elif isinstance(ast, UnaryOpNode):
            operand = self.evaluate(ast.expr, context, environment)
            if ast.op == '-':
                assert isinstance(operand, int)
                return -operand
            elif ast.op == '!':
                assert isinstance(operand, bool)
                return not operand
            elif ast.op == '#':
                assert isinstance(operand, str)
                return self.base94_to_int(self.encode_string(operand))
            elif ast.op == '$':
                assert isinstance(operand, int)
                return self.decode_string(self.int_to_base94(operand))
        elif isinstance(ast, BinaryOpNode):
            if ast.op == '$':
                func = self.evaluate(ast.left, context, environment)
                if not isinstance(func, LambdaNode):
                    raise ValueError("Expected lambda, got: {func}")
                new_env = func.env.copy()
                new_env[func.var_num] = Thunk(ast.right, environment)
                context.beta_reductions += 1
                return self.evaluate(func.body, context, new_env)
            elif ast.op == '!':
                func = self.evaluate(ast.left, context, environment)
                if not isinstance(func, LambdaNode):
                    raise ValueError("Expected lambda, got: {func}")
                new_env = func.env.copy()
                new_env[func.var_num] = self.evaluate(ast.right, context, environment)
                context.beta_reductions += 1
                return self.evaluate(func.body, context, new_env)
            else:
                left = self.evaluate(ast.left, context, environment)
                right = self.evaluate(ast.right, context, environment)
                if ast.op == '+':
                    assert isinstance(left, int)
                    assert isinstance(right, int)
                    return left + right
                elif ast.op == '-':
                    assert isinstance(left, int)
                    assert isinstance(right, int)
                    return left - right
                elif ast.op == '*':
                    assert isinstance(left, int)
                    assert isinstance(right, int)
                    return left * right
                elif ast.op == '/':
                    assert isinstance(left, int)
                    assert isinstance(right, int)
                    return div_towards_zero(left, right)
                elif ast.op == '%':
                    assert isinstance(left, int)
                    assert isinstance(right, int)
                    return mod_towards_zero(left, right)
                elif ast.op == '<':
                    assert isinstance(left, int)
                    assert isinstance(right, int)
                    return left < right
                elif ast.op == '>':
                    assert isinstance(left, int)
                    assert isinstance(right, int)
                    return left > right
                elif ast.op == '=':
                    assert isinstance(left, int) or isinstance(left, bool) or isinstance(left, str)
                    assert isinstance(right, int) or isinstance(right, bool) or isinstance(right, str)
                    return left == right
                elif ast.op == '|':
                    assert isinstance(left, bool)
                    assert isinstance(right, bool)
                    return left or right
                elif ast.op == '&':
                    assert isinstance(left, bool)
                    assert isinstance(right, bool)
                    return left and right
                elif ast.op == '.':
                    assert isinstance(left, str)
                    assert isinstance(right, str)
                    return left + right
                elif ast.op == 'T':
                    assert isinstance(left, int)
                    assert isinstance(right, str)
                    assert len(right) >= left
                    return right[:left]
                elif ast.op == 'D':
                    assert isinstance(left, int)
                    assert isinstance(right, str)
                    assert len(right) >= left
                    return right[left:]
        elif isinstance(ast, IfNode):
            condition = self.evaluate(ast.condition, context, environment)
            assert isinstance(condition, bool)
            if condition:
                return self.evaluate(ast.true_branch, context, environment)
            else:
                return self.evaluate(ast.false_branch, context, environment)
        elif isinstance(ast, LambdaNode):
            # Capture the current environment in the lambda
            new_env = environment.copy()
            if ast.var_num in new_env:
                del new_env[ast.var_num]
            return LambdaNode(ast.var_num, ast.body, new_env)

        raise ValueError(f"Unknown AST node: {ast}")

    def run(self, program, debug=False, optimize=False):
        tokens = self.tokenize(program)
        ast = self.parse(tokens)
        eval_context = EvalContext(beta_reductions=0, debug=debug, optimize=optimize)
        return self.evaluate(ast, eval_context), eval_context

    def base94_to_int(self, s):
        res = 0
        for c in s:
            res *= BASE
            res += ord(c) - OFFSET
        return res

    def int_to_base94(self, n):
        if n == 0:
            return '!'
        digits = []
        while n:
            digits.append(chr((n % BASE) + OFFSET))
            n //= BASE
        return ''.join(digits[::-1])

    def decode_string(self, s):
        return s.translate(DECODE)

    def encode_string(self, s):
        return s.translate(ENCODE)

def run_fast(code):
    interpreter = ICFPInterpreter()
    import sys
    sys.setrecursionlimit(10000000)
    import threading
    threading.stack_size(2**27)
    result = []
    def run():
        result.append(interpreter.run(code, optimize=True)[0])
    thread = threading.Thread(target=run)
    thread.start()
    thread.join()
    return result[0]

if __name__ == "__main__":
    interpreter = ICFPInterpreter()

    print(interpreter.run('B$ L# B$ L" B+ v" v" B* I$ I# v8'))
    print(interpreter.run('? B> I# I$ S9%3 S./'))
    print(interpreter.run('B$ B$ L# L$ v# B. SB%,,/ S}Q/2,$_ IK'))
    print(interpreter.run('B$ B$ L" B$ L# B$ v" B$ v# v# L# B$ v" B$ v# v# L" L# ? B= v# I! I" B$ L$ B+ B$ v" v$ B$ v" v$ B- v# I" I%', optimize=True))
    code = '''? B= B$ B$ B$ B$ L$ L$ L$ L# v$ I" I# I$ I% I$ ? B= B$ L$ v$ I+ I+ ? B= BD I$ S4%34 S4 ? B= BT I$ S4%34 S4%3 ? B= B. S4% S34 S4%34 ? U! B& T F ? B& T T ? U! B| F F ? B| F T ? B< U- I$ U- I# ? B> I$ I# ? B= U- I" B% U- I$ I# ? B= I" B% I( I$ ? B= U- I" B/ U- I$ I# ? B= I# B/ I( I$ ? B= I' B* I# I$ ? B= I$ B+ I" I# ? B= U$ I4%34 S4%34 ? B= U# S4%34 I4%34 ? U! F ? B= U- I$ B- I# I& ? B= I$ B- I& I# ? B= S4%34 S4%34 ? B= F F ? B= I$ I$ ? T B. B. SM%,&k#(%#+}IEj}3%.$}z3/,6%},!.'5!'%y4%34} U$ B+ I# B* I$> I1~s:U@ Sz}4/}#,!)-}0/).43}&/2})4 S)&})3}./4}#/22%#4 S").!29}q})3}./4}#/22%#4 S").!29}q})3}./4}#/22%#4 S").!29}q})3}./4}#/22%#4 S").!29}k})3}./4}#/22%#4 S5.!29}k})3}./4}#/22%#4 S5.!29}_})3}./4}#/22%#4 S5.!29}a})3}./4}#/22%#4 S5.!29}b})3}./4}#/22%#4 S").!29}i})3}./4}#/22%#4 S").!29}h})3}./4}#/22%#4 S").!29}m})3}./4}#/22%#4 S").!29}m})3}./4}#/22%#4 S").!29}c})3}./4}#/22%#4 S").!29}c})3}./4}#/22%#4 S").!29}r})3}./4}#/22%#4 S").!29}p})3}./4}#/22%#4 S").!29}{})3}./4}#/22%#4 S").!29}{})3}./4}#/22%#4 S").!29}d})3}./4}#/22%#4 S").!29}d})3}./4}#/22%#4 S").!29}l})3}./4}#/22%#4 S").!29}N})3}./4}#/22%#4 S").!29}>})3}./4}#/22%#4 S!00,)#!4)/.})3}./4}#/22%#4 S!00,)#!4)/.})3}./4}#/22%#4'''
    print(interpreter.parse(interpreter.tokenize(code)))
    print(interpreter.run(code, optimize=True))