import os
import json
import logging
import subprocess
from concurrent.futures import ThreadPoolExecutor
from dotenv import load_dotenv
from flask_cors import CORS
from flask import Flask, Response, request, send_from_directory

# from scripts.problem_stats import get_estimated_scores, get_our_best_scores, get_problem_stats
# from solvers.python import hello_json
from .api import icfpc as ICFPC
from .utils import get_sanitized_args, cached
from language.interpreter import ICFPInterpreter
# import numpy as np

icfpc_interpreter = ICFPInterpreter()

logger = logging.getLogger(__name__)

load_dotenv()

app = Flask(__name__)
CORS(app)

def root_folder_path(path):
    return os.path.dirname(__file__)+'/../' + path


@app.route("/")
def ping():
    return "pong"


@app.post("/check-auth")
def post_check_auth():
    return ICFPC.check_auth()

@app.post("/run_solver")
def post_run_solver():
    payload = request.get_json()
    problem_id = payload.get("problem_id")
    lang = payload["lang"]
    solver = payload.get("solver", 'main')
    args = payload.get("args", '')

    if lang == 'cpp':
        done = subprocess.run(
            [f'./build_run_and_submit.sh', f'./{solver}.solver', *args.split(' ')],
            cwd='solvers/cpp',
            capture_output=True,
            text=True
        )
        return { "output": str(done.stdout), "stderr": str(done.stderr), "code": done.returncode }
    elif lang == 'python':
        done = subprocess.run(
            ['python', f'solvers/python/{solver}/solver.py', str(problem_id)],
            capture_output=True,
            text=True
        )
        return { "output": str(done.stdout), "code": done.returncode }

def run_script_example():
    payload = request.get_json()
    args = payload.get("args", '')
    done = subprocess.run(
        [f'./script.sh', *args.split(' ')],
        cwd='scripts',
        capture_output=True,
        text=True
    )
    return { "output": str(done.stdout), "stderr": str(done.stderr), "code": done.returncode }

def get_problem_ids():
    problems_dir = os.path.dirname(__file__)+'/../problems'
    problems_files = os.listdir(problems_dir)
    problems_ids = sorted([int(p.rstrip('.json'))
                      for p in problems_files if '.json' in p])
    return problems_ids

@app.get("/problems")
@app.get("/problems/")
def get_problems():
    return { 'problems': get_problem_ids() }

@cached(ttl=60)
def get_cached_problem_stats():
    stats = get_problem_stats()
    estimated_max = get_estimated_scores()
    our_best = get_our_best_scores()
    scores = ICFPC.get_cached_userboard()
    merged_stats = {
        id: {
            **stats[id],
            "score": scores[id] if id in scores else -0,
            "estimated_max": estimated_max.get(id, -1),
            "our_best": our_best.get(id, {})
        }
        for id in stats.keys()
    }
    return merged_stats

@app.post("/decode")
def post_decode():
    text = request.get_json().get('text')
    return icfpc_interpreter.run(text)[0]

@app.post("/encode")
def post_encode():
    text = request.get_json().get('text')
    return "S" + icfpc_interpreter.encode_string(text)

@app.post("/communicate")
def post_communicate():
    text = request.get_json().get('text')
    return ICFPC.send_raw_msg(text)

@app.get("/problems/stats")
def handle_get_problems_stats():
    return { 'problems': get_cached_problem_stats() }

@app.get("/problems/<track>/<id>")
def get_problem(id):
    return send_from_directory(f'../problems/{track}', id+'.json')

@app.get("/problems/<id>/preview")
def get_problem_preview(id):
    return send_from_directory('../previews_best', id+'.svg')

@app.get("/problems/<id>/stats")
def handle_get_problem_id_stats(id):
    stats = get_cached_problem_stats()
    return stats[int(id)]

@app.get("/problems/total")
def get_total_available_problems():
    return {
        "total_problems": ICFPC.get_number_of_problems()
    }

@app.get("/problems/download/<track>/<start>/<end>")
def get_download_problems(track, start, end):
    start, end = int(start), int(end)
    for id in range(start, end+1):
        msg = 'S'+icfpc_interpreter.encode_string(f'get {track}{id}')
        resp = ICFPC.send_raw_msg(msg)
        with open(f'{root_folder_path("problems/")}{track}/raw_{track}{id}.txt', 'w') as file:
            file.write(resp)
    return 'downloaded'

@app.get("/problems/<id>/download")
def get_download_problem(id):
    problem = ICFPC.get_problem(id)
    with open(f'{root_folder_path("problems/")}{id}.json', 'w') as file:
        file.write(json.dumps(problem, indent=2))
    return problem

def get_all_solutions(nickname=None, problem_id=None):
    solutions_dir = os.path.dirname(__file__)+'/../solutions'
    solutions_folders_with_files = []
    for path, folders, files in os.walk(solutions_dir):
        solutions_folders_with_files.append(
            (path[len(solutions_dir + '/'):], files))
    solutions = []
    for path, files in solutions_folders_with_files:
        for file in files:
            solutions.append(path+'/'+file if path else file)
    return [
        s for s in sorted(solutions)
        if (nickname in s if nickname else True)
        and (f'/{problem_id}.json' in s if problem_id else True)
    ]

@app.get("/solutions")
@app.get("/solutions/")
def get_solutions():
    return {'solutions': sorted(get_all_solutions())}

@app.get("/scoreboard")
def get_scoreboard():
    return ICFPC.get_scoreboard()

@app.get("/userboard")
def get_userboard():
    return ICFPC.get_cached_userboard()

@app.get("/solutions/<nickname>")
@app.get("/solutions/<nickname>/")
def get_user_solutions(nickname):
    return {'solutions': sorted(get_all_solutions(nickname=nickname))}

@app.get("/solutions/<path:path>")
def get_solution(path):
    print(path)
    return send_from_directory('../solutions', path)

@app.get("/problems/<problem_id>/solutions")
def handle_get_problem_solutions(problem_id):
    return { "solutions": get_all_solutions(problem_id=problem_id) }

@app.get("/problems/<problem_id>/solutions/<nickname>")
def handle_get_problem_solutions_by_user(nickname, problem_id):
    return { "solutions": get_all_solutions(nickname=nickname, problem_id=problem_id) }

@app.post("/solutions/<username>/<problem_id>")
def post_solution(username, problem_id):
    return ICFPC.submit(username, problem_id)

@app.post("/solutions/<username>")
def post_all_solutions(username):
    return ICFPC.submit_all(username)

@app.get("/best_solutions/<path:path>")
def get_best_solution(path):
    print('get_best_solution::', path)
    solutions_dir = os.path.dirname(__file__)+'/../solutions_best'
    timestamps = os.listdir(solutions_dir)
    last_ts = max(timestamps)
    print (last_ts)

    for solution in os.listdir(f"{solutions_dir}/{last_ts}"):
        if solution.startswith(f"{path}_"):
            print ('Best = ', solution)
            return send_from_directory(f"{solutions_dir}/{last_ts}", solution)
    return ""

@app.get('/icfpc/<path:path>')
def get_icfpc_endpoint(path):
    result = ICFPC.proxy_get_endpoint(path)
    return result

@app.errorhandler(Exception)
def unhandled_error(error):
    logging.exception(error)
    return str(error), getattr(error, 'code', 500)
