
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
            

def get_new_scores():
    names = ["3d", "lambdaman", "spaceship"]
    # names = ["3d"]
    all_scores = {}
    for name in names:
        msg = get_scores_for_problem(name)
        scores = parse_scores(msg, name)
        all_scores.update(scores)
    # pprint (all_scores)
    return all_scores

def get_old_scores():
    if os.path.exists(path):
        with open(path, "r") as f:
            return json.loads(f.read())
    return {}    

def dump_new_scores(scores):
    with open(path, "w") as f:
        f.write(json.dumps(scores, indent=2))

def dump_to_archive(scores):
    current_timestamp = time.time()
    ts = int(current_timestamp)
    archive = f"logs/leaderboard/{ts}.json"
    print(f'dumping to {archive}')
    
    with open(archive, "w") as f:
        f.write(json.dumps(scores, indent=2))


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

def send_update_to_slack(diff_scores):
    lines = []
    for (prob_id, old, new) in diff_scores:
        # Example message: Correct, you solved lambdaman17 with a score of 192!
        # lines.append(f"Correct, you solved {prob_id} with a score of {new["your"]}! (previous={old["your"]}, best={new["best"]})")
    
        # Example message: * [3d1] Your score: 6600. Best score: 2982.
        lines.append(
            f"* [{prob_id}] Your score: {old["your"]} -> {new["your"]}. Best score: {new["best"]}."
        )


    msg = "\n".join([
        "New solutions detected:"
        "```",
        "\n".join(lines),
        "```",
    ])
    print (f"msg = {msg}")
    post_to_slack(msg)

def main():
    new_scores = get_new_scores()
    old_scores = get_old_scores()
    diff_scores = compute_diff_scores(old_scores, new_scores)
    
    if len(diff_scores) > 0:
        print ("detected new scores:")
        pprint(diff_scores)
        send_update_to_slack(diff_scores)

    dump_to_archive(new_scores)
    dump_new_scores(new_scores)



if __name__ == "__main__":
    main()