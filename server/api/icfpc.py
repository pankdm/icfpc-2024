from dataclasses import dataclass
from datetime import datetime, timezone, timedelta
from dateutil.parser import parse as dateparse
from ..utils import cached
import os
import json
import requests
import requests.auth
from language.interpreter import ICFPInterpreter

from time import time
import os
from collections import defaultdict

from dotenv import load_dotenv
load_dotenv()

WEBSITE_ROOT = 'https://www.icfpcontest.com'
API_ROOT = 'https://boundvariable.space'
CDN_ROOT = 'https://cdn.icfpcontest.com'
PROBLEMS_URL_TEMPLATE = 'https://cdn.icfpcontest.com/problems/{problem_id}.json'
ICFPC_USER_EMAIL = os.environ.get('ICFPC_USER_EMAIL')
ICFPC_USER_PASSWORD = os.environ.get('ICFPC_USER_PASSWORD')

@dataclass
class Client:
    email:str = None
    token:str = None
    expires:datetime = None
    session:requests.Session = None

common_session = requests.Session()
icfpc_client = Client()

def login(email, password):
    response = requests.post(API_ROOT+'/login', json={ "email": email, "password": password })
    response.raise_for_status()
    return response.json()

def ensure_auth():
    if icfpc_client.email == ICFPC_USER_EMAIL and icfpc_client.token and icfpc_client.expires and icfpc_client.expires > datetime.now(tz=timezone.utc):
        return
    token = login(ICFPC_USER_EMAIL, ICFPC_USER_PASSWORD)
    icfpc_client.email = ICFPC_USER_EMAIL
    icfpc_client.token = token
    icfpc_client.expires = datetime.now(tz=timezone.utc) + timedelta(hours=1)
    icfpc_client.session = requests.Session()
    icfpc_client.session.headers.update({'Authorization': f'Bearer {icfpc_client.token}'})

def send_raw_msg(encoded_msg):
    ensure_auth()
    request_size = len(encoded_msg)
    print (f"  >> command size = {request_size}")
    if (request_size > 10**6):
        raise Exception("Error: Request is too large!")
    response = icfpc_client.session.post(
        "https://boundvariable.space/communicate",
        data=encoded_msg
    )
    response.raise_for_status()
    return response.text

def communicate(encoded_msg):
    return send_raw_msg(encoded_msg)

def send_msg(msg):
    interpreter = ICFPInterpreter()
    encoded_msg = 'S' + interpreter.encode_string(msg)
    response = send_raw_msg(encoded_msg)
    return interpreter.run(response)[0]

def submit(username: str, problem: str):
    ensure_auth()
    with open(f'solutions/{username}/{problem}.json', 'r') as file:
        return submit_solution(problem, file.read())

def submit_all(username: str):
    ensure_auth()

    submitted = 0
    with os.scandir(f'solutions/{username}') as dir:
        for entry in dir:
            if not entry.name.startswith('.') and entry.is_file():
                problem, _ = os.path.splitext(os.path.basename(entry))
                with open(entry, 'r') as file:
                    submit_solution(problem, file.read())
                    submitted += 1

    return f'{submitted}'

def submit_solution(problem: str, solution: str):
    response = icfpc_client.session.post(API_ROOT+'/submission', json={ "problem_id": int(problem), "contents": solution })
    if response.status_code >= 300:
        print(response.text)
        response.raise_for_status()
    return response.text

def get_scoreboard():
    # ensure_auth()
    response = common_session.get(API_ROOT+f'/scoreboard')
    response.raise_for_status()
    return response.json()

@cached(ttl=60)
def get_cached_scoreboard():
    return get_scoreboard()

def get_userboard():
    ensure_auth()
    response = icfpc_client.session.get(API_ROOT+f'/userboard')
    response.raise_for_status()
    data = response.json()
    return data

@cached(ttl=60)
def get_cached_userboard():
    return get_userboard()

def request_get(path):
    ensure_auth()
    response = icfpc_client.session.get(API_ROOT+'/'+path)
    response.raise_for_status()
    return response.json()

def request_post(path, json=None):
    ensure_auth()
    response = icfpc_client.session.post(API_ROOT+'/'+path, json=json)
    response.raise_for_status()
    return response.json()

def check_auth():
    ensure_auth()
    return { 'user': icfpc_client.email, 'expires': icfpc_client.expires.isoformat() }

# def get_number_of_problems():
#     from bs4 import BeautifulSoup
#     webpage = common_session.get(WEBSITE_ROOT+f'/problems')
#     webpage.raise_for_status()
#     soup = BeautifulSoup(webpage.text, 'html.parser')
#     script = soup.find('script', { "id": "__NEXT_DATA__" })
#     data = json.loads(script.text)
#     return data['props']['pageProps']['numberOfProblems']

def get_problem(id):
    response = common_session.get(CDN_ROOT+f'/problems/{id}.json')
    response.raise_for_status()
    print(f'downloaded problem {id}')
    return response.json()

def get_submission(id):
    ensure_auth()
    response = icfpc_client.session.get(API_ROOT+f'/submissions/{id}')
    response.raise_for_status()
    return response.json()


def download_best_submissions(check_ts=True):
    ensure_auth()

    res = get_submissions()
    # print(res)

    ts = int(time())
    solutions_dir = os.path.dirname(__file__)+'/../../best_solutions'
    timestamps = os.listdir(solutions_dir)
    last_ts = int(max(timestamps))
    print (last_ts)

    # go to server only every 2 minutes
    if check_ts and int(last_ts) + 120 > ts:
        print (f"Last download was too early: {ts - last_ts} seconds ago, exiting")
        return


    existing = defaultdict(int)
    for solution in os.listdir(f"{solutions_dir}/{last_ts}"):
        name = solution.split(".")[0]
        problem_id, score = name.split("_")[:2]
        existing[problem_id] = int(score)

    all_problems = defaultdict(list)
    for submit_info in res['submissions']:
        problem_id = submit_info['problem_id']
        # print (submit_info)
        if 'score' not in submit_info:
            continue
        if submit_info['status'] != 'SUCCEEDED':
            continue
        score = submit_info['score']
        submit_id = submit_info['id']
        all_problems[str(problem_id)].append((score, submit_id))

    need_download = False
    total = 0
    for problem_id, scores in all_problems.items():
        (score, submit_id) = min(scores)
        if problem_id not in existing or existing[problem_id] > score:
            print (f"problem {problem_id} is better: {score} vs {existing[problem_id]}")
            if existing[problem_id] > 0:
                total += existing[problem_id] - score
            need_download = True
    print (f"total delta: {total}")
    print ("")

    if not need_download:
        print ("Nothing new to download, exiting...")
        return

    # print (all_problems)

    folder = os.path.dirname(__file__)+f'/../../best_solutions/{ts}'
    os.mkdir(folder)

    for problem_id, scores in all_problems.items():
        (score, submit_id) = min(scores)
        detailed_info = get_submission(submit_id)
        # print (problem_id, score, id, detailed_info)
        url = detailed_info['file_url']
        code_response = requests.get(url)
        content = code_response.content.decode()
        print (f'Downloading {submit_id} as {problem_id}_{score}.txt')
        with open(f"{folder}/{problem_id}_{score}.txt", "wt") as f:
            f.write(content)


        best = os.path.dirname(__file__)+f'/../../solutions/best/'
        os.system(f"mkdir -p {best}")
        with open(f"{best}/{problem_id}.txt", "wt") as f:
            f.write(content)


@cached(ttl=600)
def get_cached_best_solutions():
    download_best_submissions()
