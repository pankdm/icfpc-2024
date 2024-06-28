import os
import types
import importlib

from dotenv import load_dotenv
load_dotenv()

from server import server
from server.api import icfpc as ICFPC
import scripts
import scripts.slack_integration as slack

modules = {
  "ICFPC": ICFPC,
  "server": server,
  "scripts": scripts,
  "slack": slack,
}

def reload_package(package, max_depth=4):
    if not hasattr(module, "__package__"):
        return
    print('reloading', package.__name__)
    importlib.reload(module)
    if max_depth <= 0:
        return
    for module_child in vars(module).values():
        if not hasattr(module_child, "__package__"):
            continue
        reload_package(module_child, depth=depth+1)


def reload_modules():
  for _, m in modules.items():
    reload_package(m)

print('Welcome to ICFPC-2023!')
print('Team: Snakes, Monkeys and Two Smoking Lambdas')
print('')
print('Modules:')
for repl_key, m in modules.items():
  print(f'  - {repl_key}')
