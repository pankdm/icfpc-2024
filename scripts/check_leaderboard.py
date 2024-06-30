
from dotenv import load_dotenv
from interpreter import ICFPInterpreter
from requests import post, exceptions
import os
import re
from pprint import pprint
import json
from slack_integration import post_to_slack
import time

load_dotenv()

# TMP file - in .gitignore 
path = "./__leaderboard.txt"


class DiffStats:
    def __init__(self):
        self.scores = []
        self.ranks = []

    def has_any(self):
        if len(self.scores) > 0:
            return True
        if len(self.ranks) > 0:
            return True
        return False


def get_scores_for_problem(name):
    interpreter = ICFPInterpreter()
    command = "S" + interpreter.encode_string(f"get {name}")
    try:
        token = os.environ["ICFPC_TOKEN"]
        # print (token)
        resp = post(
            "https://boundvariable.space/communicate",
            data=command,
            headers={"Authorization": f"Bearer {os.environ['ICFPC_TOKEN']}"}
        )
        resp.raise_for_status()
        # print (f"  >> got {resp.text}")
        encoded = interpreter.decode_string(resp.text[1:])
        return encoded
    except exceptions.RequestException as e:
        print("Error: ", e)
        return None

def get_leaderboard_for_problem(name):
    interpreter = ICFPInterpreter()
    if name != "":
        command = "S" + interpreter.encode_string(f"get scoreboard {name}")
    else:
        command = "S" + interpreter.encode_string(f"get scoreboard")

    try:
        token = os.environ["ICFPC_TOKEN"]
        # print (token)
        resp = post(
            "https://boundvariable.space/communicate",
            data=command,
            headers={"Authorization": f"Bearer {os.environ['ICFPC_TOKEN']}"}
        )
        resp.raise_for_status()
        # print (f"  >> got {resp.text}")
        encoded = interpreter.decode_string(resp.text[1:])
        return encoded
    except exceptions.RequestException as e:
        print("Error: ", e)
        return None


def maybe_extract(pattern, s):
    match = re.search(pattern, s)
    if match:
        return match.group(1)
    else:
        return None


def parse_scores(msg, name):
    scores = {}
    prefix = f"[{name}"

    pattern_name = r"\* \[([^\]]+)\]"
    pattern_your = r"Your score: (\d+)"
    pattern_best = r"Best score: (\d+)"

    # Use re.search to find the pattern in the string
    for line in msg.split("\n"):
        problem_name = maybe_extract(pattern_name, line)
        if problem_name is not None:
            your_score = maybe_extract(pattern_your, line) or 0
            best_score = maybe_extract(pattern_best, line) or 0
            # print (f"{problem_name} -> {your_score} (best = {best_score})")
            scores[problem_name] = {
                "your": your_score,
                "best": best_score
            }

    return scores
            
def parse_ranks(msg):
    for line in msg.split("\n"):
        if "Two Smoking Lambdas" in line:
            line = line.replace("*", "").replace(" ", "")
            parts = line.split("|")
            return parts
    return []



def get_new_scores():
    names = ["3d", "lambdaman", "spaceship"]
    # names = ["3d"]
    all_scores = {}
    for name in names:
        msg = get_scores_for_problem(name)
        scores = parse_scores(msg, name)
        all_scores.update(scores)
    # pprint (all_scores)


    all_ranks = {}
    for name in names + [""]:
        msg = get_leaderboard_for_problem(name)
        ranks = parse_ranks(msg)
        all_ranks[name] = ranks

    return {
        "scores": all_scores,
        "ranks": all_ranks,
    }

def get_old_scores():
    if os.path.exists(path):
        with open(path, "r") as f:
            return json.loads(f.read())
    return {
        "scores": {},
        "ranks": {},
    }    

def dump_new_scores(scores):
    print(f'dumping to {path}')
    with open(path, "w") as f:
        f.write(json.dumps(scores, indent=4))

def dump_to_archive(scores):
    current_timestamp = time.time()
    ts = int(current_timestamp)
    archive = f"logs/leaderboard/{ts}.json"
    print(f'dumping to {archive}')
    
    with open(archive, "w") as f:
        f.write(json.dumps(scores, indent=4))


def compute_diff_stats(old, new):
    stats = DiffStats()
    if "scores" in old and "scores" in new:
        stats.scores = compute_diff_scores(old["scores"], new["scores"])

    if "ranks" in old and "ranks" in new:
        stats.ranks = compute_diff_ranks(old["ranks"], new["ranks"])

    return stats

def compute_diff_scores(old, new):
    diff = []
    for prob_id, new_entry in new.items():
        # skip non-existing entries - it means this is a fresh run
        if prob_id not in old:
            continue
        old_entry = old[prob_id]
        if old_entry["your"] != new_entry["your"]:
            diff.append((prob_id, old_entry, new_entry))
    diff.sort()
    return diff

def any_smaller(old, new):
    for (a, b) in zip(new, old):
        try:
            a = int(a)
            b = int(b)
            if a < b:
                return True
        except:
            pass
    return False


def compute_diff_ranks(old, new):
    diff = []
    for name_id, new_entry in new.items():
        # skip non-existing entries - it means this is a fresh run
        if name_id not in old:
            continue
        old_entry = old[name_id]
        if any_smaller(old_entry, new_entry):
            diff.append((name_id, old_entry, new_entry))
    # diff.sort()
    return diff


def send_update_to_slack(diff_stats):
    msgs = [
        ":rocket::rocket:rocket:"
    ]
    if len(diff_stats.scores) > 0:
        lines = []
        for (prob_id, old, new) in diff_stats.scores:
            # Example message: Correct, you solved lambdaman17 with a score of 192!
            # lines.append(f"Correct, you solved {prob_id} with a score of {new["your"]}! (previous={old["your"]}, best={new["best"]})")
        
            # Example message: * [3d1] Your score: 6600. Best score: 2982.
            lines.append(
                f"* [{prob_id}] Your score: {old["your"]} -> {new["your"]}. Best score: {new["best"]}."
            )

        msgs.append("\n".join([
            "New solutions detected:"
            "```",
            "\n".join(lines),
            "```",
        ]))
    if len(diff_stats.ranks) > 0:
        for (name_id, old, new) in diff_stats.ranks:
            lines = []
            lines.extend([
                f"Before: {' | '.join(old)}",
                f"After:  {' | '.join(new)}",
            ])

            msgs.append("\n".join([
                f"Problem `{name_id or "global"}`, rank improvement:"
                "```",
                "\n".join(lines),
                "```",
            ]))


    msg = "\n".join(msgs)
    print (f"msg = {msg}")
    post_to_slack(msg)

def main():
    new_scores = get_new_scores()
    old_scores = get_old_scores()
    diff_stats = compute_diff_stats(old_scores, new_scores)
    
    if diff_stats.has_any() > 0:
        # print ("detected new scores:")
        # pprint(diff_stats.scores)
        # pprint(diff_stats.ranks)

        send_update_to_slack(diff_stats)

    dump_to_archive(new_scores)
    dump_new_scores(new_scores)



if __name__ == "__main__":
    main()
